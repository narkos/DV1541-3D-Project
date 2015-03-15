#include "timer.h"

TimerClass::TimerClass()
{

}


TimerClass::TimerClass(const TimerClass& other)
{

}


TimerClass::~TimerClass()
{

}

//Den Initialize funktionen kommer först söka i systemet för att se om den stödjer högfrekventa timers . Om den returnerar en frekvens så använder vi det värdet för att bestämma hur 
//många ticks som kommer att inträffa varje millisekund . Vi kan sedan använda detta värde varje frame för att beräkna frametiden . I slutet av Initialize funktionen fråga vi 
//efter frametiden för denna frame för att starta tidtagningen .

bool TimerClass::Initialize()
{
	//Kollar så att systemet stödjer högfrekventa timers
		QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency);
	if (mFrequency == 0)
	{
		return false;
	}


	//Tar reda på hur många gånger frequency counter ticks varje millisekund .
	mTicksPerMs = (float)(mFrequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&mStartTime);

	return true;

}

//Frame funktionen kallas för varje enskild loop av exekvering av huvudprogrammet. 
//På så sätt kan vi beräkna skillnaden i tid mellan loops och bestämma tid det tog att utföra denna frame . 
//Vi hämtar, beräknar , och sedan lagrar tiden för denna frame i mFrameTime så att den kan användas av alla anropande objektet för synkronisering . 
//Vi lagrar sedan den aktuella tiden som början på nästa frame .

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	//läger in tiden i c
	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - mStartTime);

	mFrameTime = timeDifference / mTicksPerMs;

	mStartTime = currentTime;

	return;
}
//GetTime returns the most recent frame time that was calculated.

float TimerClass::GetTime()
{
	return mFrameTime;
}
