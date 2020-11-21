#pragma once
#include <thread>

#define AUTOSAVE      1
#define AUTOHIGHLIGHT 2

struct ThreadAction
{
	bool IsHalted;
	uint32_t TotalAction, Interval;
};

class AutoThread
{
public:
	static bool DeployAutoSave( uint32_t interval = 0 );
	static bool DeployAutoHighlight( uint32_t interval = 0 );

	static void Destroy( int thread );
	static void Halt( int thread );
	static void Continue( int thread );
	static void SetInterval( int thread, uint32_t interval ); // in seconds
	static uint32_t GetTotalAction( int thread );

	static void RoutineAutoSave();
	static void RoutineAutoHighlight();

private:
	static void ReleaseThread( void (*func)(void), ThreadAction* action );

	static ThreadAction mActionAS;
	static ThreadAction mActionAH;
	static std::thread* mThreadAS;
	static std::thread* mThreadAH;
};