#ifndef ATOMTRACECL_TIMER_H_
#define ATOMTRACECL_TIMER_H_

namespace AtomTraceCL
{
	namespace Timer
	{
		typedef __int64 Tick;

		void Init();

		const float TickToMS(const Tick i_tick);
		
		const Tick GetCurrentTick();

		unsigned int GetDifferentTick(const Tick i_start, const Tick i_end);
		const float GetDifferentTickToMS(const Tick i_start, const Tick i_end);
	}
	
}
#include "Timer.inl"

#endif //ATOMTRACECL_TIMER_H_

