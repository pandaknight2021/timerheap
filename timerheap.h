#ifndef __TIMER_HEAP_H__
#define __TIMER_HEAP_H__

#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector
#include <queue>        // priority_queue
#include <functional>     // std::greater
#include <memory>       // shared_ptr
#include <chrono>
#include <thread>       // std::
#include <mutex>
#include <condition_variable>
#include <iostream>


namespace pandaknight{

typedef char int8;
typedef unsigned char uint8;
typedef uint8 byte;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

struct Timer{
    uint32 expires;
    uint32 period;
    void (*timer_cb)(void*);
    void *param;
    ~Timer(){
        std::cout << "~Timer: " << period << std::endl;
    }
};


using TimerPtr = std::shared_ptr<Timer>;

class TimerManager{
public:
    TimerManager(TimerManager const&)    = delete;
    void operator=(TimerManager const&)  = delete;


    static TimerManager& getInstance()
    {
        static TimerManager instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }
 

    TimerPtr SetTimer(uint32 msDelay, uint32 msPeriod,void (*timer_cb)(void*), void* arg);
    void OnTick();


    void KillTimer(TimerPtr tmr){
        if(tmr.get() != nullptr){
            tmr->timer_cb = nullptr;
        }
    }
   
    uint32 Now(){
        return _tick;
    }

    ~TimerManager(){
        {
            std::unique_lock<std::mutex> lk(mu);
            _exit = true;
        }
        cv.notify_one();
        std::cout << "~TimerManager: heap_size=" << min_heap.size() << std::endl;
    }

private:
    TimerManager();

    struct timercmp{
        bool operator()(const TimerPtr& lhs, const TimerPtr& rhs) {
            return lhs->expires > rhs->expires;
        }
    };
    
    uint32 _tick;
    uint32 _next_expires;
    std::priority_queue<TimerPtr,std::vector<TimerPtr>, timercmp> min_heap;
    std::mutex mu;
    std::condition_variable cv;
    std::queue<std::function<void()> > q;
    bool _exit;
};



}



#endif
