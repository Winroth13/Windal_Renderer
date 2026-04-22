#include "core/logger.h"
#include <iostream>
#include <Windows.h>

#include <stdarg.h>
#include <stdio.h>

void Logger::Info(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << "INFO: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << text << std::endl;
#endif
}

void Logger::Error(const std::string& text)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << "ERROR: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << text << std::endl;
}

void Logger::Warn(const std::string& text)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	std::cout << "WARN: ";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << text << std::endl;
#endif
}