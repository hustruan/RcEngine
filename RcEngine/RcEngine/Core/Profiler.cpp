#include <Core/Profiler.h>
#include <Core/Timer.h>

#define INVALID_TIME	((uint64_t)(-1))
#define INVALID_INDEX   ((size_t)(-1))

namespace RcEngine {

//////////////////////////////////////////////////////////////////////////
CpuAutoProfiler::CpuAutoProfiler( const char* name )
{
	ProfilerManager::GetSingleton().ProfilerStart(name);
}

CpuAutoProfiler::~CpuAutoProfiler()
{
	ProfilerManager::GetSingleton().ProfilerEnd();
}

//////////////////////////////////////////////////////////////////////////
ProfilerManager::ProfilerManager()
	: mCurrNumSamples(0),
	  mLastOpenedSample(INVALID_INDEX),
	  mCallStackDepth(0)
{

}

ProfilerManager::~ProfilerManager()
{

}

void ProfilerManager::ProfilerStart( const char* name )
{
	bool found = false;
	for (uint32_t i = 0; i < mCurrNumSamples; ++i)
	{
		if (mSamples[i].SampleName == name)
		{
			mSamples[i].ParentSampleIndex = mLastOpenedSample;
			mLastOpenedSample = i;

			mSamples[i].CallStackDepth = mCallStackDepth++;
			++mSamples[i].CallCount;

			found = true;
			break;
		}
	}

	if (!found)
	{
		size_t index = mCurrNumSamples++;
		assert(index < MAX_PROFILERS);
		
		mSamples[index].SampleName = name;
		mSamples[index].SampleIndex = index;

		mSamples[index].ParentSampleIndex = mLastOpenedSample;
		mLastOpenedSample = index;

		mSamples[index].CallStackDepth = mCallStackDepth++;
		mSamples[index].CallCount = 1;
		mSamples[index].TotalTime = 0;
		mSamples[index].TotalChildTime = 0;

		mSamples[index].StartTime = SystemClock::Now();
	
		if (mSamples[index].ParentSampleIndex == INVALID_INDEX)
		{
			// Root Begin
			mFrameBeginTime = mSamples[index].StartTime;
		}
	}
}

void ProfilerManager::ProfilerEnd( )
{
	uint64_t endTime = SystemClock::Now();
	uint64_t elapsedTime = endTime - mSamples[mLastOpenedSample].StartTime;

	if (mSamples[mLastOpenedSample].ParentSampleIndex != INVALID_INDEX)
	{
		size_t parentIndex = mSamples[mLastOpenedSample].ParentSampleIndex;
		mSamples[parentIndex].TotalChildTime += elapsedTime;
	}
	else
	{
		// Root End
		mFrameEndTime = endTime;
	}

	mSamples[mLastOpenedSample].TotalTime += elapsedTime;
	mLastOpenedSample = mSamples[mLastOpenedSample].ParentSampleIndex;
	--mCallStackDepth;
}

void ProfilerManager::ResetProfiler( const char* name )
{

}

void ProfilerManager::ResetAll()
{
	
}

void ProfilerManager::Output()
{
	for (uint32_t i = 0; i < mCurrNumSamples; ++i)
	{

		char namebuf[256];

		for (uint32_t indent = 0; indent < mSamples[i].CallStackDepth; ++indent)
			namebuf[indent] = ' ';

		strcpy(namebuf + mSamples[i].CallStackDepth, mSamples[i].SampleName);
		printf("time=%f, childTime=%f, call=%d, %s\n", SystemClock::ToSeconds(mSamples[i].TotalTime), SystemClock::ToSeconds(mSamples[i].TotalChildTime), mSamples[i].CallCount, namebuf);

		//reset the sample for next time
		mSamples[i].CallCount = 0;
		mSamples[i].TotalTime = 0;
		mSamples[i].TotalChildTime = 0;
	}
}





}