#pragma once
#include <chrono>

class TimeMgr {
    SINGLE(TimeMgr);
private:
    std::chrono::high_resolution_clock::time_point  curTime;
    std::chrono::high_resolution_clock::time_point  prevTime;
    double                                          dDT;
    double                                          dAcc;
    unsigned int                                    iCallCount;
    unsigned int                                    iFPS;

public:
    void                                            Init();
    void                                            Update();
    void                                            Render();

    void                                            UpdateFPS();
    double                                          GetDT() { return dDT; }
    double                                          GetfDT() { return (float)dDT; }
};
