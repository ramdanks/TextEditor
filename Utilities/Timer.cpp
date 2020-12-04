#include "Timer.h"
#include <iostream>
#include <direct.h>
#include <fstream>

namespace Util
{
	Timer::Timer()
		: mTime(SEC), mPrintOnDestroy(false)
	{
		Tic();
	}

	Timer::Timer( TimerPoint TP, bool PrintOnDestroy )
		: mTime(TP), mPrintOnDestroy(PrintOnDestroy)
	{
		Tic();
	}

	Timer::Timer( const std::string& Title, TimerPoint TP, bool PrintOnDestroy )
		: mTitle(Title), mTime(TP), mPrintOnDestroy(PrintOnDestroy)
	{
		Tic();
	}

	Timer::Timer( std::string&& Title, TimerPoint TP, bool PrintOnDestroy )
		: mTitle( std::move( Title ) ), mTime( TP ), mPrintOnDestroy( PrintOnDestroy )
	{
		Tic();
	}

	Timer::~Timer()
	{
		if ( mPrintOnDestroy )
			std::cout << Toc_String().c_str() << std::endl;	
	}

	void Timer::Setting( std::string&& Title, TimerPoint TP, bool PrintOnDestroy )
	{
		mTitle = std::move( Title );
		mTime = TP;
		mPrintOnDestroy = PrintOnDestroy;
	}

	void Timer::Setting( const std::string& Title, TimerPoint TP, bool PrintOnDestroy )
	{
		mTitle = Title;
		mTime = TP;
		mPrintOnDestroy = PrintOnDestroy;
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

	std::string Timer::Toc_String()
	{
		auto time = Toc();

		TimerPoint tp = mTime;
		if ( tp == ADJUST )
		{
			if      ( time > MIN )        tp = MIN;
			else if ( time > SEC )        tp = SEC;
			else if ( time * MS >= 1.0f ) tp = MS;
			else if ( time * US >= 1.0f ) tp = US;
			else                          tp = NS;
			time = Adjust_Time( tp, time );
		}

		// indicator
		std::string spec;
		if      ( tp == MIN ) spec = " (min)";
		else if ( tp == SEC ) spec = " (sec)";
		else if ( tp == MS )  spec = " (ms)";
		else if ( tp == US )  spec = " (us)";
		else if ( tp == NS )  spec = " (ns)";

		return mTitle + " Time: " + std::to_string( time ) + spec;
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
		strftime( buf, sizeof( buf ), "%d-%m-%Y %X", &tstruct );
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
}