#include "stdafx.h"
#include "Log.h"

Log* Log::m_Instance = NULL;
static char gLogBuffer[2048] = { 0 };
int Log::m_Level = LOG_DEBUG;

Log::Log(int max)
{
	m_MaxSize = max;
	m_Mutex = CreateMutex(NULL, FALSE, "Log");
}

Log::~Log()
{
	while (m_Logs.size() > 0)
	{
		m_Logs.pop();
	}
	m_Instance = NULL;
}

bool Log::PushLog(int level, char* fmt, ...)
{
	if (fmt == NULL || m_Level < level) return false;
	if (m_Logs.size() > m_MaxSize)  return false;

	SYSTEMTIME st;
	GetLocalTime(&st);
	char time[64] = {0};
	sprintf(time, "%04d-%02d-%02d %02d:%02d:%02d.%03d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	char line[1024] = { 0 }; //限制t不能太大
	_snprintf(line,1023, "%s %s %s\n", time, level == LOG_TRACE ? "TRACE" : (level == LOG_DEBUG ? "DEBUG" : (level == LOG_INFO ? "!INFO" : (level == LOG_WARN ? "!WARN" : (level == LOG_ERR ? "ERROR" : "EMERG")))), fmt);
	
	va_list params;
	va_start(params, fmt);
	vsnprintf(gLogBuffer, 2047, line, params);
	va_end(params);

	string logLine = gLogBuffer;
	WaitForSingleObject(m_Mutex, INFINITE);
	m_Logs.push(logLine);
	ReleaseMutex(m_Mutex);
	printf("%s\n", logLine.c_str());

	return true;
}
bool Log::PopLog(string& logLine)
{
	if (m_Logs.size() > 0)
	{
		WaitForSingleObject(m_Mutex, INFINITE);
		logLine = m_Logs.front();
		m_Logs.pop();
		ReleaseMutex(m_Mutex);
		return true;
	} 
	else
	{
		return false;
	}
}

bool Log::SetLevle(int level)
{
	if (level > LOG_TRACE || level < LOG_EMERG)
		return false;

	m_Level = level;
	return true;
}

Log* Log::getInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new Log();
	}
	return m_Instance;
}

