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
		bool isPause = false;
	};

public:
	static bool DeployThread( int thread );
	static bool DeployAutoSave( uint32_t interval = 0 );
	static bool DeployAutoHighlight( uint32_t interval = 0 );
	static bool DeployAutoConnect( uint32_t interval = 0 );

	static bool isDeploy( int thread );

	static void Continue( int thread );
	static void Pause( int thread );
	static void Destroy( int thread );

	static void RoutineAutoSave();
	static void RoutineAutoHighlight();
	static void RoutineAutoConnect();

private:
	static sThread mHighlighter;
	static sThread mSaver;
	static sThread mSocket;
};