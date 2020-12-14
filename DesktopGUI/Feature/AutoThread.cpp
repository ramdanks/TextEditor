#include "AutoThread.h"
#include "../Frame/DictionaryFrame.h"
#include "../Frame/ShareFrame.h"
#include "../TextField.h"
#include "LogGUI.h"
#include "Config.h"

AutoThread::sThread AutoThread::mHighlighter;
AutoThread::sThread AutoThread::mSaver;
AutoThread::sThread AutoThread::mSocket;

bool AutoThread::DeployThread( int thread )
{
	if ( thread == THREAD_SAVER )
	{
		if ( mSaver.pThread != nullptr ) return false;
		mSaver.pThread = new std::thread( RoutineAutoSave );
		mSaver.pThread->detach();
	}
	else if ( thread == THREAD_HIGHLIGHTER )
	{
		if ( mHighlighter.pThread != nullptr ) return false;
		mHighlighter.pThread = new std::thread( RoutineAutoHighlight );
		mHighlighter.pThread->detach();
	}
	else if ( thread == THREAD_SOCKET )
	{
		if ( mSocket.pThread != nullptr ) return false;
		mSocket.pThread = new std::thread( RoutineAutoConnect );
	}
	return true;
}

bool AutoThread::DeployAutoSave( uint32_t interval )
{
	return DeployThread( THREAD_SAVER );
}

bool AutoThread::DeployAutoHighlight( uint32_t interval )
{
	return DeployThread( THREAD_HIGHLIGHTER );
}

bool AutoThread::DeployAutoConnect( uint32_t interval )
{
	return DeployThread( THREAD_SOCKET );
}

bool AutoThread::isDeploy( int thread )
{
	switch ( thread )
	{
	case THREAD_SAVER:       return mSaver.pThread != nullptr;
	case THREAD_HIGHLIGHTER: return mHighlighter.pThread != nullptr;
	case THREAD_SOCKET:      return mSocket.pThread != nullptr;
	default: break;
	}
}

void AutoThread::Continue( int thread )
{
	switch ( thread )
	{
	case THREAD_SAVER:       mSaver.isPause       = false;   break;
	case THREAD_HIGHLIGHTER: mHighlighter.isPause = false;   break;
	case THREAD_SOCKET:      mSocket.isPause      = false;   break;
	case THREAD_ALL:         mSaver.isPause       = false;
		                     mHighlighter.isPause = false;
		                     mSocket.isPause      = false;   break;
	default: break;
	}
}

void AutoThread::Pause( int thread )
{
	switch ( thread )
	{
	case THREAD_SAVER:       mSaver.isPause        = true;   break;
	case THREAD_HIGHLIGHTER: mHighlighter.isPause  = true;   break;
	case THREAD_SOCKET:      mSocket.isPause       = true;   break;
	case THREAD_ALL:         mSaver.isPause        = true;
		                     mHighlighter.isPause  = true;
		                     mSocket.isPause       = true;   break;
	default: break;
	}
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
	mSaver.isJoin = false;
	while ( !mSaver.isJoin )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( Config::sAutosave.Param ) );
		if ( mSaver.isJoin ) break;
		if ( mSaver.isPause ) continue;

		TextField::SaveTempAll();
		LOG_THREAD( LV_TRACE, "AutoThread Saving all temporary files" );

	}
}

void AutoThread::RoutineAutoHighlight()
{
	mHighlighter.isJoin = false;
	while ( !mHighlighter.isJoin )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( Config::sAutohigh.Param ) );
		if ( mHighlighter.isJoin ) break;
		if ( mHighlighter.isPause ) continue;

		static size_t hashBefore = 0;
		static std::hash<wxString> hash;

		auto page = TextField::GetActivePage();
		if ( page == wxNOT_FOUND ) continue;

		auto textPath = TextField::mPageData[page].FilePath;
		if ( !DictionaryFrame::ExistList( textPath ) ) continue;

		auto text = TextField::mPageData[page].TextField->GetText().mbc_str();
		size_t hashNow = hash( text );
		if ( hashNow != hashBefore )
		{
			hashBefore = hashNow;
			if ( !DictionaryFrame::StartStyling( textPath, Config::GetDictionaryFlags() ) ) continue;
			LOG_THREAD_FORMAT( LV_TRACE, "AutoThread Highlighting at page: %d", page );
		}	
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