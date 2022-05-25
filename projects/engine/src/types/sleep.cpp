#include <alte/types/sleep.hpp>
#include <alte/types/time.hpp>

#ifdef _WIN32
  #define NOMINMAX
  #define WIN32_LEAN_AND_MEAN
  #define _WIN32_WINDOWS 0x0501
  #define _WIN32_WINNT 0x0501
  #define WINVER 0x0501
  #include <windows.h>

  #include <mmsystem.h>

#else

  #include <cerrno>
  #include <ctime>
#endif

namespace AltE {

#ifdef _WIN32
  void sleepImpl(Time time) {
    // Get the minimum supported timer resolution on this system
    static const UINT periodMin = [] {
      TIMECAPS tc;
      timeGetDevCaps(&tc, sizeof(TIMECAPS));
      return tc.wPeriodMin;
    }();

    // Set the timer resolution to the minimum for the Sleep call
    timeBeginPeriod(periodMin);

    // Wait...
    ::Sleep(static_cast<DWORD>(time.as_millis()));

    // Reset the timer resolution back to the system default
    timeEndPeriod(periodMin);
  }
#else

  void sleepImpl(Time time) {
    const uint64_t usecs = time.as_micros();

    // Construct the time to wait
    timespec ti;
    ti.tv_sec = static_cast<time_t>(usecs / 1000000);
    ti.tv_nsec = static_cast<long>((usecs % 1000000) * 1000);

    // Wait...
    // If nanosleep returns -1, we check errno. If it is EINTR
    // nanosleep was interrupted and has set ti to the remaining
    // duration. We continue sleeping until the complete duration
    // has passed. We stop sleeping if it was due to an error.
    while ((nanosleep(&ti, &ti) == -1) && (errno == EINTR)) {
    }
  }
#endif

  void sleep(Time duration) {
    // Note that 'std::this_thread::sleep_for' is intentionally not used here
    // as it results in inconsistent sleeping times under MinGW-w64.

    if (duration >= Time::Zero)
      sleepImpl(duration);
  }
} // namespace AltE