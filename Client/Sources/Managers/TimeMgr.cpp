#include "pch.h" 
#include "TimeMgr.h"

TimeMgr::TimeMgr()
    : dDT(0.0)
    , dAcc(0.0)
    , iCallCount(0)
    , iFPS(0)
{
}

TimeMgr::~TimeMgr() {
}

void TimeMgr::Init() {
    prevTime = std::chrono::high_resolution_clock::now();
}

void TimeMgr::Update() {
    curTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = curTime - prevTime;
    dDT = elapsed.count();

    double targetFrameTime = 1.0 / MAXFPS;
    
    if (dDT < targetFrameTime) {
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((targetFrameTime - dDT) * 1000)));
        curTime = std::chrono::high_resolution_clock::now();
        elapsed = curTime - prevTime;
        dDT = elapsed.count();
    }
    prevTime = curTime;
}

void TimeMgr::Render() {
    ++iCallCount;
    dAcc += dDT;

    if (dAcc >= 1.0) {
        iFPS = iCallCount;
        dAcc = 0.0;
        iCallCount = 0;
        
        std::ostringstream oss;
        oss << "FPS: " << iFPS << ", DT: " << dDT;
        glutSetWindowTitle(oss.str().c_str());
        // std::cout << "FPS: " << iFPS << ", Delta Time: " << dDT << std::endl;
    }
}
