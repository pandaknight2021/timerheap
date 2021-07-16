#include "timerheap.h"
#include <uv.h>
#include <iostream>
#include <thread>

using namespace pandaknight;
using namespace std;

uv_loop_t *loop;

uv_timer_t timer1;

TimerManager* tmr;

bool stop = false;

int num = 0;

void timer_callback(uv_timer_t* handle){
    
    tmr->OnTick();          //驱动定时器时钟步进
    //num++;

    if(stop) {
        cout << tmr->Now() << "  " << num << endl;
        uv_timer_stop(handle);
        uv_stop(loop);
    }
}

void cb(void*p){
    pTimer* timer = (pTimer*)p;
    //cout << timer << "  " << tmr->Now() << endl;
    if(stop) {
        tmr->KillTimer(*timer);
    }
}



int main() {
    loop = uv_default_loop();
    uv_timer_init(loop, &timer1);
    uv_timer_start(&timer1, timer_callback, 0, 1);
    
    tmr = &TimerManager::getInstance();     // 获取TimerManager singleton
  
    std::thread([]{
        pTimer timer[4];
        int interval[] = {13,3,5,7};
        timer[0] = tmr->SetTimer(0, interval[0], cb, &timer[0]);    //启动异步定时器
        timer[1] = tmr->SetTimer(9, 0, cb, &timer[1]);
        timer[2] = tmr->SetTimer(0, interval[2], cb, &timer[2]);
        timer[3] = tmr->SetTimer(0, interval[3], cb, &timer[3]);
        auto t0 = std::chrono::steady_clock::now();
        this_thread::sleep_for(std::chrono::milliseconds(1000));
        cout << " sleep(1s) elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count() << endl;
        stop = true;
    }).detach();
   
 
    return uv_run(loop, UV_RUN_DEFAULT);
}