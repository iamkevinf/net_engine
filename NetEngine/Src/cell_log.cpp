#include "cell_log.h"

#include <cstdarg>
#include <string>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#else
#include <iostream>
#endif

namespace knet
{
	namespace LogPrefix
	{
		const std::string Trace = "Trace: ";
		const std::string Debug = "DEBUG: ";
		const std::string Info = "INFO : ";
		const std::string Warn = "WARN : ";
		const std::string Error = "ERROR: ";
		const std::string Fatal = "FATAL: ";

		const std::string & GetLogPrefix(int lvl)
		{
			if (lvl >= LOG_LVL_FATAL) return Fatal;
			if (lvl >= LOG_LVL_ERROR) return Error;
			if (lvl >= LOG_LVL_WARN) return Warn;
			if (lvl >= LOG_LVL_INFO) return Info;
			if (lvl >= LOG_LVL_DEBUG) return Debug;
			return Trace;
		}
	}

	bool StringFromVAList(std::string & out, const char * format, va_list va)
	{
		int len = 128;
		int ret = -1;

		while (ret < 0)
		{
			len <<= 1;

			out.resize(len);
			ret = vsnprintf(&out[0], len, format, va);

			//mac上，如果长度不够长，会返回需要的长度
			if (ret > len)
			{
				len = (ret >> 1) + 1;
				ret = -1;
			}

			assert(len < 65535 && "stringFromVAList");
		}

		out.erase(ret);
		return true;
	}

	void cell_log(int lvl, const char* format, ...)
	{
		std::string msg;

		va_list va;
		va_start(va, format);

		StringFromVAList(msg, format, va);

		va_end(va);

		const std::string & prefix = LogPrefix::GetLogPrefix(lvl);
		msg.insert(0, prefix);

#ifdef _WIN32
		OutputDebugString((msg + "\r").c_str());
#else
		std::cout << msg << std::endl;
#endif
		std::cout << msg << std::endl;

		if (lvl >= LOG_LVL_FATAL)
		{
			abort();
		}
	}

}; // end of namespace knet
