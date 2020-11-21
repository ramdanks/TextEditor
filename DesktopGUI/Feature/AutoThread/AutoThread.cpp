#include "AutoThread.h"
#include ""
ThreadAction AutoThread::mActionAS;
ThreadAction AutoThread::mActionAH;
std::thread* AutoThread::mThreadAS;
std::thread* AutoThread::mThreadAH;

bool AutoThread::DeployAutoSave( uint32_t interval )
{
	if ( mThreadAS != nullptr ) return false;
	if ( mActionAS.Interval == 0 ) mActionAS.Interval = 60;

	mThreadAS = new std::thread( &ReleaseThread, &RoutineAutoSave, &mActionAS );
	return mThreadAS != nullptr;
}

bool AutoThread::DeployAutoHighlight( uint32_t interval )
{
	if ( mThreadAH != nullptr ) return false;
	if ( mActionAH.Interval == 0 ) mActionAH.Interval = 60;

	mThreadAS = new std::thread( &ReleaseThread, &RoutineAutoHighlight, &mActionAH );
	return mThreadAS != nullptr;
}

void AutoThread::Destroy( int thread )
{
	if ( thread == AUTOSAVE )
		if ( mThreadAS != nullptr ) delete mThreadAS;
	if ( thread == AUTOHIGHLIGHT )
		if ( mThreadAH != nullptr ) delete mThreadAH;
}

void AutoThread::Halt( int thread )
{
	if ( thread == AUTOSAVE ) mActionAS.IsHalted = true;
	if ( thread == AUTOHIGHLIGHT ) mActionAH.IsHalted = true;
}

void AutoThread::Continue( int thread )
{
	if ( thread == AUTOSAVE ) mActionAS.IsHalted = false;
	if ( thread == AUTOHIGHLIGHT ) mActionAH.IsHalted = false;
}

void AutoThread::SetInterval( int thread, uint32_t interval )
{
	if ( thread == AUTOSAVE ) mActionAS.Interval = false;
	if ( thread == AUTOHIGHLIGHT ) mActionAH.IsHalted = false;
}

uint32_t AutoThread::GetTotalAction( int thread )
{
	if ( thread == AUTOSAVE ) return mActionAS.TotalAction;
	if ( thread == AUTOHIGHLIGHT ) return mActionAH.TotalAction;
}

void AutoThread::RoutineAutoSave()
{
}

void AutoThread::RoutineAutoHighlight()
{
	DictionaryFrame::
}

void AutoThread::ReleaseThread( void (*func)(void), ThreadAction* action )
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( action->Interval ) );
		if ( !action->IsHalted )
		{
			func();
			action->TotalAction++;
		}
	}
}