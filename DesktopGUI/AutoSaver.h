#pragma once
#include <vector>
#include <thread>
#include "TextField.h"
#include <mutex>

class AutoSaver
{
public:
	//thread sleep interval in second
	AutoSaver( uint32_t interval = 0 );

	uint32_t GetTotalAction();
	bool Deploy();
	void Destroy();

	//thread sleep interval in second
	void SetInterval( uint32_t interval );

private:
	friend void Routine( AutoSaver* as );

	uint32_t mTotalAction;
	uint32_t mTimeInterval;
	std::thread* mThread;
	std::mutex* mMutex;
};