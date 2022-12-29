#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include <memory>
#include "thread.h"
#include <ucontext.h>
#include <functional>

namespace sylar{

class Scheduler;

/**
* @brief 协程类
*/

class Fiber: public std::enable_shared_from_this<Fiber>{
friend class Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    /**
    * @brief 协程状态
    */
    enum State{
        INIT,       // 初始化状态
        HOLD,       // 暂停状态
        EXEC,       // 执行中状态
        TERM,       // 结束状态
        READY,      // 可执行状态
        EXCEPT      // 异常状态
    };

private:
    /**
     * @brief 无参构造函数
     * @attention 每个线程第一个协程的构造
     */

    Fiber();

public:

    /**
     * @brief 构造函数
     * @param[in] cb 协程执行的函数
     * @param[in] stacksize 协程栈大小
     * @param[in] use_caller 是否在MainFiber上调度
     */
    Fiber(std::function<void()>cb, size_t stacksize = 0,bool use_caller = false);

    /**
     * @brief 析构函数
     */    
    ~Fiber();

    /**
     * @brief 重置协程执行函数,并设置状态
     * @pre getState() 为 INIT, TERM, EXCEPT
     * @post getState() = INIT
     */
    // 重置协程函数，并重置状态
    // INIT，TERM
    void reset(std::function<void()> cb);

    /**
     * @brief 将当前协程切换到运行状态
     * @pre getState() != EXEC
     * @post getState() = EXEC
     */
    // 切换到当前协程执行
    void swapIn();


    /**
     * @brief 将当前协程切换到后台
     */
    // 切换到后台
    void swapOut();

    /**
     * @brief 将当前线程切换到执行状态
     * @pre 执行的为当前线程的主协程
     */
    void call();
    
    
    /**
     * @brief 将当前线程切换到后台
     * @pre 执行的为该协程
     * @post 返回到线程的主协程
     */    
    void back();

    /**
     * @brief 返回协程id
     */
    uint64_t getId() const{return m_id;}


    /**
     * @brief 返回协程状态
     */
    State getState() const{return m_state;}

public:
    // 设置当前协程
    void SetThis(Fiber* f);
    
    // 返回当前协程
    static Fiber::ptr GetThis();
    
    // 协程切换到后台，并且设置为ready状态
    static void YieldToReady();
    
    // 协程切换到后台，并且设置为hold状态
    static void YieldToHold();
    
    // 总协程数
    static uint64_t TotalFibers();

    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程主协程
     */
    static void MainFunc();
    
    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程调度协程
     */    
    static void CallerMainFunc();

    // 获取协程id
    static uint64_t GetFiberId();

private:
    uint64_t m_id = 0;              // 协程id
    uint32_t m_stakesize = 0;       // 协程运行栈大小
    State m_state = INIT;           // 协程状态

    ucontext_t m_ctx;               // 协程上下文
    void* m_stack = nullptr;        // 协程运行栈指针

    std::function<void()> m_cb;     // 协程运行函数

};

}

#endif