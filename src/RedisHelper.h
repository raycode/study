#pragma once

struct redisContext;

/*
	���� ���ϱ� 
	zadd leaderboard_d score user_nick

	�ش� ���� ���� �������� 
	zscore leaderboard_d user_nick

	1�� ���� 100�� ���� ��� 
	zrevrange leaderboard_d 0 99 withscores

	�������忡 ���� �������� 
	zcard leaderboard_d 

	������ �������� �������� 
	zrevrank leaderboard_d user_nick

	������ ����... 
	ZCOUNT key score+1 +inf 
*/

class RedisHelper
{
public:
	RedisHelper();
	virtual ~RedisHelper();

	bool ConnectTo(const char* szIP, int nPort, int nTimeoutSec, const std::string& szPassword);

	bool Ping();

	bool Integer_Command(const char* szCommand, OUT __int64& result);
	bool String_Command(const char* szCommand, OUT std::string& result);
	bool Array_Command(const char* szCommand, OUT std::vector<std::string>& result);
	bool IsConnected() { return m_bIsConnected; }
private:

	bool			m_bIsConnected;
	redisContext*	m_pRedisContext;
	std::string		m_szIP;
	USHORT			m_nPort;

	int				m_nLastErrorCode;
	std::string		m_szLastErrorString;

	std::string		m_szLastReturn;

};