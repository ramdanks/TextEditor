#pragma once
#include <chrono>
#include <string>

enum TimerPoint
{ 
	ADJUST,
	MIN		= 60,
	SEC		= 1,
	MS		= 1000,
	US		= 1000000,
	NS		= 1000000000
};

namespace Util
{
	class Timer
	{
		std::string mTitle;
		TimerPoint mTime;
		bool mPrintOnDestroy;

	public:
		Timer();
		Timer( std::string Title, TimerPoint TP, bool PrintOnDestroy );
		~Timer();


		void Tic();
		float Toc();
		void Setting( std::string Title, TimerPoint TP, bool PrintOnDestroy );
		static float Adjust_Time( TimerPoint TP, float Sec );
		static std::string Get_Current_Time();

	private:
		std::chrono::time_point<std::chrono::steady_clock> TimeLog;
		std::chrono::duration<float> Duration;
	};

	class StatTimer
	{
		Timer mTimer;
		float mLow, mAvg, mHigh;
		long long mIteration;

	public:
		StatTimer();
		void Start();
		void Refresh();
		void Print_Result( TimerPoint TP );

		float Get_Low() const;
		float Get_Avg() const;
		float Get_High() const;
	};
}