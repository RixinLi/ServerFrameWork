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
```

## 协程库封装

## socket函数库

## http协议开发

## 分布协议

## 推荐系统
