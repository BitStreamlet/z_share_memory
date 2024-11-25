#include <stdint.h>
#include "z_shared_memory.h"
#include "z_debug.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE (4 * 1024)  // 4KB buffer

// 数据包头结构
struct packet_header {
    uint32_t magic;      // 魔数，用于校验
    uint32_t length;     // 数据长度
    uint32_t seq;        // 序列号
};

#define PACKET_MAGIC 0x12345678

int main() {
    struct z_shared_memory_struct shm;
    int ret;
    char data_buffer[BUFFER_SIZE];

    // 接收端作为获取者
    ret = z_shared_memory_init(&shm, BUFFER_SIZE, "test", Z_SHM_GET);
    if (ret != 0) {
        Z_RAW("Failed to initialize shared memory\n");
        return -1;
    }

    Z_RAW("Receiver process initialized (PID: %d)\n", getpid());
    Z_RAW("Shared Memory ID: %d\n", shm.shm_id);

    while (1) {
        struct packet_header header;

        // 接收包头
        ret = z_shared_memory_recv(&shm, &header, sizeof(header), 2000);
        if (ret != 0) {
            Z_RAW("Timeout or error receiving header\n");
            continue;
        }

        // 验证魔数
        if (header.magic != PACKET_MAGIC) {
            Z_RAW("Invalid packet magic: 0x%08X\n", header.magic);
            continue;
        }

        // 检查是否是结束标记
        if (header.length == 0) {
            Z_RAW("Received end marker, seq: %u\n", header.seq);
            break;
        }

        // 检查数据长度
        if (header.length > BUFFER_SIZE) {
            Z_RAW("Data length too large: %u\n", header.length);
            continue;
        }

        // 接收数据
        ret = z_shared_memory_recv(&shm, data_buffer, header.length, 2000);
        if (ret != 0) {
            Z_RAW("Failed to receive data, seq: %u\n", header.seq);
            continue;
        }

        Z_RAW("Received message %u: '%s' (length: %u)\n", 
              header.seq, data_buffer, header.length);
    }

    // 作为获取者清理资源
    z_shared_memory_free(&shm, Z_SHM_GET);

    Z_RAW("Receiver process completed\n");
    return 0;
} 