#pragma once

#define THREAD_ALL         0
#define THREAD_SAVER       1
#define THREAD_HIGHLIGHTER 2
#define THREAD_SOCKET      3

class AutoThread
{
	struct sThread
	{
		std::thread* pThread = nullptr;
		bool isJoin = false;
	};

public:
	static bool DeployAutoSave( uint32_t interval = 0 );
	static bool DeployAutoHighlight( uint32_t interval = 0 );
	static bool DeployAutoConnect( uint32_t interval = 0 );

	static void Destroy( int thread );

	static void RoutineAutoSave();
	static void RoutineAutoHighlight();
	static void RoutineAutoConnect();

private:
	static bool DeployThread( int thread, uint32_t interval );
	static void ReleaseThread( void (*func)(void), const bool* join, uint32_t interval );

	static sThread mHighlighter;
	static sThread mSaver;
	static sThread mSocket;
};