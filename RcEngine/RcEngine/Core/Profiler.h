#ifndef Profiler_h__
#define Profiler_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine {

#define INVALID_TIME	((uint64_t)(-1))
#define MAX_PROFILERS   (200)

struct _ApiExport ProfilerSample
{
	const char* SampleName;
	
	size_t SampleIndex;
	size_t ParentSampleIndex;

	uint64_t StartTime;
	uint64_t TotalTime;
	uint64_t TotalChildTime;

	uint32_t CallCount;
	uint32_t CallStackDepth;

	ProfilerSample() : TotalTime(0), CallCount(0) {}
};

class _ApiExport ProfilerManager : public Singleton<ProfilerManager>
{
public:
	ProfilerManager();
	~ProfilerManager();

public:
	void ProfilerStart(const char* name);
	void ProfilerEnd();

	void ResetProfiler(const char* name);
	void ResetAll();

	void Output();

private:
	ProfilerSample mSamples[MAX_PROFILERS];

	size_t mLastOpenedSample;

	uint32_t mCurrNumSamples;
	uint32_t mCallStackDepth;

	uint64_t mFrameBeginTime;
	uint64_t mFrameEndTime;

	volatile uint32_t mCurrFrame;		// Global frame counter

	friend struct CpuAutoProfiler;
	friend struct GpuAutoProfiler;
};

struct _ApiExport CpuAutoProfiler
{
	CpuAutoProfiler(const char* name);
	~CpuAutoProfiler();
};

#define ENGINE_CPU_AUTO_PROFIER(name) CpuAutoProfiler _cpu_profiler(name);
#define ENGINE_PUSH_CPU_PROFIER(name) ProfilerManager::GetSingleton().ProfilerStart(name)
#define ENGINE_POP_CPU_PROFIER(name)  ProfilerManager::GetSingleton().ProfilerEnd()
#define ENGINE_DUMP_PROFILERS()		  ProfilerManager::GetSingleton().Output()
}


#endif // Profiler_h__
