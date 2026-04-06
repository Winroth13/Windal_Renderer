#pragma once
#include <string>

class Logger
{
public:
	static void Info(std::string text);
	static void Error(std::string text);
	static void Warn(std::string text);
};