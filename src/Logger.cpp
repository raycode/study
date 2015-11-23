#include "stdafx.h"
#include "Logger.h"
#include "NxFileTrace.h"

#ifdef _DEBUG
int s_nLogLevel = Logger::TRACE;
#else
int s_nLogLevel = Logger::ERR;
#endif 


NxFileTrace			g_MFileLog;

void __Log(int nLevel, const WCHAR* szDesc, ...)
{
	const int __BufferSize = 8192;
	WCHAR strBuffer[__BufferSize] = {0,};

	va_list args;
	va_start(args, szDesc);
	StringCchVPrintfW( strBuffer, __BufferSize, szDesc, args );
	strBuffer[__BufferSize-1] = L'\0';
	va_end(args);

	std::wstring szLog;
	szLog = strBuffer;

	int __color[]=
	{
		7,
		7,		/*TRACE*/
		14,		/*WARN*/
		236,	/*ERR*/
		207,	/*FATAL*/
		15		/*ALWAYS */
	};

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	//NxGuard<NxSyncObject> guard(g_MFileLogLock);
	if ( nLevel < _countof(__color) )
	{
		SetConsoleTextAttribute(hConsole, __color[nLevel]);
	}

	wcout <<  szLog << endl;

	SetConsoleTextAttribute(hConsole, __color[0]);


	g_MFileLog.TraceToFile(szLog.c_str());
}


void Logger::Log(int nLevel, const WCHAR* szDesc, ...)
{
	if ( nLevel < s_nLogLevel)
		return;

	const int __BufferSize = 8192;
	WCHAR strBuffer[__BufferSize] = {0,};

	va_list args;
	va_start(args, szDesc);
	StringCchVPrintfW( strBuffer, __BufferSize, szDesc, args );
	strBuffer[__BufferSize-1] = L'\0';
	va_end(args);

	std::wstring szLog;
	szLog = strBuffer;

	__Log(nLevel,L"%s", szLog.c_str());

}

void Logger::LogTrace( const WCHAR* szDesc, ...)
{
	if ( Logger::TRACE < s_nLogLevel)
		return;

	const int __BufferSize = 8192;
	WCHAR strBuffer[__BufferSize] = {0,};

	va_list args;
	va_start(args, szDesc);
	StringCchVPrintfW( strBuffer, __BufferSize, szDesc, args );
	strBuffer[__BufferSize-1] = L'\0';
	va_end(args);

	std::wstring szLog;
	szLog = strBuffer;

	__Log(Logger::TRACE,L"%s", szLog.c_str());
}