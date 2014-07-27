#ifndef Timer_h__
#define Timer_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport SystemClock
{
public:
	static void InitClock();
	static void ShutClock();

	static uint64_t Now();
	static inline double ToSeconds(uint64_t timeCounts) { return timeCounts * SecondsPerCount; }

private:
	static int64_t StartTime;
	static double SecondsPerCount;
};


/** 
 * Game Timer
 */
class _ApiExport Timer
{
public:
	Timer();

	float GetGameTime() const;		// in seconds
	float GetDeltaTime() const;		// in seconds

	void Reset();		// Call before game loop
	void Start();		// Call when unpaused
	void Stop();	    // Call when paused
	void Tick();		// Call every frame

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};




} // Namespace RcEngine


#endif // Timer_h__
