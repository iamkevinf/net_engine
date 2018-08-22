#ifndef __CELL_LOG_H__
#define __CELL_LOG_H__

#define LOG_LVL_TRACE 1
#define LOG_LVL_DEBUG 10
#define LOG_LVL_INFO  20
#define LOG_LVL_WARN  30
#define LOG_LVL_ERROR 40
#define LOG_LVL_FATAL 50

#ifndef LOG_LVL
#   ifndef _DEBUG
#       define LOG_LVL LOG_LVL_WARN
#   else
#       define LOG_LVL LOG_LVL_TRACE
#   endif
#endif

namespace knet
{
	void cell_log(int lvl, const char* format, ...);
}; // end of namespace knet

#if LOG_LVL_TRACE >= LOG_LVL
#   define LOG_TRACE(...) knet::cell_log(LOG_LVL_TRACE, __VA_ARGS__)
#else
#   define LOG_TRACE(...) void
#endif

#if LOG_LVL_DEBUG >= LOG_LVL
#   define LOG_DEBUG(...) knet::cell_log(LOG_LVL_DEBUG, __VA_ARGS__)
#else
#   define LOG_DEBUG(...) void
#endif

#if LOG_LVL_INFO >= LOG_LVL
#   define LOG_INFO(...) knet::cell_log(LOG_LVL_INFO, __VA_ARGS__)
#else
#   define LOG_INFO(...) void
#endif

#if LOG_LVL_WARN >= LOG_LVL
#   define LOG_WARN(...) knet::cell_log(LOG_LVL_WARN, __VA_ARGS__)
#else
#   define LOG_WARN(...) void
#endif

#if LOG_LVL_ERROR >= LOG_LVL
#   define LOG_ERROR(...) knet::cell_log(LOG_LVL_ERROR, __VA_ARGS__)
#else
#   define LOG_ERROR(...) void
#endif

#define LOG_FATAL(...) knet::cell_log(LOG_LVL_FATAL, __VA_ARGS__)

#define STACK_TRACE LOG_TRACE("%s() LINE:%d FILE:%s ", __FUNCTION__, __LINE__, __FILE__)

#endif // __CELL_LOG_H__