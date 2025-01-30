#pragma once

enum class WARNING_SEVERITY
{
	LOW,
	MEDIUM,
	HIGH,
	FATAL,
};

class Logger
{
private:
	Logger() {};
public:
	static void CreateLogger();
	static void DeleteLogger();
	static void Log(const char* info);
	static void LogWarning(const char* warning, WARNING_SEVERITY severity = WARNING_SEVERITY::LOW);
private:
	static Logger* m_logger;
};