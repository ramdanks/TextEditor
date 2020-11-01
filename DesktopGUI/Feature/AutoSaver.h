#pragma once
#include <vector>
#include <thread>

class AutoSaver
{
public:
	static void Init( uint32_t interval );
	uint32_t GetTotalAction();
	//thread sleep in second
	static bool Deploy();
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