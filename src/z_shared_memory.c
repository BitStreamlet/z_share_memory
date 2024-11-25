#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "z_shared_memory.h"
#include "z_debug.h"
#include "z_tool.h"

int z_shared_memory_init(struct z_shared_memory_struct *p_shm, 
                        uint32_t size,
                        const char *name,
                        int flags) 
{
    key_t key;
    void *shm_addr;
    int total_size;
    uint32_t aligned_struct_size;
    uint32_t aligned_buffer_size;
    
    if (!p_shm || !name || size == 0) {
        return -EINVAL;
    }

    // 保存共享内存名称
    strncpy(p_shm->name, name, sizeof(p_shm->name) - 1);
    p_shm->name[sizeof(p_shm->name) - 1] = '\0';

    // 对齐 kfifo 结构体大小和 buffer 大小
    aligned_struct_size = Z_TOOL_ALIGN_SYS(sizeof(struct z_kfifo_struct));
    aligned_buffer_size = z_tool_roundup_pow_of_two(size);

    // 计算需要的总内存大小：对齐后的kfifo结构体 + 对齐后的buffer大小
    total_size = aligned_struct_size + aligned_buffer_size;

    // 生成key
    key = ftok(name, Z_SHM_PROJ_ID_FIFO);
    if (key == -1) {
        Z_ERROR("ftok failed: %s\n", strerror(errno));
        return -errno;
    }

    if (flags & Z_SHM_CREATE) {
        // 创建共享内存
        p_shm->shm_id = shmget(key, total_size, IPC_CREAT | IPC_EXCL | 0666);
        if (p_shm->shm_id == -1) {
            Z_ERROR("shmget failed: %s\n", strerror(errno));
            return -errno;
        }
    } else {
        // 获取已存在的共享内存
        p_shm->shm_id = shmget(key, total_size, 0666);
        if (p_shm->shm_id == -1) {
            Z_ERROR("shmget failed: %s\n", strerror(errno));
            return -errno;
        }
    }

    // 映射共享内存
    shm_addr = shmat(p_shm->shm_id, NULL, 0);
    if (shm_addr == (void *)-1) {
        Z_ERROR("shmat failed: %s\n", strerror(errno));
        if (flags & Z_SHM_CREATE) {
            shmctl(p_shm->shm_id, IPC_RMID, NULL);
        }
        return -errno;
    }

    // 设置kfifo结构体指针
    p_shm->p_fifo = (struct z_kfifo_struct *)shm_addr;
    
    // 设置buffer偏移量 (使用对齐后的结构体大小)
    p_shm->fifo_buffer_offset = aligned_struct_size;
    p_shm->p_buf_addr = (unsigned char *)shm_addr + p_shm->fifo_buffer_offset;
    // 如果是创建模式，初始化kfifo
    if (flags & Z_SHM_CREATE) {
        // 初始化kfifo，buffer地址为共享内存起始地址加上偏移量
        z_kfifo_init(p_shm->p_fifo, p_shm->p_buf_addr, aligned_buffer_size);
    }

    return 0;
}

void z_shared_memory_free(struct z_shared_memory_struct *p_shm, int flags)
{
    if (!p_shm) {
        return;
    }

    if (p_shm->p_fifo) {
        shmdt(p_shm->p_fifo);
        p_shm->p_fifo = NULL;
    }

    if ((flags & Z_SHM_CREATE) && p_shm->shm_id >= 0) {
        shmctl(p_shm->shm_id, IPC_RMID, NULL);
    }
    
}

// Send data to shared memory with timeout
int z_shared_memory_send(struct z_shared_memory_struct *p_shm, 
                        const void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms) {
    if (!p_shm || !p_data || len == 0 || !p_shm->p_fifo) {
        return -EINVAL;
    }

    uint32_t elapsed = 0;
    const uint32_t sleep_interval = 1;

    while (elapsed < timeout_ms) {
        if (z_kfifo_space(p_shm->p_fifo) >= len) {
            uint32_t written = z_kfifo_shm_in(p_shm->p_fifo, p_data, len, p_shm->p_buf_addr);
            return (written == len) ? 0 : -EIO;
        }
        usleep(sleep_interval * 1000);
        elapsed += sleep_interval;
    }

    return -ETIMEDOUT;
}

// Receive data from shared memory with timeout
int z_shared_memory_recv(struct z_shared_memory_struct *p_shm, 
                        void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms) {
    if (!p_shm || !p_data || len == 0 || !p_shm->p_fifo) {
        return -EINVAL;
    }

    uint32_t elapsed = 0;
    const uint32_t sleep_interval = 1;

    while (elapsed < timeout_ms) {
        if (z_kfifo_data_len(p_shm->p_fifo) >= len) {
            uint32_t read = z_kfifo_shm_out(p_shm->p_fifo, p_data, len, p_shm->p_buf_addr);
            return (read == len) ? 0 : -EIO;
        }
        usleep(sleep_interval * 1000);
        elapsed += sleep_interval;
    }

    return -ETIMEDOUT;
}