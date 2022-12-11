#ifndef _SYLAR_UTIL_H
#define _SYLAR_UTIL_H

#include<pthread.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>

namespace sylar{

pid_t GetThreadID(); // 获取线程ID
u_int32_t GetFiberId(); // 获取协程ID

}

#endif 
