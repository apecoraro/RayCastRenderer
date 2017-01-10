#pragma once

namespace cs500
{
    typedef __int64 Timer_t;

    class Timer
    {
    public:
        static Timer* instance();
        Timer();
        ~Timer() {};

        Timer_t tick() const;
        inline double getElapsedSecs( Timer_t t1, Timer_t t2 ) const { return (double)(t2 - t1)*m_secsPerTick; }
        inline double getElapseMilliSecs( Timer_t t1, Timer_t t2 ) const { return getElapsedSecs(t1,t2)*1e3; }
        inline double getElapsedMicroSecs( Timer_t t1, Timer_t t2 ) const { return getElapsedSecs(t1,t2)*1e6; }
        inline double getElapsedNanoSecs( Timer_t t1, Timer_t t2 ) const { return getElapsedSecs(t1,t2)*1e9; }
        inline double getSecondsPerTick() const { return m_secsPerTick; }

    private:
        double m_secsPerTick;
    };
};