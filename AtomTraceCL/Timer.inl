#if defined (_WIN32) | defined (_WIN64)
#include <Windows.h>
#include <assert.h>

namespace AtomTraceCL
{
	namespace Timer
	{
		inline const Tick GetCurrentTick()
		{
			LARGE_INTEGER count;

			BOOL bSucees = QueryPerformanceCounter(&count);
			assert(bSucees == TRUE);

			return count.QuadPart;
		}

		inline unsigned int GetDifferentTick(const Tick i_start, const Tick i_end)
		{
			return static_cast<unsigned int>(i_end - i_start);
		}

		inline const float GetDifferentTickToMS(const Tick i_start, const Tick i_end)
		{
			Tick diffTick = i_end - i_start;
			return TickToMS(diffTick);
		}
	}
}

#endif //defined (_WIN32) | defined (_WIN64)