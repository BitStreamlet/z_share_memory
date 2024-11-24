#ifndef _Z_DEBUG_
#define _Z_DEBUG_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define E_LEVEL_COLOR_ESC_START "\033["
#define E_LEVEL_COLOR_ESC_END "\033[0m"
#define E_LEVEL_COLOR_FATAL "1;31;40m"   // Bold Red text on a black background
#define E_LEVEL_COLOR_ERROR "1;35;40m"   // Bold Magenta text on a black background
#define E_LEVEL_COLOR_WARN "1;33;40m"    // Bold Yellow text on a black background
#define E_LEVEL_COLOR_NOTICE "1;34;40m"  // Bold Blue text on a black background
#define E_LEVEL_COLOR_INFO "1;32;40m"    // Bold Green text on a black background
#define E_LEVEL_COLORRE_DEBUG "1;36;40m" // Bold Cyan text on a black background
#define E_LINE_MODE "\r\n"

enum z_level_enum {
    E_Z_DEBUG_LEVEL_PRINTF = -1,
    E_Z_DEBUG_LEVEL_RAW = 0,
    E_Z_DEBUG_LEVEL_FATAL,
    E_Z_DEBUG_LEVEL_ERROR,
    E_Z_DEBUG_LEVEL_WARN,
    E_Z_DEBUG_LEVEL_NOTICE,
    E_Z_DEBUG_LEVEL_INFO,
    E_Z_DEBUG_LEVEL_DEBUG,
    E_Z_DEBUG_LEVEL_MAX
};
#define _Z_DEBUG(_level, format, args...)                                                                                                                                        \
    while (1) {                                                                                                                                                                  \
        switch (_level) {                                                                                                                                                        \
            case E_Z_DEBUG_LEVEL_FATAL:                                                                                                                                          \
                printf("[fatal ][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLOR_FATAL format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args);   \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_ERROR:                                                                                                                                          \
                printf("[error ][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLOR_ERROR format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args);   \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_WARN:                                                                                                                                           \
                printf("[wart  ][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLOR_WARN format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args);    \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_NOTICE:                                                                                                                                         \
                printf("[notice][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLOR_NOTICE format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args);  \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_INFO:                                                                                                                                           \
                printf("[info  ][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLOR_INFO format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args);    \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_DEBUG:                                                                                                                                          \
                printf("[debug ][%s:%d, %s] " E_LEVEL_COLOR_ESC_START E_LEVEL_COLORRE_DEBUG format E_LEVEL_COLOR_ESC_END E_LINE_MODE, __FILE__, __LINE__, __FUNCTION__, ##args); \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            case E_Z_DEBUG_LEVEL_RAW:                                                                                                                                            \
            case E_Z_DEBUG_LEVEL_PRINTF:                                                                                                                                         \
                printf(format, ##args);                                                                                                                                          \
                fflush(stdout);                                                                                                                                                  \
                break;                                                                                                                                                           \
            default:                                                                                                                                                             \
                break;                                                                                                                                                           \
        }                                                                                                                                                                        \
        break;                                                                                                                                                                   \
    }

#define Z_DEBUG(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_DEBUG, format, ##args)
#define Z_ERROR(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_ERROR, format, ##args)
#define Z_WARN(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_WARN, format, ##args)
#define Z_NOTICE(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_NOTICE, format, ##args)
#define Z_FATAL(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_FATAL, format, ##args)
#define Z_INFO(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_INFO, format, ##args)
#define Z_RAW(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_RAW, format, ##args)
#define Z_PRINTF(format, args...) _Z_DEBUG(E_Z_DEBUG_LEVEL_PRINTF, format, ##args)
#define Z_DEBUG_ENTER() _Z_DEBUG(E_Z_DEBUG_LEVEL_DEBUG, "enter");
#define Z_DEBUG_EXIT(_ret) _Z_DEBUG(E_Z_DEBUG_LEVEL_DEBUG, "exit :%d", _ret);

#define Z_BASE_ASSERT(condition) \
    int *p = NULL;               \
    while (1) {                  \
        *p = 0x5a5a5a5a;         \
        assert(0);               \
    }

//_ASSERT(0):就会终止
#define Z_ASSERT(condition)                                          \
    do {                                                             \
        if (!(condition)) {                                          \
            Z_FATAL("ASSERT: Condition '%s' is true !", #condition); \
            Z_BASE_ASSERT(condition);                                \
        }                                                            \
    } while (0)

#define Z_WARN_ON(condition)                                        \
    do {                                                            \
        if (condition) {                                            \
            Z_WARN("WARNING: Condition '%s' is true!", #condition); \
        }                                                           \
    } while (0)

#define Z_WARN_ON_ONCE(condition)                                   \
    do {                                                            \
        static int __warned = 0;                                    \
        if (!__warned && (condition)) {                             \
            Z_WARN("WARNING: Condition '%s' is true!", #condition); \
            __warned = 1;                                           \
        }                                                           \
    } while (0)

#define Z_BUG()             \
    do {                    \
        z("BUG: failure!"); \
        z("BUG!");          \
    } while (0)

#define Z_BUG_ON(condition)                                 \
    do {                                                    \
        if ((condition)) {                                  \
            z("BUG: Condition '%s' is true !", #condition); \
            Z_BASE_ASSERT(condition);                       \
        }                                                   \
    } while (0)

// 测试断言宏
#define Z_ASSERT_TEST(condition, message)      \
    if (!(condition)) {                        \
        Z_ERROR("Test failed: %s", (message)); \
    } else {                                   \
        Z_ERROR("Test passed: %s", (message)); \
    }

#define Z_DEBUG_THREAD_SET_TRACE(func)                   \
    do {                                                 \
        char tmp[16];                                    \
        snprintf(tmp, sizeof(tmp), "%p", func);          \
        prctl(PR_SET_NAME, (unsigned long)tmp, 0, 0, 0); \
    } while (0);
#define Z_DEBUG_THREAD_CALL_INFO()
#define Z_DEBUG_THREAD_CALL_INFO_FREE()

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Z_CONFIG_H_ */
