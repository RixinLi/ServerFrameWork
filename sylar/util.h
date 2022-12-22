#ifndef _SYLAR_UTIL_H
#define _SYLAR_UTIL_H

#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<vector>
#include<string>

namespace sylar{

pid_t GetThreadID(); // 获取线程ID
u_int32_t GetFiberId(); // 获取协程ID

void Backtrace(std::vector<std::string>& bt, int size, int skip = 1);
std::string BacktraceToString(int size, int skip = 2, const std::string& prefix = "");

}

#endif 
