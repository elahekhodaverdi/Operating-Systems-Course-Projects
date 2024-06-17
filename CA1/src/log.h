#ifndef __LOG_H
#define __LOG_H


void logNormal(int fd,const char* msg);
void logInput(int fd,const char* msg);
void logMsg(int fd,const char* msg);
void logInfo(int fd,const char* msg);
void logWarning(int fd,const char* msg);
void logError(int fd ,const char* msg);
int open_log_file(char*name);
#endif 
