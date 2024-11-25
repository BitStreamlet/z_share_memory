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
    uint32_t seq = 0;

    // 发送端作为创建者
    ret = z_shared_memory_init(&shm, BUFFER_SIZE, "test", Z_SHM_CREATE);
    if (ret != 0) {
        Z_RAW("Failed to initialize shared memory\n");
        return -1;
    }

    Z_RAW("Sender process initialized (PID: %d)\n", getpid());
    Z_RAW("Shared Memory ID: %d\n", shm.shm_id);

    // 发送多个数据包
    const char *test_messages[] = {
        "First message: Hello World!",
        "Second message: This is a test",
        "Third message: Testing shared memory",
        "Fourth message: With kfifo buffer",
        "Fifth message: Final test message"
    };

    for (int i = 0; i < sizeof(test_messages)/sizeof(test_messages[0]); i++) {
        struct packet_header header = {
            .magic = PACKET_MAGIC,
            .length = strlen(test_messages[i]) + 1,
            .seq = seq++
        };

        // 发送包头
        ret = z_shared_memory_send(&shm, &header, sizeof(header), 1000);
        if (ret != 0) {
            Z_RAW("Failed to send header, seq: %u\n", header.seq);
            continue;
        }

        // 发送数据
        ret = z_shared_memory_send(&shm, test_messages[i], header.length, 1000);
        if (ret != 0) {
            Z_RAW("Failed to send data, seq: %u\n", header.seq);
            continue;
        }

        Z_RAW("Sent message %u: '%s' (length: %u)\n", 
              header.seq, test_messages[i], header.length);

        // 短暂延时，方便观察
        usleep(500000);  // 500ms
    }

    // 发送结束标记（长度为0的包）
    struct packet_header end_header = {
        .magic = PACKET_MAGIC,
        .length = 0,
        .seq = seq
    };
    z_shared_memory_send(&shm, &end_header, sizeof(end_header), 1000);

    // 等待一会儿确保接收端能收到最后的消息
    sleep(5);

    // 作为创建者清理资源
    z_shared_memory_free(&shm, Z_SHM_CREATE);

    Z_RAW("Sender process completed\n");
    return 0;
} 