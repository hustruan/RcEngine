#include <Core/Timer.h>
#include <windows.h>

namespace RcEngine {

double SystemClock::SecondsPerCount;
int64_t SystemClock::StartTime;

void SystemClock::InitClock()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	SecondsPerCount = 1.0 / double(frequency.QuadPart);

	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);
}

void SystemClock::ShutClock()
{

}

uint64_t SystemClock::Now()
{
	int64_t now; 
	QueryPerformanceCounter((LARGE_INTEGER*)&now);
	return uint64_t(now - StartTime);
}




//#if defined(RcWindows)
//	static double __micro_second_per_count;
//	static __int64 __time_at_init;
//
//	void InitSystemClock()
//	{
//		__int64 countsPerSec;
//		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
//		__micro_second_per_count = 1000.0 / (double)countsPerSec;
//
//		QueryPerformanceCounter((LARGE_INTEGER*)&__time_at_init);
//	}
//
//	void ShutSystemClock() {}
//
//	double GetTimeMS()
//	{
//		__int64 now;
//		QueryPerformanceCounter((LARGE_INTEGER*)&now);
//		return (now-__time_at_init) * __micro_second_per_count;
//	}
//#endif


Timer::Timer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}


float Timer::GetGameTime() const
{
	if(mStopped)
	{
		return (float)((mStopTime - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)
			((mCurrTime - mBaseTime - mStopTime) * mSecondsPerCount);
	}
}


float Timer::GetDeltaTime() const
{
	return (float)mDeltaTime;
}

void Timer::Tick()
{
	if(mStopped)
	{
		mDeltaTime = 0;
		return;
	}

	// Get Time this frame
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrTime = currentTime;

	// Times difference between this and previous frame
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	// Prepare for next frame
	mPrevTime = mCurrTime;
	
	// Force nonnegative. The DXSDK's CDXUTTimer mentions that if the
	// processor goes into a power save mode or we get shuffled to
	// another processor, then mDeltaTime can be negative.
	if(mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Stop()
{
	if(!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}




} // Namespace RcEngine