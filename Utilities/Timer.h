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
	public:
		Timer();
		Timer( TimerPoint TP, bool PrintOnDestroy );
		Timer( const std::string& Title, TimerPoint TP, bool PrintOnDestroy );
		Timer( std::string&& Title, TimerPoint TP, bool PrintOnDestroy );
		~Timer();

		void Setting( std::string&& Title, TimerPoint TP, bool PrintOnDestroy );
		void Setting( const std::string& Title, TimerPoint TP, bool PrintOnDestroy );

		void Tic();
		float Toc();
		std::string Toc_String();
		static float Adjust_Time( TimerPoint TP, float Sec );
		static std::string Get_Current_Time();

	private:
		std::string mTitle;
		TimerPoint mTime;
		bool mPrintOnDestroy;
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

#define TIMER_MACROS 1
#if TIMER_MACROS
	#define TIMER_ONLY( OBJ,TP,POD )       Util::Timer OBJ(TP,POD)
	#define TIMER_SCOPE( OBJ,STR,TP,POD )  Util::Timer OBJ(STR,TP,POD)
	#define TIMER_FUNCTION( OBJ,TP,POD )   TIMER_SCOPE(OBJ,__FUNCSIG__,TP,POD)
	#define TIMER_GET( OBJ )               OBJ.Toc()
	#define TIMER_GETSTR( OBJ )            OBJ.Toc_String()
#else
	#define TIMER_ONLY( OBJ,TP,POD )
	#define TIMER_SCOPE( OBJ,STR,TP,POD )
	#define TIMER_FUNCTION( OBJ,TP,POD )
	#define TIMER_GET( OBJ )       
	#define TIMER_GETSTR( OBJ )
#endif