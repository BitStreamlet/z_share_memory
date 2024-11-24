#include "z_tool.h"
#include "z_debug.h"
#include "z_kfifo.h"

// Initializes the kfifo structure with a buffer offset and its size.
void z_kfifo_init(struct z_kfifo_struct *p_fifo, uint32_t buffer_offset, uint32_t size) {
    if (!p_fifo) return;
    p_fifo->buffer_offset = buffer_offset;
    p_fifo->size = size;
    p_fifo->in = p_fifo->out = 0;
}

// Writes data into the kfifo buffer.
uint32_t z_kfifo_in(struct z_kfifo_struct *p_fifo, const void *p_from, uint32_t len, void *base_addr) {
    if (!p_fifo || !p_from || !base_addr) return 0;
    uint32_t l, off;
    uint8_t *buffer = (uint8_t *)base_addr + p_fifo->buffer_offset;

    // Limit the length to the available space in the buffer.
    len = Z_TOOL_MIN(p_fifo->size - (p_fifo->in - p_fifo->out), len);
    off = p_fifo->in & (p_fifo->size - 1); // Calculate offset for circular buffer.

    // Determine how much data can be written in the first segment.
    l = Z_TOOL_MIN(len, p_fifo->size - off);
    memcpy(buffer + off, p_from, l); // Write to the buffer.

    // Write remaining data if the buffer wraps around.
    memcpy(buffer, (char *)p_from + l, len - l);
    p_fifo->in += len; // Update the write pointer.
    return len;        // Return the number of bytes written.
}

// Reads data from the kfifo buffer.
uint32_t z_kfifo_out(struct z_kfifo_struct *p_fifo, void *p_to, uint32_t len, void *base_addr) {
    if (!p_fifo || !p_to || !base_addr) return 0;
    uint32_t off, l;
    uint8_t *buffer = (uint8_t *)base_addr + p_fifo->buffer_offset;

    // Limit the length to the available data in the buffer.
    len = Z_TOOL_MIN(p_fifo->in - p_fifo->out, len);
    off = p_fifo->out & (p_fifo->size - 1); // Calculate offset for circular buffer.

    // Determine how much data can be read in the first segment.
    l = Z_TOOL_MIN(len, p_fifo->size - off);
    memcpy(p_to, buffer + off, l); // Read from the buffer.

    // Read remaining data if the buffer wraps around.
    memcpy((char *)p_to + l, buffer, len - l);
    p_fifo->out += len; // Update the read pointer.
    return len;         // Return the number of bytes read.
}

// Returns the amount of free space in the kfifo.
uint32_t z_kfifo_space(struct z_kfifo_struct *p_fifo) {
    if (!p_fifo) return 0;
    return p_fifo->size - (p_fifo->in - p_fifo->out);
}

// Returns the amount of data currently stored in the kfifo.
uint32_t z_kfifo_data_len(struct z_kfifo_struct *p_fifo) {
    if (!p_fifo) return 0;
    return p_fifo->in - p_fifo->out;
}
