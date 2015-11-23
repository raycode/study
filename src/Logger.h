#pragma once



class Logger
{
public:
	enum LogLevel
	{
		TRACE = 1,
		WARN,
		ERR,
		FATAL,
		ALWAYS
	};

	static void Log(int nLevel, const WCHAR* szDesc, ...);
	static void LogTrace( const WCHAR* szDesc, ...);
};

#ifdef _DEBUG
#define LOG_TRACE(...)		Logger::LogTrace(__VA_ARGS__)
#else 
#define LOG_TRACE(...)		__noop
#endif 

extern int s_nLogLevel;