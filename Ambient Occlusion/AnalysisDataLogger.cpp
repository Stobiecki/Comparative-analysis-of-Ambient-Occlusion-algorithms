#include "AnalysisDataLogger.h"

bool AnalysisDataLogger::AddFileToLogger(std::string fileName)
{
    if (files.find(fileName) != files.end())
        return false;

    files[fileName] = 0;
    return true;
}

void AnalysisDataLogger::LogDataLimitPerSet(int iterations)
{
    loggedDataLimit = iterations;
}

// should be called once per frame
bool AnalysisDataLogger::CheckIterationLimit()
{
    static const int percentageStep = 5;

    if (lastRecord != 0l)
        currentIterations++;

    // displaying current percentage of done iterations
    int currentPercentage = (currentIterations * 100) / loggedDataLimit;
    // Display message at every 10% completion
    if (currentPercentage % percentageStep == 0 && currentIterations % (loggedDataLimit / (100 / percentageStep)) == 0)
        std::cout << "\nCompleted " << currentPercentage << "% of iterations";

    if (currentIterations <= loggedDataLimit)
        return true;
    else
    {
        currentIterations = 0;
        return false;
    }
}
void AnalysisDataLogger::DisplayStatistics()
{
    std::cout << "\ntests are done";
    for (auto& f : files)
    {
        std::cout << "\ntest - " << f.first << " sredni czas dla " << loggedDataLimit << " testow: " << (((double)(f.second)) / loggedDataLimit) << "ns";

        std::stringstream ss;
        ss << "\n\nczas œredni wszystkich operacji: " << (((double)(f.second)) / loggedDataLimit);
        AddLog(f.first, ss);
        files[f.first] = 0;
    }
}

void AnalysisDataLogger::AddLog(std::string fileName, std::stringstream &dataSS, long long time)
{
    lastRecord = time;
    if (files.find(fileName) != files.end())
    {
        files[fileName] += time;
        std::ofstream ofs(rootFolder + fileName, std::ios::app);
        if (ofs.is_open()) 
        {
            ofs << dataSS.str();
            ofs.close();
        }
        else 
            std::cerr << "Unable to open file: " << fileName << std::endl;
    }
    else 
        std::cerr << "File not found in logger: " << fileName << std::endl;
}

void AnalysisDataLogger::ClearLogFile(std::string fileName)
{
    if (files.find(fileName) != files.end()) 
    {
        std::ofstream outFile(rootFolder + fileName, std::ios::trunc);
        if (outFile.is_open()) 
        {
            outFile.close();
            files[fileName] = 0;
        }
        else
            std::cerr << "Unable to open file: " << fileName << std::endl;
    }
    else
        std::cerr << "File not found in logger: " << fileName << std::endl;
}