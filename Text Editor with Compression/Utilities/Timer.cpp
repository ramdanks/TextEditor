#include "Timer.h"
#include <iostream>
#include <direct.h>
#include <fstream>

Timer::Timer()
	:
	mTitle(""),
	mTime(SEC),
	mPrintOnDestroy(false)
{}

Timer::Timer( std::string Title, TimerPoint TP, bool PrintOnDestroy )
{
	this->Setting( Title, TP, PrintOnDestroy );
	Tic();
}

Timer::~Timer()
{
	if ( mPrintOnDestroy )
	{
		float res = Toc();
		std::string spec;
		if ( mTime == ADJUST )
		{
			if ( res > MIN )
			{
				res = Adjust_Time( MIN, res );
				spec = "(min)";
			}
			else if ( res > SEC )
			{
				res = Adjust_Time( SEC, res );
				spec = "(sec)";
			}
			else if ( res * MS >= 1.0f )
			{
				res = Adjust_Time( MS, res );
				spec = "(ms)";
			}
			else if ( res * US >= 1.0f )
			{
				res = Adjust_Time( US, res );
				spec = "(us)";
			}
			else
			{
				res = Adjust_Time( NS, res );
				spec = "(ns)";
			}
		}
		std::cout << mTitle << " Time:" << res << spec << std::endl;
	}
}

void Timer::Tic()
{
	TimeLog = std::chrono::high_resolution_clock::now();
}

float Timer::Toc()
{
	Duration = std::chrono::high_resolution_clock::now() - TimeLog;
	return Adjust_Time( this->mTime, Duration.count() );
}

void Timer::Setting( std::string Title, TimerPoint TP, bool PrintOnDestroy )
{
	this->mTitle = Title;
	this->mTime = TP;
	this->mPrintOnDestroy = PrintOnDestroy;
}

float Timer::Adjust_Time( TimerPoint TP, float Sec )
{
	switch ( TP )
	{
		case MIN:	return Sec / MIN;
		case SEC:	return Sec;
		case MS:	return Sec * MS;
		case US:	return Sec * US;
		case NS:	return Sec * NS;
		default:	return Sec;
	}
}

std::string Timer::Get_Current_Time()
{
	time_t now = time( 0 );
	tm tstruct;
	char buf[64];
	tstruct = *localtime( &now );
	strftime( buf, sizeof( buf ), "%Y-%m-%d %X", &tstruct );
	return std::string( buf );
}

StatTimer::StatTimer()
	:
	mLow( 0.0f ),
	mAvg( 0.0f ),
	mHigh( 0.0f ),
	mIteration( 0u )
{
	this->mTimer.Tic();
}

void StatTimer::Start()
{
	this->mTimer.Tic();
}

void StatTimer::Refresh()
{
	float res = this->mTimer.Toc();
	if ( mIteration == 0u )
	{
		mHigh = res;
		mLow = res;
		mAvg = res;
	}
	else
	{
		if ( res > mHigh ) mHigh = res;
		else if ( res < mLow ) mLow = res;
		mAvg = (mAvg * mIteration + res) / (mIteration + 1);
	}
	mIteration++;
	this->mTimer.Tic();
}

void StatTimer::Print_Result( TimerPoint TP )
{
	std::cout << "Low:" << Timer::Adjust_Time( TP, mLow ) << std::endl;
	std::cout << "Avg:" << Timer::Adjust_Time( TP, mAvg ) << std::endl;
	std::cout << "High:" << Timer::Adjust_Time( TP, mHigh ) << std::endl;
}

float StatTimer::Get_Low() const
{
	return mLow;
}

float StatTimer::Get_Avg() const
{
	return mAvg;
}

float StatTimer::Get_High() const
{
	return mHigh;
}