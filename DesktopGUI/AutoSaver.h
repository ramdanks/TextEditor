#pragma once
#include <vector>
#include <thread>

class AutoSaver
{
public:
	//thread sleep interval in second
	AutoSaver();

	uint32_t GetTotalAction();
	bool Deploy( uint32_t interval = 0 );
	void Destroy();

	//thread sleep interval in second
	void SetInterval( uint32_t interval );

private:
	friend void Routine( AutoSaver* as );

	uint32_t mTotalAction;
	uint32_t mTimeInterval;
	std::thread* mThread;
};