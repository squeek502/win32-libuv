#ifndef _WIN32_UV_TIMERS_H_
#define _WIN32_UV_TIMERS_H_

#include "uv.h"
#include <windows.h>

void timer_set_interval_win32(UINT interval);
void timer_set_interval_uv(UINT interval);
void win32uv_timers_init();

#endif