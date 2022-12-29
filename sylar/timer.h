#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__

#include <memory>
#include "thread.h"

namespace sylar{

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer>{
friend class TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

private:
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

private:
    bool m_recurring = false;           // 是否循环执行定时器
    uint64_t m_ms = 0;                  // 定时器的循环间隔
    uint64_t m_next = 0;                // 精确的执行时间
    std::function<void()> m_cb;
    TimerManager* m_manager = nullptr;

private:
    struct Comparator{
        bool operator()(const Time::ptr& lhs, const Timer::ptr& rhs) const;
    }
};


class TimerManager{
friend class Timer;

public:
    typedef WRMutex RWMutexType;

    TimerManager();
    virtual ~TimerManager();

    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb
                                , std::weak_ptr<void> weak_cond, bool recurring = false);
protected:
      virtual void onTimerInsertedAtFront() = 0;
private:
    RWMutexType m_mutex;
    std::set<Timer::ptr,Timer::Comparator> m_timers;
};

}


#endif