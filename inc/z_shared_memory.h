#ifndef _Z_SHARED_MEMORY_H_
#define _Z_SHARED_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <sys/shm.h>
#include "z_kfifo.h"

// 共享内存相关的常量定义
#define Z_SHM_PROJ_ID_FIFO 'F'          // 用于fifo的project id
#define Z_SHM_PROJ_ID_DATA 'D'          // 用于data的project id

// 共享内存访问模式
#define Z_SHM_CREATE 0x01    // 创建者模式
#define Z_SHM_GET    0x02    // 获取者模式

// Shared memory structure
struct z_shared_memory_struct {
    struct z_kfifo_struct *p_fifo;    /* kfifo结构体指针 */
    void *buffer_addr;                /* buffer共享内存地址 */
    int shm_id_fifo;                  /* fifo结构体共享内存ID */
    int shm_id_data;                  /* 数据buffer共享内存ID */
    char name[64];                /* 共享内存名称 */
};

// Initialize shared memory
int z_shared_memory_init(struct z_shared_memory_struct *p_shm, 
                        uint32_t size,
                        const char *name,
                        int flags);  // 添加标志参数

// Send data to shared memory with timeout
int z_shared_memory_send(struct z_shared_memory_struct *p_shm, 
                        const void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms);

// Receive data from shared memory with timeout
int z_shared_memory_recv(struct z_shared_memory_struct *p_shm, 
                        void *p_data, 
                        uint32_t len, 
                        uint32_t timeout_ms);

// Free shared memory resources
void z_shared_memory_free(struct z_shared_memory_struct *p_shm,
                         int flags);  // 添加标志参数

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_SHARED_MEMORY_H_ */
