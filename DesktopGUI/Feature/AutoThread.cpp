#include "AutoThread.h"
#include "../Frame/DictionaryFrame.h"
#include "../Frame/ShareFrame.h"
#include "../TextField.h"
#include "LogGUI.h"

AutoThread::sThread AutoThread::mHighlighter;
AutoThread::sThread AutoThread::mSaver;
AutoThread::sThread AutoThread::mSocket;

bool AutoThread::DeployAutoSave( uint32_t interval )
{
	return DeployThread( THREAD_SAVER, interval );
}

bool AutoThread::DeployAutoHighlight( uint32_t interval )
{
	return DeployThread( THREAD_HIGHLIGHTER, interval );
}

bool AutoThread::DeployAutoConnect( uint32_t interval )
{
	return DeployThread( THREAD_SOCKET, interval );
}

void AutoThread::Destroy( int thread )
{
	switch ( thread )
	{
	case THREAD_SAVER:       mSaver.isJoin       = true;   break;
	case THREAD_HIGHLIGHTER: mHighlighter.isJoin = true;   break;
	case THREAD_SOCKET:      mSocket.isJoin      = true;   break;
	case THREAD_ALL:         mSaver.isJoin       = true;		  
		                     mHighlighter.isJoin = true;  
		                     mSocket.isJoin      = true;   break;                     
	default: break;
	}
}

void AutoThread::RoutineAutoSave()
{
	TextField::SaveTempAll();
	LOG_THREAD( LV_TRACE, "AutoThread Saving all temporary files" );
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
		LOG_THREAD_FORMAT( LV_TRACE, "AutoThread Highlighting at page: %d", page );
	}
}

void AutoThread::RoutineAutoConnect()
{
	static bool before = false;
	bool serverlisten = !ShareFrame::isListenOver;
	bool status = ShareFrame::mSock->IsConnected();

	if ( status != ShareFrame::mSock->IsOK() )
		LOG_DEBUG( LV_WARN, "Connected but Not OK!" );

	if ( status != before )
	{
		before = status;
		ShareFrame::UpdateInterface( status );
		auto peerinfo = ShareFrame::mSock->GetSocketPeerInfo();
		if ( status )
			LOG_DEBUG( LV_INFO, "Connected to Host: " + std::string( peerinfo.Address ) );		
		else if ( !status )
			LOG_DEBUG( LV_INFO, "Disconnected from Host: " + std::string( peerinfo.Address ) );		
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
	if ( interval == 0 ) interval = 3600;
	try
	{
		if ( thread == THREAD_SAVER )
		{
			if ( mSaver.pThread != nullptr ) throw;
			mSaver.pThread = new std::thread( &ReleaseThread, &RoutineAutoSave, &mSaver.isJoin, interval );
		}
		else if ( thread == THREAD_HIGHLIGHTER )
		{
			if ( mHighlighter.pThread != nullptr ) throw;
			mHighlighter.pThread = new std::thread( &ReleaseThread, &RoutineAutoHighlight, &mHighlighter.isJoin, interval );
		}
		else if ( thread == THREAD_SOCKET )
		{
			if ( mSocket.pThread != nullptr ) throw;
			mSocket.pThread = new std::thread( &ReleaseThread, &RoutineAutoConnect, &mSocket.isJoin, interval );
		}
	}
	catch ( ... )
	{
		LOG_ALL( LV_WARN, "Cannot deploy a thread" );
		return false;
	}
}

void AutoThread::ReleaseThread( void (*func)(void), const bool* join, uint32_t interval )
{
	//never ending loop
	while ( true )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( interval ) );
		if ( *join ) break;
		func();	
	}
}