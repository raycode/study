#include "stdafx.h"
#include "RedisHelper.h"
#include "hiredis.h"
#include "NxStringUtil.h"
#include "Logger.h"
#include "NxFileTrace.h"


RedisHelper::RedisHelper()
{
	m_bIsConnected = false;
	m_pRedisContext = 0;
}

RedisHelper::~RedisHelper()
{

}

bool RedisHelper::ConnectTo(const char* szIP, int nPort, int nTimeoutSec, const std::string& szPassword)
{
	if ( m_bIsConnected )
	{
		//NX_CRASH();
		return false;
	}

	m_szIP = szIP;
	m_nPort = nPort;
	

	struct timeval timeout = { nTimeoutSec, 0 }; 
	m_pRedisContext = redisConnectWithTimeout((char*)szIP, nPort, timeout);
	if ( m_pRedisContext->err) 
	{
		m_szLastErrorString = m_pRedisContext->errstr;
		m_nLastErrorCode = m_pRedisContext->err;
		return false;
	}

	if ( szPassword.empty() == false )
	{
		redisReply* reply;

		std::string szCmd = NxStringUtil::FormatA("AUTH %s", szPassword.c_str());
		reply = (redisReply*)redisCommand(m_pRedisContext, szCmd.c_str());
		
		if ( reply == NULL )
			return false;

		printf("PING: %s\n", reply->str);
		m_szLastReturn = reply->str;
		freeReplyObject(reply);
	}


	m_bIsConnected = true;
	return true;

}

bool RedisHelper::Ping()
{
	if ( !m_bIsConnected )
		return false;

	redisReply* reply;

	reply = (redisReply*)redisCommand(m_pRedisContext,"PING");
	if ( reply == NULL )
	{
		m_bIsConnected = false;
		return false;
	}

	printf("PING: %s\n", reply->str);
	m_szLastReturn = reply->str;
	freeReplyObject(reply);

	return true;
}

bool RedisHelper::String_Command(const char* szCommand, OUT std::string& result)
{
	if ( !m_bIsConnected )
		return false;

	
	Logger::Log( Logger::TRACE, L"RedisCmd ID = %s", NxStringUtil::ToWideString(szCommand).c_str());

	redisReply* reply;

	reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);

	if ( reply == NULL )
	{
		m_bIsConnected = false;
		return false;
	}

	if ( reply->str )
	{
		m_szLastReturn = reply->str;
		result = m_szLastReturn;
	}
	
	freeReplyObject(reply);

	return true;
}

bool RedisHelper::Integer_Command(const char* szCommand, OUT __int64& result)
{
	if ( !m_bIsConnected )
		return false;

	Logger::Log( Logger::TRACE, L"RedisCmd ID = %s", NxStringUtil::ToWideString(szCommand).c_str());

	redisReply* reply;

	reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);
	if ( reply == NULL )
	{
		reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);
		reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);
		reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);
		if ( reply == NULL )
		{
			m_bIsConnected = false;
			return false;
		}

	}
	if ( reply->type == REDIS_REPLY_INTEGER )
	{
		result = reply->integer;
	}

	freeReplyObject(reply);

	return true;
}

bool RedisHelper::Array_Command(const char* szCommand, OUT std::vector<std::string>& result)
{
	if ( !m_bIsConnected )
		return false;

	Logger::Log( Logger::TRACE, L"RedisCmd ID = %s", NxStringUtil::ToWideString(szCommand).c_str());

	redisReply* reply;

	reply = (redisReply*)redisCommand(m_pRedisContext, szCommand);
	
	if ( reply == NULL )
	{
		m_bIsConnected = false;
		return false;
	}

	if ( reply->element > 0 ) 
	{
		for ( size_t i = 0 ; i < reply->elements ; i++ )
		{
			redisReply* element = reply->element[i];

			if ( element && element->str )
			{
				result.push_back(element->str);
			}
			else
			{
				result.push_back("__NULL__");
			}

		}
	}
	

	freeReplyObject(reply);

	return true;
}