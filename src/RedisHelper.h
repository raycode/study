#pragma once

struct redisContext;

/*
	점수 더하기 
	zadd leaderboard_d score user_nick

	해당 유저 점수 가져오기 
	zscore leaderboard_d user_nick

	1위 부터 100위 까지 목록 
	zrevrange leaderboard_d 0 99 withscores

	리더보드에 갯수 가져오기 
	zcard leaderboard_d 

	유저가 몇위인지 가져오기 
	zrevrank leaderboard_d user_nick

	내점수 순위... 
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