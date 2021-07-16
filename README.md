English | [🇨🇳中文](README_ZH.md)

## 📖 Introduction

timerheap implements a cross_platform async timerManager base on c11, The timer scheduling implementation here is based on min heap by std::priority_queue. 


## interface

```cpp
//singleton
static TimerManager& getInstance(); 

// set a timer 
pTimer SetTimer(uint32 msDelay, uint32 msPeriod,void (*timer_cb)(void*), void* arg);

// stop a timer 
void KillTimer(pTimer tmr)

//timebase tick
uint32 Now();

//you must call OnTick period, it used to schedule timers
void OnTick();

```




## usage


``` cpp
#include "timerheap.h"

using namespace pandaknight;
using namespace std;

TimerManager* tmr;
pTimer timer;

bool stop = false;

void cb(void*p){
    pTimer* timer = (pTimer*)p;
    cout << timer << "  " << tmr->Now() << endl;
    if(stop) {
        tmr->KillTimer(*timer);
    }
}

int main() {
    
    tmr = &TimerManager::getInstance();     //  singleton
  
    std::thread([]{
        
        timer = tmr->SetTimer(0, 10, cb, &timer);    //set a timer,  period = 10 * time_base 
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        stop = true;
    }).detach();
   
   while(!stop){
       this_thread::sleep_for(std::chrono::milliseconds(1));    // time_base = 1ms
       tmr->OnTick();          //tick step
   }

}

```



## 📄 License

 [MIT opensource](/LICENSE)
