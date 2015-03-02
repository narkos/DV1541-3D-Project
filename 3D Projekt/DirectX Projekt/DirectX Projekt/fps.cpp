#include "fps.h"


FpsClass::FpsClass()
{

}

FpsClass::FpsClass(const FpsClass&)
{

}


FpsClass::~FpsClass()
{

}

//The Initialize funktionen sätter alla counters till 0 coh startar tiden. 
void FpsClass::Initialize()
{
	mFps = 0;
	mCount = 0;
	mStartTime = timeGetTime();
	return;
}

//Frame funktionen måste bli kallad varje frame så att den kan öka frame count med 1.Om kommissionen finner att en sekund har gått så kommer det att lagra frame count i m_fps variabeln. Den återställer sedan räkningen och startar timern igen.
void FpsClass::Frame()
{
	mCount++;

	if (timeGetTime() >= (mStartTime + 1000))
	{
		mFps = mCount;
		mCount = 0;

		mStartTime = timeGetTime();
	}

}
//GetFps returnerar frame per sekund hastighet för den sista sekund som just passerat .
int FpsClass::GetFps()
{
	return mFps;
}
