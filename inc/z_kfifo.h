#ifndef _Z_KFIFO_H_
#define _Z_KFIFO_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Structure defining a kernel FIFO (First In, First Out) queue */
struct z_kfifo_struct {
    uint8_t *p_buffer; /* Pointer to the buffer holding the data */
    uint32_t size;     /* Total size of the allocated buffer */
    uint32_t in;       /* Offset where data is added (in % size) */
    uint32_t out;      /* Offset where data is extracted (out % size) */
};

/* Initialize the FIFO with an existing buffer and its size */
void z_kfifo_init(struct z_kfifo_struct *p_fifo, void *p_buffer, uint32_t size);

/* Allocate a buffer for the FIFO dynamically */
int z_kfifo_malloc(struct z_kfifo_struct *p_fifo, uint32_t size);

/* Free the dynamically allocated buffer in the FIFO */
void z_kfifo_free(struct z_kfifo_struct *p_fifo);

/* Add data from a source buffer to the FIFO */
uint32_t z_kfifo_in(struct z_kfifo_struct *p_fifo, const void *p_from, uint32_t len);
uint32_t z_kfifo_shm_in(struct z_kfifo_struct *p_fifo, const void *p_from, uint32_t len, void *p_buf_addr);

/* Extract data from the FIFO into a destination buffer */
uint32_t z_kfifo_out(struct z_kfifo_struct *p_fifo, void *p_to, uint32_t len);
uint32_t z_kfifo_shm_out(struct z_kfifo_struct *p_fifo, void *p_to, uint32_t len, void *p_buf_addr);

/* Calculate available space for new data in the FIFO */
uint32_t z_kfifo_space(struct z_kfifo_struct *p_fifo);

/* Determine the length of the data currently stored in the FIFO */
uint32_t z_kfifo_data_len(struct z_kfifo_struct *p_fifo);

/* Check and potentially extract data without removing it from the FIFO */
uint32_t z_kfifo_out_check(struct z_kfifo_struct *p_fifo, void *p_to, uint32_t len);

/* Test functionality of the FIFO; could be used for diagnostics or unit testing */
uint32_t z_kfifo_test(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_KFIFO_H_ */