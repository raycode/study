// RedisCommander.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "NxExceptionDump.h"
#include "NxIniHelper.h"
#include "NxFileTrace.h"
#include "NxStringUtil.h"
#include "Logger.h"
#include "hiredis.h"


extern NxFileTrace			g_MFileLog;
NxIniHelper					g_IniHelper;

redisContext*	g_pRedisContext = 0;

using namespace std;

bool RedisConnect(std::string& szIP, int nPort, std::string& auth);
bool RedisCommand(const char* szCmd);

int _tmain(int argc, _TCHAR* argv[])
{
	locale::global(locale("kor"));
	NxExceptionDump::Enable();
	g_MFileLog.SetLogPath(L"Log",L"Log",L"");


	Logger::Log(Logger::ALWAYS, L"RedisCommander Start.");

	// 현재 실행 모듈을 알아낸다.
	CHAR szCurrentFilePath[MAX_PATH]={0,};
	::GetModuleFileNameA(NULL, szCurrentFilePath, MAX_PATH);

	CHAR drive[_MAX_DRIVE]={0,};
	CHAR dir[_MAX_DIR]={0,};
	CHAR fname[_MAX_FNAME]={0,};
	CHAR ext[_MAX_EXT]={0,};

	_splitpath_s( szCurrentFilePath, drive, dir, fname, ext ); 

	// 확장자를 제거한 모듈 경로를 준비해둔다.
	std::string szIniFileName;
	std::string szCurDir;
	szIniFileName = drive;
	szIniFileName += dir;
	szCurDir = szIniFileName;
	szIniFileName += "config.ini";
	g_IniHelper.SetFile( szIniFileName.c_str());


	Logger::Log(Logger::ALWAYS, L"Load %s", NxStringUtil::ToWideString(szIniFileName).c_str() ) ;

	std::string szRedisServerIP = g_IniHelper.Read("Settings", "RedisServerIP", "127.0.0.1");
	int nRedisServerPort	= g_IniHelper.Read("Settings", "RedisServerPort", 6379);
	std::string szRedisServerPasswd = g_IniHelper.Read("Settings", "RedisServerPasswd", "rollsam");
	std::string szCommand = g_IniHelper.Read("Settings", "Command", "PING");

	Logger::Log(Logger::ALWAYS, L"Target Redis Server (%s:%d)", NxStringUtil::ToWideString(szRedisServerIP).c_str(), nRedisServerPort) ;
	bool bResult = RedisConnect(szRedisServerIP, nRedisServerPort, szRedisServerPasswd);
	Logger::Log(Logger::ALWAYS, L"Connect Redis Server %s (%s:%d)", bResult?L"Success":L"Failed", NxStringUtil::ToWideString(szRedisServerIP).c_str(), nRedisServerPort) ;
	
	if ( bResult )
	{
		std::vector<std::string> tokens;
		NxStringUtil::TokenizeA(szCommand, tokens, ";");

		Logger::Log(Logger::ALWAYS, L"Command Count : %d",  tokens.size() );

		for ( size_t i = 0 ; i < tokens.size() ; i++ )
		{
			RedisCommand(tokens[i].c_str());
		}
	}
	

	return 0;
}


bool RedisConnect(std::string& szIP, int nPort, std::string& auth)
{
	struct timeval timeout = { 5, 0 }; 

	
	g_pRedisContext = redisConnectWithTimeout((char*)szIP.c_str(), nPort, timeout);
	if ( g_pRedisContext && g_pRedisContext->err) 
	{
		g_pRedisContext->errstr;
		g_pRedisContext->err;
		
		Logger::Log(Logger::FATAL, L"Connect failed. [%s]", NxStringUtil::ToWideString(g_pRedisContext->errstr).c_str());
		return false;
	}

	if ( auth.empty() == false )
	{
		redisReply* reply;

		std::string szCmd = NxStringUtil::FormatA("AUTH %s", auth.c_str());
		reply = (redisReply*)redisCommand(g_pRedisContext, szCmd.c_str());

		if ( reply == NULL )
		{
			return false;
		}

		printf("PING: %s\n", reply->str);
		
		freeReplyObject(reply);
	}

	

	return true;


}

bool RedisCommand(const char* szCmd)
{
	Logger::Log( Logger::TRACE, L"Redis Cmd = %s", NxStringUtil::ToWideString(szCmd).c_str());

	redisReply* reply;

	reply = (redisReply*)redisCommand(g_pRedisContext, szCmd);

	if ( reply == NULL )
	{
		Logger::Log( Logger::ERR, L"redisCommand reply is null");
		return false;
	}

	if ( reply->str )
	{
		Logger::Log( Logger::TRACE, L"Redis reply = %s", NxStringUtil::ToWideString(reply->str).c_str());

	}

	freeReplyObject(reply);
	
	return true;
}