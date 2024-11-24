#ifndef _Z_TOOL_H_
#define _Z_TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Including standard libraries for various functionalities like networking, threading, etc.
#include <arpa/inet.h>    // Functions for IP address conversion
#include <assert.h>       // Defines the assert macro for debugging
#include <dirent.h>       // Functions for directory operations
#include <errno.h>        // Error code definitions
#include <fcntl.h>        // File control options
#include <math.h>         // Standard mathematical functions
#include <mtd/mtd-user.h> // Functions for interacting with MTD devices
#include <netdb.h>        // Network database operations
#include <netinet/in.h>   // Internet address family
#include <netinet/tcp.h>  // Definitions related to TCP protocol
#include <pthread.h>      // POSIX threads support
#include <pwd.h>          // User database operations
#include <semaphore.h>    // Semaphore operations
#include <signal.h>       // Signal processing functions
#include <stdarg.h>       // Handling of variable arguments
#include <stdbool.h>      // Boolean type definitions
#include <stddef.h>       // Common definitions, such as size_t
#include <stdint.h>       // Standard integer types
#include <stdio.h>        // Standard input/output functions
#include <stdlib.h>       // Standard library functions
#include <string.h>       // String operations
#include <sys/ioctl.h>    // Device control functions
#include <sys/ipc.h>      // Inter-process communication (IPC) mechanisms
#include <sys/msg.h>      // Message queue operations
#include <sys/prctl.h>    // Process control functions
#include <sys/socket.h>   // Socket interface
#include <sys/stat.h>     // File status queries
#include <sys/statfs.h>   // Filesystem status queries
#include <sys/time.h>     // Time operations
#include <sys/types.h>    // System data type definitions
#include <sys/utsname.h>  // Basic system information
#include <termios.h>      // Terminal I/O control functions
#include <time.h>         // Time operations
#include <unistd.h>       // System call functions

// Utility macros for stringifying and alignment operations
#define Z_TOOL_STRINGIFY(x) #x
#define Z_TOOL_TOSTRING(x) Z_TOOL_STRINGIFY(x)

// Align a number up to the specified alignment
#define Z_TOOL_ALIGN_UP(__num, __align) ((__num % __align) > 0 ? (__num + (__align - (__num % __align))) : __num)
// Align a number down to the specified alignment
#define Z_TOOL_ALIGN_DOWN(__num, __align) (__num - (__num % __align))

// Align the address or value to system's word size
#define Z_TOOL_ALIGN_SYS(a) (((a) % sizeof(long)) ? ((a) + (sizeof(long) - ((a) % sizeof(long)))) : ((a)))

// Macro to suppress unused variable warnings
#define Z_TOOL_UNUSE_SET(x) (void)(x);

// Macros for determining minimum and maximum of two values
#define Z_TOOL_MIN(a, b) ((a) > (b) ? (b) : (a))
#define Z_TOOL_MAX(a, b) ((a) > (b) ? (a) : (b))

// Macros for mathematical operations
/******************************************************************************
** ABS(x)                 Absolute value of x
** SIGN(x)                Sign of x
** CMP(x,y)               0 if x==y; 1 if x>y; -1 if x<y
******************************************************************************/
#define Z_TOOL_ABS(x) ((x) >= 0 ? (x) : (-(x)))

// Function to calculate the next power of two for a given number
static inline uint32_t z_tool_roundup_pow_of_two(uint32_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n + 1;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_TOOL_H_ */
