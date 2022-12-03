#ifndef _SYLAR_LOG_H
#define _SYLAR_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <stringstream>
#include <fstream>

namespace sylar{

// 日志事件
class LogEvent{
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();

        const char* getFile() const{ return m_file;}
        int32_t getLine() const {return m_line;}
        uint32_t getElapse() const{return m_elapse;}
        uint32_t getThreadId() const{return m_threadId;}
        uint32_t getFiberId() const{return m_fiberId;}
        uint64_t getTime() const{return m_time;}
        const std::string& getContent() const{return m_content;}

    private:
        const char* m_file = nullptr; // 文件名
        int32_t m_line = 0;         // 行号
        uint32_t m_elapse = 0;      // 程序启动到现在的毫秒数
        int32_t m_threadId = 0;     // 线程id
        uint32_t m_fiberId = 0;     // 协程id
        uint64_t m_time;            // 时间戳
        std::string m_content;      // 内容
}

// 日志级别
class LogLevel{
    public:    
        enum Level {
                UNKNOW = 0,
                DEBUG = 1,
                INFO = 2,
                WARN = 3,
                ERROR = 4,
                FATAL = 5
            };

        static const char* ToString(LogLevel::Level level); 
}

// 日志格式器
class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(LogLevel::Level level,const std::string& pattern);

        //%t    %thread_id %m%n
        std::string format(LogEvent::ptr event);

    public:
        
        class FormatItem {
            public:
                typedef std::shared_ptr<FormatItem> ptr;
                virtual ~FormatItem(){}
                virtual void format(std::ofstream& os, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

        void init();

    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
};



// 日志输出地址
class LogAppender{
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();

        virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter:;ptr val){m_formatter = val;}
        LogFormatter::ptr getFormatter() const{return m_formatter;}

    protected:
        LogLevel::level m_level;
        LogFormatter::ptr m_formatter;
};


// 日志输出器    
class Logger{
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string& name = "root");
        void log(LogLevel::Level level, LogEvent::ptr LogEvent);
        
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level get Level() const {return m_level;}
        void setLevel(LogLevel::Level val){m_level = val;}

    private:
        std::string m_name;      //日志名称
        LogLevel::level m_level; //日志级别
        std::list<LogAppender::ptr> m_appenders;        // Appender集合
};


// 输出到控制台的appender
class StdoutLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(LogLevel::Level level, LogEvent::ptr event) override;
    private:    
};

// 输出到文件的appender
class FileLogAppender: public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        
        void log(LogLevel::Level level, LogEvent::ptr event) override;

        // 重新打开文件，文件打开成功返回true
        bool reopen();

    private:
        std::string m_filename;
        std::ofstream m_filestream;
};


}
#endif