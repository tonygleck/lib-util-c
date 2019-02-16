// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef APP_LOGGING_H
#define APP_LOGGING_H

#ifdef __cplusplus
extern "C" {
#else

#endif

typedef enum log_category_tag
{
    log_error,
    log_trace,
    log_warning,
    log_info,
    log_debug,
    log_all,
} log_category;

#define LOG_LINE    0x01

#if defined _MSC_VER
#define FUNC_NAME       __FUNCDNAME__
#define VARIABLE_ARG    __VA_ARGS__
#else
#define FUNC_NAME       __func__
#endif

typedef void(*APP_LOG_FN)(log_category category, const char* file, const char* func, int line, unsigned int options, const char* format, ...);

#define LOG(log_category, log_option, format, ...) \
{ \
    if (0) { (void)printf(format, ##__VA_ARGS__); } \
    { \
        APP_LOG_FN lg_fn = log_get_log_function();    \
        if (lg_fn != NULL) lg_fn(log_category, __FILE__, FUNC_NAME, __LINE__, log_option, format, ##__VA_ARGS__); \
    } \
}

#define log_error(FORMAT, ...) do { LOG(log_error, LOG_LINE, FORMAT, ##__VA_ARGS__); }while((void)0, 0)
#define log_warning(FORMAT, ...)  do { LOG(log_warning, LOG_LINE, FORMAT, ##__VA_ARGS__); }while((void)0, 0)
#define log_info(FORMAT, ...)  do { LOG(log_info, LOG_LINE, FORMAT, ##__VA_ARGS__); }while((void)0, 0)
#define log_debug(FORMAT, ...)  do { LOG(log_debug, LOG_LINE, FORMAT, ##__VA_ARGS__); }while((void)0, 0)
#define log_trace(FORMAT, ...)  do { LOG(log_trace, 0x00, FORMAT, ##__VA_ARGS__); }while((void)0, 0)

extern void log_set_level(log_category category);
extern log_category log_get_level(void);
extern APP_LOG_FN log_get_log_function(void);
extern void log_set_log_function(APP_LOG_FN log_fn);

#ifdef __cplusplus
}
#endif

#endif // APP_LOGGING_H
