#include "Timer.h"
#include "JobDispatcher.h"




Timer::Timer()
{
	mBeginTickPoint = Clock::now();
	LTickCount = GetCurrentTick();
}

int64_t Timer::GetCurrentTick()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - mBeginTickPoint).count();
}

void Timer::PushTimerJob(AsyncExecutablePtr obj, uint32_t after, JobEntry* task)
{
	int64_t dueTimeTick = after + LTickCount;

	mTimerJobQueue.push(TimerJobElement(obj, task, dueTimeTick));
}


void Timer::DoTimerJob()
{
	/// thread tick update
	LTickCount = GetCurrentTick();

	while ( !mTimerJobQueue.empty() )
	{
		const TimerJobElement& timerJobElem = mTimerJobQueue.top();

		if (LTickCount < timerJobElem.mExecutionTick )
			break ;

		timerJobElem.mOwner->DoTask(timerJobElem.mTask); ///< pass to the dispatcher

		mTimerJobQueue.pop();
	}

	/// optional: for the low cpu-utilization
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

