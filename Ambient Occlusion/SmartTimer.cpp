#include "SmartTimer.h"

#define __CHECK_FOR_ERRORS 	{GLenum errCode; if ((errCode = glGetError()) != GL_NO_ERROR) printf("Error (%d): %s in file %s at line %d !\n", errCode, glad_glGetString(errCode), __FILE__,  __LINE__);}

SmartTimer::SmartTimer(const std::string& taskName):
    taskName(taskName), 
    startTime(std::chrono::high_resolution_clock::now()) 
{
    __CHECK_FOR_ERRORS
    glGenQueries(1, &query);
    __CHECK_FOR_ERRORS
    glBeginQuery(GL_TIME_ELAPSED, query);
    __CHECK_FOR_ERRORS
}

SmartTimer::~SmartTimer() 
{
    __CHECK_FOR_ERRORS
    glEndQuery(GL_TIME_ELAPSED);
    if (RenderData::enableLogging)
    {
        GLuint64 timeElapsed = 0;
        glGetQueryObjectui64v(query, GL_QUERY_RESULT, &timeElapsed);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
        double durationMs = duration / 1e6;

        std::stringstream ss;
        ss << std::fixed << std::setprecision(6) << timeElapsed / 1e6 << "\n";

        
        RenderData::logger.AddLog(taskName, ss, timeElapsed);
    }
    glDeleteQueries(1, &query);
    __CHECK_FOR_ERRORS
}