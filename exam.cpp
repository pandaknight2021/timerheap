#include "timerheap.h"

using namespace pandaknight;
using namespace std;

TimerManager* tmr;
TimerPtr timer;

bool stop = false;

void cb(void*p){
    TimerPtr* timer = (TimerPtr*)p;
    cout << timer << "  " << tmr->Now() << endl;
    if(stop) {
        tmr->KillTimer(*timer);
    }
}

int main() {
    
    tmr = &TimerManager::getInstance();     // 获取TimerManager singleton
  
    std::thread([]{
        
        timer = tmr->SetTimer(0, 10, cb, &timer);    //启动异步定时器 period = 10 * time_base 
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        stop = true;
    }).detach();
   
   while(!stop){
       this_thread::sleep_for(std::chrono::milliseconds(1));    // time_base = 1ms
       tmr->OnTick();          //驱动定时器时钟步进
   }

}
