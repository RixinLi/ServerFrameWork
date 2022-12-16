#include "log.h"
#include <iostream>
#include <functional>
#include <map>
#include <time.h>
#include <string.h>
#include "config.h"

namespace sylar{

const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define XX(name) \
        case LogLevel::name:\
            return #name; \
            break;

        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default: return "UNKNOW";
    }
    return "UNKNOW";
}


LogEventWrap::LogEventWrap(LogEvent::ptr e): m_event(e){
    
}

LogEventWrap::~LogEventWrap(){
    m_event->getLogger()->log(m_event->getLevel(),m_event);
}

void LogEvent::format(const char* fmt, ...){
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}


void LogEvent::format(const char* fmt, va_list al){
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if (len != -1){
        m_ss << std::string(buf,len);
        free(buf);
    }
}


std::stringstream& LogEventWrap::getSS(){
    return m_event->getSS();
}


    class MessageFormatItem : public LogFormatter::FormatItem{
    public:
        MessageFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem{
    public:
        LevelFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem{
    public:
        ElapseFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem{
    public:
        NameFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getLogger()->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem{
    public:
        ThreadIdFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem{
    public:
        FiberIdFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getFiberId();
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem{
    public:
        
        DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S"): m_format(format){
            if (m_format.empty()){
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time,&tm);
            char buf[64];
            strftime(buf,sizeof(buf),m_format.c_str(),&tm);
            os<< buf;
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem{
    public:
        FilenameFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem{
    public:
        LineFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem{
    public:
        NewLineFormatItem(const std::string& str = ""){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< std::endl;
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem{
    public:
        StringFormatItem(const std::string& str): m_string(str){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< m_string;
        }
    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem{
    public:
        TabFormatItem(const std::string& str=""): m_string(str){}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override{
            os<< "\t";
        }
    private:
        std::string m_string;
    };



LogEvent::LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
        const char* file, int32_t line, uint32_t elaspse, 
        uint32_t thread_id, uint32_t fiber_id, uint64_t time)
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elaspse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_logger(logger)
        ,m_level(level){

}


Logger::Logger(const std::string& name)
    : m_name(name)
    , m_level(LogLevel::DEBUG){
    m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
}

void Logger::addAppender(LogAppender::ptr appender){
    if (!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}


void Logger::delAppender(LogAppender::ptr appender){
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if (*it==appender){
            m_appenders.erase(it);
            break;
        }
    }
}


void Logger::log(LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level){
        auto self = shared_from_this();
        if (!m_appenders.empty()){
            for (auto& i : m_appenders){
                i->log(self,level,event);
            }
        }else if (m_root){
            m_root->log(level,event);
        }
    }
}  


void Logger::debug(LogEvent::ptr event){
     log(LogLevel::DEBUG,event);
}

void Logger::info(LogEvent::ptr event){
     log(LogLevel::INFO,event);
}
void Logger::warn(LogEvent::ptr event){
     log(LogLevel::WARN,event);
}
void Logger::error(LogEvent::ptr event){
     log(LogLevel::ERROR,event);
}
void Logger::fatal(LogEvent::ptr event){
     log(LogLevel::FATAL,event);
}


// LogAppender 含两种重载

FileLogAppender::FileLogAppender(const std::string& filename): m_filename(filename){
    reopen();
}

void FileLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level){
        m_filestream << m_formatter->format(logger,level,event);
    }
}

bool FileLogAppender::reopen(){
    if (m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event){
    if (level >= m_level){
        std::cout<<m_formatter->format(logger,level,event);
    }
}


// LogFormatter 函数实现

LogFormatter::LogFormatter(const std::string& pattern): m_pattern(pattern){
    init();
}


std::string LogFormatter::format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event){
    std::stringstream ss;
    for (auto &i : m_items){
        i->format(ss,logger,level,event);
    }
    return ss.str();
}

// %xxx %xxx{xxx} %%
void LogFormatter::init(){
    
    // tuple : {str,fmt,type}

    std::vector<std::tuple<std::string,std::string,int>> vec;

    std::string nstr;

    for (size_t i = 0 ; i< m_pattern.size() ; i++){

        if (m_pattern[i] != '%'){
            nstr.append(1,m_pattern[i]);
            continue;
        }

        if ( (i+1)<m_pattern.size() && m_pattern[i+1]=='%'){
            nstr.append(1,'%');
            continue;
        }

        size_t n = i+1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str="",fmt="";

        while( n < m_pattern.size() ){

            if (!fmt_status && !isalpha(m_pattern[n]) && m_pattern[n]!='{' && m_pattern[n]!='}'){
                str = m_pattern.substr(i+1,n-i-1);
                break;
            }

            if (!fmt_status && m_pattern[n]=='{'){
                str = m_pattern.substr(i+1,n-i-1);
                fmt_status = 1;
                fmt_begin = n;
                ++n;
                continue;
            }

            if (fmt_status && m_pattern[n]=='}'){
                fmt = m_pattern.substr(fmt_begin+1,n-fmt_begin-1);
                fmt_status = 0;
                fmt_begin = 0;
                ++n;
                break;
            }
            n++;
            if (n==m_pattern.size() && str.empty()) str = m_pattern.substr(i+1);
        }

        if (!fmt_status){
            if (!nstr.empty()){
                vec.push_back(std::make_tuple(nstr,"",0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str,fmt,1));
            i = n-1;
        }else{
            std::cout<<"pattern parse error:"<<m_pattern<<"-"<<m_pattern.substr(i)<<std::endl;
            //m_error=true;
            vec.push_back(std::make_tuple("<<pattern_error>>","",0));
        }

    }

    if (!nstr.empty()) vec.push_back(std::make_tuple(nstr,"",0));

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str,C) \
        { #str, [](const std::string& fmt) {return FormatItem::ptr(new C(fmt));} }

        XX(m,MessageFormatItem),
        XX(p,LevelFormatItem),
        XX(r,ElapseFormatItem),
        XX(c,NameFormatItem),
        XX(t,ThreadIdFormatItem),
        XX(n,NewLineFormatItem),
        XX(d,DateTimeFormatItem),
        XX(f,FilenameFormatItem),
        XX(l,LineFormatItem),
        XX(T,TabFormatItem),
        XX(F,FiberIdFormatItem),
#undef XX
    };

    // %m -- 消息体 
    // %p -- level
    // %r -- 启动后的时间
    // %c -- 日志名称
    // %t -- 线程ID
    // %n -- 回车
    // %d -- 时间
    // %f -- 文件名
    // %l -- 行号

    for (auto& i : vec){
        if (std::get<2>(i) == 0){
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }else{
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
      //  std::cout<< "("<<std::get<0>(i) << ") - ("<< std::get<1>(i) << ") - (" << std::get<2>(i) << ")"<<std::endl;
    }

    //std::cout<< m_items.size() <<std::endl;

}

LoggerManager::LoggerManager(){
    m_root.reset(new Logger);
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    init();
}

Logger::ptr LoggerManager::getLogger(const std::string& name){
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()){
        return it->second;
    }
    Logger::ptr logger(new Logger(name));
    logger->m_root = m_root;
    m_loggers[name] = logger;
    return logger;
}

struct LogAppenderDefine{
    int type = 0; // 1 File, 2 Stdout
    LogLevel::Level level LogLevel::UNKNOW;
    std::string formatter;
    std::string file;
    bool operator==(const LogAppenderDefine& oth) const{
        return type ==  oth.type &&
               level == oth.level &&
               formatter == oth.formatter &&
               file == oth.file;
    }

};

struct LogDefine{
    std::string name;
    LogLevel::Level level LogLevel::UNKNOW;
    std::string formatter;  
    std::vector<LogAppenderDefine> appenders;

    bool operator==(const LogDefine& oth) const{
        return name == oth.name
                && level == oth.level
                && formatter == oth.formatter
                && appenders == oth.appenders;
    }

    bool operator<(const LogDefine& oth) const{
        return name < oth.name;
    }
};

sylar::ConfigVar<std::set<LogDefine> >::ptr g_log_defines = 
    sylar::Config::Lookup("logs",std::vector<LogDefine>(),"logs config");

struct LogIniter {
    LogIniter(){
        g_log_defines->addListener(0xF1E231,[](const std::set<LogDefine>& old_value, const std::set<LogDefine>& new_value ){
            
        });
    }
};

static LogIniter __log_init;

void LoggerManager::init(){

}



}
