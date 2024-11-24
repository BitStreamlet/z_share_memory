#include "z_tool.h"

// Structure to configure the print table options
struct z_table_print_struct {
    int width;                              // Width of the table, influencing the line length
    char buffer[256];                       // Buffer to hold printed line content temporarily
    void (*print)(const char *format, ...); // Function pointer for custom print functionality
};

// Default print function declaration
extern void __z_table_print_default(const char *format, ...);

// Global instance with default settings for table printing
struct z_table_print_struct gs_table_print = {80, {0}, __z_table_print_default};

// Default implementation of the print function
void __z_table_print_default(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

// Function to print a border line using the specified printer settings
static void __z_table_print_border(struct z_table_print_struct *printer) {
    if (printer->print != NULL) {                     // Check if a print function is set
        memset(printer->buffer, '-', printer->width); // Fill buffer with dashes
        printer->buffer[printer->width] = '\0';       // Null-terminate the string
        printer->print("%s\r\n", printer->buffer);    // Printer the border line
    }
    return;
}

// Function to print a title within borders using the specified printer settings
static void __z_table_print_title(struct z_table_print_struct *printer, const char *title) {
    if (printer->print != NULL) {                                                             // Check if a print function is set
        __z_table_print_border(printer);                                                      // Print top border
        snprintf(printer->buffer, printer->width - 4, "| %-*s |", printer->width - 4, title); // Format title row
        printer->print("%s\r\n", printer->buffer);                                            // Print the title row
        __z_table_print_border(printer);                                                      // Print bottom border
    }
    return;
}

// Public function to print a table border
void z_table_print_border(void) {
    __z_table_print_border(&gs_table_print);
    return;
}

// Public function to print a table title
void z_table_print_title(const char *title) {
    __z_table_print_title(&gs_table_print, title);
    return;
}

// Public function to print a formatted row within the table
void z_table_print_row(const char *format, ...) {
    struct z_table_print_struct *printer = &gs_table_print;
    if (printer->print != NULL) { // Check if a print function is set
        va_list args;
        va_start(args, format);
        vsnprintf(printer->buffer, sizeof(printer->buffer), format, args); // Format row content
        va_end(args);
        printer->print("%s", printer->buffer); // Print the row content
    }
    return;
}

// Public function to set the table's width, ensuring it does not exceed buffer size
void z_table_print_set_width(int width) {
    gs_table_print.width = width;

    // Ensure the width does not exceed the buffer's capacity
    if (width >= sizeof(gs_table_print.buffer)) {
        gs_table_print.width = sizeof(gs_table_print.buffer) - 1;
    }
    return;
}

// Public function to set a custom print function
void z_table_print_set_func(void (*print)(const char *format, ...)) {
    if (print) { // Verify the function pointer is valid
        gs_table_print.print = print;
    }
    return;
}
