#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector
#include <queue>
#include <functional>     // std::greater
#include <memory>
#include <chrono>
#include "timerheap.h"


namespace pandaknight{


TimerPtr TimerManager::SetTimer(uint32 msDelay, uint32 msPeriod,void (*timer_cb)(void*), void* arg){
    if(timer_cb != nullptr){
        TimerPtr tmr = std::make_shared<Timer>();
      
        if(msDelay > 0){
            tmr->expires = _tick + msDelay;
        }else{
            tmr->expires = _tick + msPeriod;
        }
        tmr->period = msPeriod;
        tmr->timer_cb = timer_cb;
        tmr->param = arg;

        min_heap.push(tmr);

        _next_expires = min_heap.top()->expires;

        return tmr;
    }else{
        return nullptr;
    }
}


void TimerManager::OnTick(){
    int cnt = 0;
    _tick++;                                // time base 

    if(min_heap.empty() || _tick < _next_expires) return;

    while(min_heap.size()){
        auto tmr = min_heap.top();
        if(tmr->timer_cb){
            if(_tick < tmr->expires){
                _next_expires = tmr->expires;
                if(cnt) cv.notify_one();
                break;
            }else{
                cnt++;
                {
                    std::unique_lock<std::mutex> lock(mu);
                    q.emplace([tmr]{
                        tmr->timer_cb(tmr->param);
                    });
                    //tmr->timer_cb(tmr->param);        
                }
                min_heap.pop();
                if(tmr->period > 0){
                    tmr->expires = _tick + tmr->period;
                    min_heap.push(tmr);                    
                }
            }
        }else{
            min_heap.pop();
        }
    }
}

TimerManager::TimerManager(): _tick(0),_exit(false),_next_expires(0) {
        std::thread([this]{
            while(1){
                {
                    std::unique_lock<std::mutex> lk(this->mu);
                    this->cv.wait(lk, [this]{return this->_exit || !this->q.empty();});
                }
                if(this->_exit) break;

                while(!this->q.empty()){
                    auto fn = q.front();
                    fn();
                    q.pop();
                }
            }
        }).detach();
    }

}
