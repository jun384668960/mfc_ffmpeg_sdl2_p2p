#pragma once
#include <queue>
using namespace std;

#define   LOG_EMERG   0
#define   LOG_ERR     1
#define   LOG_WARN    2
#define   LOG_INFO    3
#define   LOG_DEBUG   4
#define   LOG_TRACE   5

#define LOGT_print(t, ...)	Log::getInstance()->PushLog(LOG_TRACE, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGD_print(t, ...) 	Log::getInstance()->PushLog(LOG_DEBUG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGI_print(t, ...) 	Log::getInstance()->PushLog(LOG_INFO,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGW_print(t, ...)	Log::getInstance()->PushLog(LOG_WARN,  "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGE_print(t, ...) 	Log::getInstance()->PushLog(LOG_ERR,   "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOGM_print(t, ...) 	Log::getInstance()->PushLog(LOG_EMERG, "[%s][%d]"t"", __FUNCTION__, __LINE__, ##__VA_ARGS__)

class Log
{
public:
	Log(int max = 1024);
	virtual ~Log();

	bool PushLog(int level, char* fmt, ...);
	bool PopLog(string& logLine);
	bool SetLevle(int level);

	static Log* getInstance();
	static Log* m_Instance;
	static int  m_Level;
protected:
	HANDLE				m_Mutex;
	queue<string>		m_Logs;
	int					m_MaxSize;
};

