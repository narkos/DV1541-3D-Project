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

//Den Initialize funktionen kommer f�rst s�ka i systemet f�r att se om den st�djer h�gfrekventa timers . Om den returnerar en frekvens s� anv�nder vi det v�rdet f�r att best�mma hur 
//m�nga ticks som kommer att intr�ffa varje millisekund . Vi kan sedan anv�nda detta v�rde varje frame f�r att ber�kna frametiden . I slutet av Initialize funktionen fr�ga vi 
//efter frametiden f�r denna frame f�r att starta tidtagningen .

bool TimerClass::Initialize()
{
	//Kollar s� att systemet st�djer h�gfrekventa timers
		QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency);
	if (mFrequency == 0)
	{
		return false;
	}


	//Tar reda p� hur m�nga g�nger frequency counter ticks varje millisekund .
	mTicksPerMs = (float)(mFrequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&mStartTime);

	return true;

}

//Frame funktionen kallas f�r varje enskild loop av exekvering av huvudprogrammet. 
//P� s� s�tt kan vi ber�kna skillnaden i tid mellan loops och best�mma tid det tog att utf�ra denna frame . 
//Vi h�mtar, ber�knar , och sedan lagrar tiden f�r denna frame i mFrameTime s� att den kan anv�ndas av alla anropande objektet f�r synkronisering . 
//Vi lagrar sedan den aktuella tiden som b�rjan p� n�sta frame .

void TimerClass::Frame()
{
	INT64 currentTime;
	float timeDifference;

	//l�ger in tiden i c
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
