[英文](README.md) | 🇨🇳中文

## 📖 简介

TimerManager 是一个基于C11 实现的跨平台异步定时器管理器, 内部调度采用最小堆结构, 基于std::priority_queue实现.



## 接口：

```cpp
//单例接口
static TimerManager& getInstance(); 

// 设置定时器
TimerPtr SetTimer(uint32 msDelay, uint32 msPeriod,void (*timer_cb)(void*), void* arg);

//关闭定时器 
void KillTimer(TimerPtr tmr)

//返回当前 timebase tick
uint32 Now();

//定时timebase 步进函数, 需周期调用本函数完成定时器的调度管理,可采用select/sleep/或其他定时器等完成
void OnTick();

```




## 使用


``` cpp
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

```

