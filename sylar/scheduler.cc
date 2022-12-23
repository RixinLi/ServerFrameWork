#include "scheduler.h"
#include "log.h"



namespace sylar{

    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system"); 

    Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name){

    }
    ~Scheduler::Scheduler(){

    }

    Scheduler* Scheduler::GetThis(){

    }
    Fiber* Scheduler::GetMainFiber(){

    }

    void Scheduler::start(){

    }
    void Scheduler::stop(){
        
    }


}