#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <fstream>
#include <mutex>  

#define LogInfo(...) \
    Logger::instance().writeLog("INFO",__FILE__,__LINE__,__VA_ARGS__)

#define LogWarning(...) \
    Logger::instance().writeLog("WAINING",__FILE__,__LINE__,__VA_ARGS__)

#define LogError(...) \
    Logger::instance().writeLog("Error",__FILE__,__LINE__,__VA_ARGS__)

class Logger{
public:
    Logger(const Logger&)=delete;
    Logger& operator=(const Logger&)=delete;

    static Logger& instance();
    bool writeLog(const std::string& level,const char* file,int line,const char* format,...);

private:
    Logger();
    ~Logger();
    
    std::string filename;
    std::fstream logfile;
    std::mutex m;
};

#endif
