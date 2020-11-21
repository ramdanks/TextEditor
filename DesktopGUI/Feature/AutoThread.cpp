#include "AutoThread.h"
#include "LogGUI.h"
#include "../../Utilities/Timer.h"
#include "../Frame/DictionaryFrame.h"
#include "../TextField.h"
#include <functional>

ThreadAction AutoThread::mActionAS;
ThreadAction AutoThread::mActionAH;
std::thread* AutoThread::mThreadAS;
std::thread* AutoThread::mThreadAH;

bool AutoThread::DeployAutoSave( uint32_t interval )
{
	if ( mThreadAS != nullptr ) return false;
	if ( interval == 0 ) interval = 3600;
	mActionAS.Interval = interval;

	mThreadAS = new std::thread( &ReleaseThread, &RoutineAutoSave, &mActionAS );
	return mThreadAS != nullptr;
}

bool AutoThread::DeployAutoHighlight( uint32_t interval )
{
	if ( mThreadAH != nullptr ) return false;
	if ( interval == 0 ) interval = 3600;
	mActionAH.Interval = interval;

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
	TextField::SaveTempAll();
	LOG_CONSOLE_FORMAT( LEVEL_TRACE, "AutoThread Saver actions(%u)", mActionAS.TotalAction );
}

void AutoThread::RoutineAutoHighlight()
{
	static size_t hashBefore = 0;
	static std::hash<wxString> hash;

	auto page = TextField::GetActivePage();
	if ( page == wxNOT_FOUND ) return;

	auto textPath = TextField::mPageData[page].FilePath;
	if ( !DictionaryFrame::ExistList( textPath ) ) return;

	auto text = TextField::mPageData[page].TextField->GetText().mbc_str();
	if ( hash( text ) != hashBefore )
	{
		hashBefore = hash( text );
		if ( !DictionaryFrame::StartStyling( textPath ) ) return;
		LOG_CONSOLE_FORMAT( LEVEL_TRACE, "AutoThread Highlighting at page(%d), actions(%u)", page, mActionAH.TotalAction );
	}
}

void AutoThread::ReleaseThread( void (*func)(void), ThreadAction* action )
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( action->Interval ) );
		if ( !action->IsHalted )
		{
			action->TotalAction++;
			func();
		}
	}
}