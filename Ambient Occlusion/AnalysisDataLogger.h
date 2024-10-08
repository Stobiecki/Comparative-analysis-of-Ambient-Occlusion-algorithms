#pragma once

#include <iostream>
#include <sstream>
#include <tuple>
#include <map>
#include <vector>
#include <string>
#include <fstream>

class AnalysisDataLogger
{
public:
	int loggedDataLimit = 1;
	unsigned int currentIterations = 0;
	std::string rootFolder = "logs/";
	std::map<std::string, long long> files;

	long long lastRecord = 0;

	bool AddFileToLogger(std::string);
	void LogDataLimitPerSet(int iterations);
	bool CheckIterationLimit();
	void DisplayStatistics();
	void AddLog(std::string fileName, std::stringstream &dataSS, long long time = 0);
	void ClearLogFile(std::string fileName);
};
