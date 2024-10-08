#pragma once

#include <iostream>
#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>

#include "RenderData.h"

class SmartTimer {
private:
    std::string taskName;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    GLuint query;

public:
    SmartTimer(const std::string& taskName);
    ~SmartTimer();
};