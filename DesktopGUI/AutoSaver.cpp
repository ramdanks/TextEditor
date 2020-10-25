#include "AutoSaver.h"
#include "LogGUI.h"
#include <sstream>

void Routine( AutoSaver* as );

AutoSaver::AutoSaver( uint32_t interval )
{
	mMutex = new std::mutex;
	mTimeInterval = interval;
}

bool AutoSaver::Deploy()
{
	if ( mTimeInterval == 0 )
		mTimeInterval = 60;

	mThread = new std::thread( &Routine, this );

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

void AutoSaver::SetInterval( uint32_t interval )
{
	mTimeInterval = interval;
}

void Routine( AutoSaver* as )
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::seconds( as->mTimeInterval ) );
		TextField::SaveTempAll();
		as->mTotalAction++;
		LOGCONSOLE( LEVEL_TRACE, "AutoSave performed! Action: " + std::to_string( as->mTotalAction ) );
	}
}