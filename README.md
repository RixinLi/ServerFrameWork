This is a learning project for ServerFrameWork

Learning videos: https://www.bilibili.com/video/BV184411s7qF/?spm_id_from=333.1007.top_right_bar_window_custom_collection.content.click&vd_source=e6f95788b99a9f46202928a5a17cb640

# sylar

# 开发环境
Centos7
gcc 9.1
Cmake

## 项目路径


## 日志系统
	1)
	Log4J
	Logger(定义日志类别）
	
		|
		|------- ------- Formatter (日志格式）
		|
	Appender（日志输出地方）

## 配置系统

Config --> Yaml

Yaml-cpp: github 搜

make lib && cd lib && cmake .. && cmake install

```cpp
YAML::Node node = YAML::Loadfile("file path");

node.IsMap()
for (auto it = node.begin();it!=node.end();it++){it->first,it->second}

node.IsSequence()
for (size_t i=0;i<node.size();i++){}

node.IsScalar();
```
配置系统的原则，约定大于配置

```cpp
template<class T, class FromStr, class ToStr>
class configVar;

template<class F, class T>
LexicalCast;

// 容器偏特化: 目前支持vector
// list, set, map, unordered_map, unordered_set
// map/unordered_map 支持key=std::string
// Config::LookUp(key), key相同，类型不同的不会报错，这个需要处理一下
```

自定义类型，需要实现sylar::LexicalCast，偏特化
实现后，就可以支持Config解析自定义类型，自定义类型可以和常规stl容器一起使用。

配置的事件机制
当一个配置事件发生更改时，可以反向通知代码，回调

# 日志系统整合配置系统
```yaml
logs:
	- name: root
	  level: (debug, info, warn, error, fatal)
          formatter: '%d%T%p%T%t%T%m%n'
	  appender:
	    - type: (StdoutLogAppender, FileLogAppender)
	      level: (debug, info, warn, error, fatal)
	      file: /logs/xxx.log
```

```cpp
	sylar::Logger g_logger =  sylar::LoggerMgr::GetInstance()->getLogger(name)
	SYLAR_LOG_INFO(g_logger)<<"xxx log";
```

```cpp
static Logger::ptr g_log = SYLAR_LOG_NAME("system"); // m_root
// m_root , m_system -> m_root 
当logger的appenders为空，使用root写logger
```

```cpp

```
遗留问题：
1. appender定义的formatter读取yaml的时候，没有被初始化
2. 去掉额外的调试日志
3. 文件名问题
4. 总结：ProcessOn

## 线程库

Thread, Mutex, 
Pthread

pthread pthread_create

信号量：semaphore
互斥量: mutex

读写锁: sylar::WRMutex::ReadLock sylar::WRMutex::WriteLock

和log整合
Logger, Appender, Formatter

SpinLock 替换 Mutex

写文件周期性reopen，防止文件被删除了，未被感知

和config整合

## 协程库封装

定义协程接口
ucontext_t
macro

```
Fiber::GetThis() // 创建主协程
Thread->main_fiber -----> sub_fiber // 使用swapin 切换子协程
	    ^
	    |
	    |
	    v
PutMessage(msg,) +信号量+1
Message_queue
    |
    |---Thread
    |---Thread
    V	wait()-信号量 RevMessage(msg,1_
    信号量

异步IO，等待数据返回。epoll_wait

epoll_create, epoll_ctl, epoll_wait

```

协程调度模块schedular
```
	1->N	1->N	
schedular-->thread--> fiber
1. 线程池，分配一组线程
2. 协程调度器，将协程指定到相应的线程执行
3. 

N:M

m_threads
<function<void()>, fiber, threadid> m_fibers

schedule(func/fiber)

start()
stop()
run()

1. 设置当前线程的scheduler
2. 设置当前线程的run，fiber
3. 协程调度循环while(true)
	1.协程消息队列里面是否有队列
	2.无任务执行idle方法
```

```
IOManager(epoll)-->Scheduler
	|
	|
	V
	idle(epoll_wait)

```

```cpp
timer-> addTime()
timer-> cancel()
获取当前的定时器触发离现在的时间差
返回当前需要的定时器
```
```
	[FIBER]				[Timer]
	   ^ N				^
	   |				|
	   | 1				|
	[Thread]		[TimerMananger]
	    ^N			 ^
	    |			 |	
	    |1			 |
	[Scheduler] <---- [IOManager(epoll)]
```

## HOOK
sleep,
usleep,

socket 相关的(socket, connect, accept)
io相关(read, write, send, recv, ...)
fd相关(fcntl, iocntl, ...)



## socket函数库
		 [UnixAddress]
		-----------	
		     |	
		-----------	
						| [IPV4Address]
		| Address | --- [IPAddress] --- |
						| [IPV6Address]
		-----------
		     |
		     |
		  -------
		  | socket |
		  ---------
		  connect, accept, read, write,close
		     
		     
## 序列化 byteArray

write(int,float,int64,...)
read(int,float,int64,...)

## http协议开发

## 分布协议

## 推荐系统
