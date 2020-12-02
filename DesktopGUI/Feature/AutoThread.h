#pragma once
#include <thread>

#define AUTOSAVE      1
#define AUTOHIGHLIGHT 2
#define AUTOCONNECT   3

class AutoThread
{
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
	static void ReleaseThread( void (*func)(void), uint32_t interval );

	static std::thread* mThreadAC;
	static std::thread* mThreadAS;
	static std::thread* mThreadAH;
};