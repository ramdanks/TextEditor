#include "AutoSaver.h"
#include "LogGUI.h"
#include <sstream>
#include "TextField.h"

bool            AutoSaver::isHalted;
uint32_t        AutoSaver::mTotalAction;
uint32_t        AutoSaver::mTimeInterval;
std::thread*    AutoSaver::mThread;

void AutoSaver::Init( uint32_t interval )
{
	mTimeInterval = interval;
	isHalted = false;
}

bool AutoSaver::Deploy()
{
	if ( mTimeInterval == 0 ) mTimeInterval = 60;

	mThread = new std::thread( &Routine );
	if ( mThread == nullptr ) return false;

	std::ostringstream ss;
	ss << mThread->get_id();
	LOGALL( LEVEL_TRACE, "AutoSaver deploying a thread with ID: " + ss.str() );

	return true;
}


uint32_t AutoSaver::GetTotalAction()
{
	return mTotalAction;
}

void AutoSaver::Destroy()
{
	if ( mThread == nullptr ) return;
	delete mThread;
}

void AutoSaver::Continue()
{
	isHalted = false;
}

void AutoSaver::Halt()
{
	isHalted = true;
}

void AutoSaver::SetInterval( uint32_t interval )
{
	mTimeInterval = interval;
}

void AutoSaver::Routine()
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( mTimeInterval ) );
		if ( !AutoSaver::isHalted )
		{
			TextField::SaveTempAll();
			mTotalAction++;
			LOGCONSOLE( LEVEL_TRACE, "AutoSave performed! Action: " + std::to_string( mTotalAction ) );
		}
	}
}
