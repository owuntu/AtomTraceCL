#if defined (_WIN32) | defined (_WIN64)
#include "Timer.h"
#include <Windows.h>
#include <assert.h>

namespace AtomTraceCL
{
	namespace Timer
	{
		LARGE_INTEGER s_Freq = { 0LL };

		void Init()
		{
			if (s_Freq.QuadPart == 0LL){
				BOOL bSucess = QueryPerformanceFrequency(&s_Freq);
				assert(bSucess == TRUE);
			}
		}

		const float TickToMS(const Tick i_tick)
		{
			if(s_Freq.QuadPart == 0LL)
				Init();
			float diffTick = static_cast<float>(i_tick * 1000);
			float freq = static_cast<float>(s_Freq.QuadPart);
			return  static_cast<float>(diffTick/s_Freq.QuadPart);
		}
	}
}

#endif // defined (_WIN32) | defined (_WIN64)
