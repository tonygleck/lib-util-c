// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "lib-util-c/app_logging.h"

static log_category g_logging_category = log_all;

#if defined(__GNUC__)
__attribute__ ((format (printf, 6, 7)))
#endif
static void default_log_function(log_category category, const char* file, const char* func, int line, unsigned int options, const char* format, ...)
{
    if (g_logging_category >= category)
    {
        time_t t;
        va_list args;
        va_start(args, format);

        t = time(NULL);
        if (category == log_error)
        {
            (void)printf("Error: Time:%.24s File:%s Func:%s Line:%d ", ctime(&t), file, func, line);
        }

        (void)vprintf(format, args);
        va_end(args);

        if (options & LOG_LINE)
        {
            (void)printf("\r\n");
        }
    }
}

static APP_LOG_FN g_log_function = default_log_function;

void log_set_level(log_category category)
{
    g_logging_category = category;
}

log_category log_get_level(void)
{
    return g_logging_category;
}

APP_LOG_FN log_get_log_function(void)
{
    return g_log_function;
}

void log_set_log_function(APP_LOG_FN log_fn)
{
    g_log_function = log_fn;
}