#pragma once
#include <vector>
#include <thread>

class AutoSaver
{
public:
	//thread sleep interval in second
	static bool Deploy( uint32_t interval = 0 );
	uint32_t GetTotalAction();
	static void Destroy();
	static void Halt();
	static void Continue();
	//thread sleep interval in second
	static void SetInterval( uint32_t interval );

private:
	static void Routine();

	static bool isHalted;
	static uint32_t mTotalAction;
	static uint32_t mTimeInterval;
	static std::thread* mThread;
};