#include "z_tool.h"
#include "z_kfifo.h"
#include "z_debug.h"
#include "z_shared_memory.h"
#include "z_table_print.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>

#define Z_SHARED_MEMORY_VERSION "0.0.1.0"
#define SHM_PROJ_ID_FIFO Z_SHM_PROJ_ID_FIFO
#define SHM_PROJ_ID_DATA Z_SHM_PROJ_ID_DATA

// Initialize shared memory
int z_shared_memory_init(struct z_shared_memory_struct *p_shm, 
                        uint32_t size,
                        const char *name,
                        int flags) {
    // 参数检查
    if (!p_shm || size == 0 || !name) return -1;
    if (!(flags & (Z_SHM_CREATE | Z_SHM_GET))) return -1;
    if (strlen(name) >= sizeof(p_shm->name)) {
        Z_RAW("Name too long\n");
        return -1;
    }

    // 初始化结构体
    memset(p_shm, 0, sizeof(struct z_shared_memory_struct));
    p_shm->shm_id_fifo = -1;
    p_shm->shm_id_data = -1;

    // 生成路径和名称
    strncpy(p_shm->name, name, sizeof(p_shm->name) - 1);

    char shm_path[256];
    snprintf(shm_path, sizeof(shm_path), "/tmp/%s_shm", name);

    // 创建者模式特有的初始化
    if (flags & Z_SHM_CREATE) {
        FILE *fp = fopen(shm_path, "w");
        if (!fp) {
            Z_RAW("Failed to create key file: %s\n", strerror(errno));
            return -1;
        }
        fprintf(fp, "shared memory key file");
        fclose(fp);
        chmod(shm_path, 0666);
    }

    // 生成共享内存key
    key_t key_fifo = ftok(shm_path, SHM_PROJ_ID_FIFO);
    key_t key_data = ftok(shm_path, SHM_PROJ_ID_DATA);
    if (key_fifo == -1 || key_data == -1) {
        Z_RAW("Failed to create keys: %s\n", strerror(errno));
        goto error_exit;
    }

    // 共享内存操作标志
    int shmflg = (flags & Z_SHM_CREATE) ? (IPC_CREAT | 0666) : 0666;
    size = z_tool_roundup_pow_of_two(size);  // 确保size是4K对齐的

    // 创建/获取共享内存
    p_shm->shm_id_fifo = shmget(key_fifo, sizeof(struct z_kfifo_struct), shmflg);
    if (p_shm->shm_id_fifo < 0) {
        Z_RAW("Failed to %s fifo shared memory: %s\n", 
              (flags & Z_SHM_CREATE) ? "create" : "get", strerror(errno));
        goto error_exit;
    }

    p_shm->shm_id_data = shmget(key_data, size, shmflg);
    if (p_shm->shm_id_data < 0) {
        Z_RAW("Failed to %s data shared memory: %s\n",
              (flags & Z_SHM_CREATE) ? "create" : "get", strerror(errno));
        goto error_exit_fifo;
    }

    // 映射共享内存
    p_shm->p_fifo = shmat(p_shm->shm_id_fifo, NULL, 0);
    if (p_shm->p_fifo == (void *)-1) {
        Z_RAW("Failed to attach fifo shared memory: %s\n", strerror(errno));
        goto error_exit_data;
    }

    p_shm->buffer_addr = shmat(p_shm->shm_id_data, NULL, 0);
    if (p_shm->buffer_addr == (void *)-1) {
        Z_RAW("Failed to attach data shared memory: %s\n", strerror(errno));
        goto error_exit_fifo_attach;
    }

    // 创建者初始化kfifo
    if (flags & Z_SHM_CREATE) {
        z_kfifo_init(p_shm->p_fifo, 0, size);
    }

    return 0;

error_exit_fifo_attach:
    shmdt(p_shm->p_fifo);
error_exit_data:
    shmctl(p_shm->shm_id_data, IPC_RMID, NULL);
error_exit_fifo:
    shmctl(p_shm->shm_id_fifo, IPC_RMID, NULL);
error_exit:
    return -1;
}

// Send data to shared memory with timeout
int z_shared_memory_send(struct z_shared_memory_struct *p_shm, 
                        const void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms) {
    if (!p_shm || !p_data || len == 0 || !p_shm->p_fifo || !p_shm->buffer_addr) {
        return -1;
    }

    uint32_t elapsed = 0;
    const uint32_t sleep_interval = 1;

    while (elapsed < timeout_ms) {
        if (z_kfifo_space(p_shm->p_fifo) >= len) {
            uint32_t written = z_kfifo_in(p_shm->p_fifo, p_data, len, p_shm->buffer_addr);
            return (written == len) ? 0 : -1;
        }
        usleep(sleep_interval * 1000);
        elapsed += sleep_interval;
    }

    return -1;  // 超时
}

// Receive data from shared memory with timeout
int z_shared_memory_recv(struct z_shared_memory_struct *p_shm, 
                        void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms) {
    if (!p_shm || !p_data || len == 0 || !p_shm->p_fifo || !p_shm->buffer_addr) {
        return -1;
    }

    uint32_t elapsed = 0;
    const uint32_t sleep_interval = 1;

    while (elapsed < timeout_ms) {
        if (z_kfifo_data_len(p_shm->p_fifo) >= len) {
            uint32_t read = z_kfifo_out(p_shm->p_fifo, p_data, len, p_shm->buffer_addr);
            return (read == len) ? 0 : -1;
        }
        usleep(sleep_interval * 1000);
        elapsed += sleep_interval;
    }

    return -1;  // 超时
}

// Free shared memory resources
void z_shared_memory_free(struct z_shared_memory_struct *p_shm, int flags) {
    if (!p_shm) return;

    // 释放共享内存
    if (p_shm->buffer_addr && p_shm->buffer_addr != (void *)-1) {
        shmdt(p_shm->buffer_addr);
        p_shm->buffer_addr = NULL;
    }
    if (p_shm->p_fifo && p_shm->p_fifo != (void *)-1) {
        shmdt(p_shm->p_fifo);
        p_shm->p_fifo = NULL;
    }

    // 只有创建者才删除共享内存
    if (flags & Z_SHM_CREATE) {
        if (p_shm->shm_id_data >= 0) {
            shmctl(p_shm->shm_id_data, IPC_RMID, NULL);
            p_shm->shm_id_data = -1;
        }
        if (p_shm->shm_id_fifo >= 0) {
            shmctl(p_shm->shm_id_fifo, IPC_RMID, NULL);
            p_shm->shm_id_fifo = -1;
        }

        // 删除key文件
        if (p_shm->name[0] != '\0') {
            char shm_path[256] = {0};
            snprintf(shm_path, sizeof(shm_path) - 1, "/tmp/%s_shm", p_shm->name);
            unlink(shm_path);
        }
    }

    // 清空结构体
    memset(p_shm, 0, sizeof(struct z_shared_memory_struct));
    p_shm->shm_id_fifo = -1;
    p_shm->shm_id_data = -1;
}
