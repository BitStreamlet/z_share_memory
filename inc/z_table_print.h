#ifndef _Z_TABLE_PRINT_H_
#define _Z_TABLE_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Function declarations for table printing utilities */

/* Prints the border for a table */
void z_table_print_border(void);

/*
 * Prints a title within the table.
 * @param title: A string representing the title to be printed.
 */
void z_table_print_title(const char *title);

/*
 * Prints a formatted row in the table.
 * @param format: A string specifying the format.
 * Other parameters are handled as per the format specifications.
 */
void z_table_print_row(const char *format, ...);

/*
 * Sets the width of the table for consistent formatting.
 * @param width: An integer representing the desired table width.
 */
void z_table_print_set_width(int width);

/*
 * Defines a custom print function to be used.
 * @param print: A pointer to a function that matches the specified signature.
 */
void z_table_print_set_func(void (*print)(const char *format, ...));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_TABLE_PRINT_H_ */
