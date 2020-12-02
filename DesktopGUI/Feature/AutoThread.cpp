#include "AutoThread.h"
#include "LogGUI.h"
#include "../../Utilities/Timer.h"
#include "../Frame/DictionaryFrame.h"
#include "../Frame/ShareFrame.h"
#include "../TextField.h"
#include <functional>

std::thread* AutoThread::mThreadAC;
std::thread* AutoThread::mThreadAS;
std::thread* AutoThread::mThreadAH;

bool AutoThread::DeployAutoSave( uint32_t interval )
{
	return DeployThread( AUTOSAVE, interval );
}

bool AutoThread::DeployAutoHighlight( uint32_t interval )
{
	return DeployThread( AUTOHIGHLIGHT, interval );
}

bool AutoThread::DeployAutoConnect( uint32_t interval )
{
	return DeployThread( AUTOCONNECT, interval );
}

void AutoThread::Destroy( int thread )
{
	if ( thread == AUTOSAVE )
		if ( mThreadAS != nullptr ) delete mThreadAS;
	if ( thread == AUTOHIGHLIGHT )
		if ( mThreadAH != nullptr ) delete mThreadAH;
}

void AutoThread::RoutineAutoSave()
{
	TextField::SaveTempAll();
	LOG_THREAD( LEVEL_TRACE, "AutoThread Saving all temporary files" );
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
		LOG_THREAD_FORMAT( LEVEL_TRACE, "AutoThread Highlighting at page: %d", page );
	}
}

void AutoThread::RoutineAutoConnect()
{
	static bool before = false;
	bool serverlisten = !ShareFrame::isListenOver;
	bool status = ShareFrame::mSock->IsConnected();

	if ( status != ShareFrame::mSock->IsOK() )
		LOG_DEBUG( LEVEL_WARN, "Connected but Not OK!" );

	if ( status != before )
	{
		before = status;
		ShareFrame::UpdateInterface( status );
		auto peerinfo = ShareFrame::mSock->GetSocketPeerInfo();
		if ( status )
		{
			LOG_DEBUG( LEVEL_INFO, "Connected to Host: " + std::string( peerinfo.Address ) );
		}
		else if ( !status )
		{	
			LOG_DEBUG( LEVEL_INFO, "Disconnected from Host: " + std::string( peerinfo.Address ) );
		}
	}
	// if not connected and listening session is over, listen again.
	if ( !serverlisten && !status )
	{
		ShareFrame::mListenThread->join();
		delete ShareFrame::mListenThread;
		ShareFrame::mListenThread = new std::thread( &ShareFrame::ListenDetach );
	}
}

bool AutoThread::DeployThread( int thread, uint32_t interval )
{
	try
	{
		if ( interval == 0 ) interval = 3600;
		if ( thread == AUTOSAVE )
		{
			if ( mThreadAS != nullptr ) throw;
			mThreadAS = new std::thread( &ReleaseThread, &RoutineAutoSave, interval );
		}
		else if ( thread == AUTOHIGHLIGHT )
		{
			if ( mThreadAH != nullptr ) throw;
			mThreadAH = new std::thread( &ReleaseThread, &RoutineAutoHighlight, interval );
		}
		else if ( thread == AUTOCONNECT )
		{
			if ( mThreadAC != nullptr ) throw;
			mThreadAC = new std::thread( &ReleaseThread, &RoutineAutoConnect, interval );
		}
		else
		{
			LOG_ALL( LEVEL_WARN, "Unrecognized thread deploy" );
		}
	}
	catch ( ... )
	{
		LOG_ALL( LEVEL_ERROR, "Cannot create a thread" );
	}
	return mThreadAS != nullptr;
}

void AutoThread::ReleaseThread( void (*func)(void), uint32_t interval )
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( interval ) );
		func();	
	}
}