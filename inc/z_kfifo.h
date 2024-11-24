#ifndef _Z_KFIFO_H_
#define _Z_KFIFO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Structure defining a kernel FIFO (First In, First Out) queue */
struct z_kfifo_struct {
    uint32_t buffer_offset;  // 相对于共享内存起始位置的偏移量
    uint32_t size;     /* Total size of the allocated buffer */
    uint32_t in;       /* Offset where data is added (in % size) */
    uint32_t out;      /* Offset where data is extracted (out % size) */
};

/* Initialize the FIFO with buffer offset and size */
void z_kfifo_init(struct z_kfifo_struct *p_fifo, uint32_t buffer_offset, uint32_t size);

/* Add data from a source buffer to the FIFO */
uint32_t z_kfifo_in(struct z_kfifo_struct *p_fifo, const void *p_from, uint32_t len, void *base_addr);

/* Extract data from the FIFO into a destination buffer */
uint32_t z_kfifo_out(struct z_kfifo_struct *p_fifo, void *p_to, uint32_t len, void *base_addr);

/* Calculate available space for new data in the FIFO */
uint32_t z_kfifo_space(struct z_kfifo_struct *p_fifo);

/* Determine the length of the data currently stored in the FIFO */
uint32_t z_kfifo_data_len(struct z_kfifo_struct *p_fifo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_KFIFO_H_ */
