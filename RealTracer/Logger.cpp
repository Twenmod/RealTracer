#include "precomp.h"
#include "Logger.h"

Logger* Logger::m_logger = 0;

void Logger::CreateLogger()
{
	m_logger = new Logger();
}

void Logger::DeleteLogger()
{
	delete m_logger;
}

void Logger::Log(const char* info)
{
	std::clog << "\x1B[96m" << info << "\n \x1B[0m";
}

void Logger::LogWarning(const char* warning, WARNING_SEVERITY severity)
{
	switch (severity) {
		case WARNING_SEVERITY::LOW:
			std::clog << "\x1B[93m ! ";
			break;
		case WARNING_SEVERITY::MEDIUM:
			std::clog << "\x1B[33m WARNING ";
			break;
		case WARNING_SEVERITY::HIGH:
			std::clog << "\x1B[91m -WARNING- ";
			break;
		case WARNING_SEVERITY::FATAL:
			std::clog << "\x1B[31m  \x1B[4m- FATAL -\x1B[24m  \x1B[5m";
			break;
	}

	std::clog << warning << "\n \x1B[0m";
}
