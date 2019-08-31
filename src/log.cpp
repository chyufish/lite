#include "log.h"

#include <cstdarg>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <mutex>
#include <iomanip>

Logger::Logger(){
    filename="server.log";
    logfile.open(filename,std::ios::out | std::ios::app);
}
Logger::~Logger(){
    logfile.close();
}
Logger& Logger::instance(){
    static Logger logger;
    return logger;
}

bool Logger::writeLog(const std::string& level,const char* file,int line,const char* format,...){
    if(!logfile.is_open()){
        LogError("file open failed\n");
        return false;
    }
    char buffer[512]={0};
    va_list va;
    va_start(va,format);
    vsnprintf(buffer,512,format,va);
    va_end(va);

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::string record=std::string(level)+": at "+file+":"+std::to_string(line)+": "+buffer;
    {
        std::lock_guard<std::mutex> lock(m);
        logfile<<std::put_time(std::localtime(&now_c), "%F %T")<<" ";
        logfile<<record<<std::endl;
    }
    return true;
}
