/*
//NetCalculations
//DMChrono.cpp/h
===================
//Полуботко Дмитрий
//2008
*/
//---------------------------------------------------------------------------

#ifdef __BORLANDC__
    #include <vcl.h>
#endif

#ifdef _WIN32
    #include <dos.h>
    #include <windows.h>
#endif

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "DMChrono.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//Методы класса DMChrono
//---------------------------------------------------------------------------
DMChrono::DMChrono()
    : mFirstTime(0.0)
{
//Конструктор

#ifdef _WIN32
    QueryPerformanceFrequency((LARGE_INTEGER*)&Freq);
#else
    clock_gettime(CLOCK_MONOTONIC, &tv_start);
#endif
}
//---------------------------------------------------------------------------
void DMChrono::Start()
{

    if(Times.empty())
    {
        mFirstTime = GetTime();
    }

    Times.push(GetTime());
}
//---------------------------------------------------------------------------
double DMChrono::Finish()
{
    if(Times.empty())
    {
        return 0.0;
    }

    double Now=GetTime();
    double dt=Now-Times.top();
    Times.pop();
    return dt;
}
//---------------------------------------------------------------------------
double DMChrono::GetTime() const
{
#ifdef _WIN32
    __int64 ThisTick;
    QueryPerformanceCounter((LARGE_INTEGER*)&ThisTick);
    return double(ThisTick)/double(Freq);

#else
    timespec tv;
    clock_gettime(CLOCK_MONOTONIC, &tv);
    return ((tv.tv_sec - tv_start.tv_sec - 1) * 1000 + (tv.tv_nsec/1000 + 1000000 - tv_start.tv_nsec/1000) / 1000) / 1000.0;

#endif
}

double DMChrono::GetTimeDiffFromFirst() const
{
    return GetTime() - mFirstTime;
}
//---------------------------------------------------------------------------
//Методы класса DMChrono END
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
