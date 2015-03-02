#include <windows.h>

class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();

	float GetTime();

private:
	INT64 mFrequency;
	float mTicksPerMs;
	INT64 mStartTime;
	float mFrameTime;
};
