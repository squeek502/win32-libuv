#include "win32uv_timers.h"
#include "win32uv_window.h"
#include "uv.h"

extern HWND g_window;
extern HWND g_uv_loop_result;
extern HWND g_win32_loop_result;

static uv_timer_t timer_uv;

static void timer_callback_uv(uv_timer_t* handle)
{
	static INT num_callbacks = 0;
	static WCHAR buf[256];
	num_callbacks++;
	int ret = swprintf(buf, 256, L"%d", num_callbacks);
	SendMessageW(g_uv_loop_result, WM_SETTEXT, (WPARAM)NULL, (LPARAM)buf);
}

static void timer_callback_win32(HWND hwnd, UINT msg, UINT_PTR timerId, DWORD dwTime)
{
	static INT num_callbacks = 0;
	static WCHAR buf[256];
	num_callbacks++;
	int ret = swprintf(buf, 256, L"%d", num_callbacks);
	SendMessageW(g_win32_loop_result, WM_SETTEXT, (WPARAM)NULL, (LPARAM)buf);
}

void win32uv_timers_init()
{
	uv_timer_init(uv_default_loop(), &timer_uv);
	uv_timer_start(&timer_uv, timer_callback_uv, 1000, 1000);

	timer_set_interval_win32(1000);
}

void timer_set_interval_win32(UINT interval)
{
	SetTimer(g_window, MESSAGE_ID_WIN32_TIMER, interval, timer_callback_win32);
}

void timer_set_interval_uv(UINT interval)
{
	uv_timer_set_repeat(&timer_uv, interval);
}
