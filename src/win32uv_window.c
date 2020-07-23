#include "win32uv_window.h"
#include "win32uv_timers.h"
#include "win32uv.h"
#include <commctrl.h>

extern HWND g_uv_loop_result;
extern HWND g_win32_loop_result;

#define WINDOW_CLASS_NAME L"Win32-Libuv"

#define BUTTON_W 75
#define DEFAULT_CONTROL_H 23
#define PADDING_SM 7
#define PADDING 11
#define PADDING_LABEL_TEXT 5
#define LABEL_H 13

static HFONT hMessageFont;
static HFONT hMessageFontBold;
static HWND kill_loop_button;
static HWND uv_loop_label;
static HWND win32_loop_label;
static HWND uv_interval_slider;
static HWND uv_interval_slider_label;
static HWND win32_interval_slider;
static HWND win32_interval_slider_label;
static HWND timer_count_label;

HWND win32uv_window_init(HINSTANCE hInstance, WNDPROC wndproc)
{
	HICON hDefaultIcon = LoadIcon(NULL, IDI_APPLICATION);
	HCURSOR hDefaultCursor = LoadCursor(NULL, IDC_ARROW);
	LOGFONT messageFontBold;

	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
	hMessageFont = CreateFontIndirect(&(ncm.lfStatusFont));

	memcpy(&messageFontBold, &ncm.lfStatusFont, sizeof(LOGFONT));
	messageFontBold.lfWeight = FW_BOLD;
	hMessageFontBold = CreateFontIndirect(&messageFontBold);

	WNDCLASSW wc = { 0 };
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
	wc.lpfnWndProc = wndproc;
	wc.hCursor = hDefaultCursor;
	wc.hIcon = hDefaultIcon;

	RegisterClassW(&wc);
	return CreateWindowW(wc.lpszClassName, WINDOW_CLASS_NAME,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 200, NULL, NULL, hInstance, NULL);
}

bool win32uv_window_on_create(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	timer_count_label = CreateWindowW(L"Static", L"Timer callback calls:",
		WS_VISIBLE | WS_CHILD | SS_NOPREFIX,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(timer_count_label, WM_SETFONT, (WPARAM)hMessageFontBold, (LPARAM)MAKELONG(TRUE, 0));

	uv_loop_label = CreateWindowW(L"Static", L"Libuv:",
		WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_NOPREFIX,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(uv_loop_label, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	g_uv_loop_result = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"0",
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(g_uv_loop_result, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	win32_loop_label = CreateWindowW(L"Static", L"Win32:",
		WS_VISIBLE | WS_CHILD | SS_RIGHT | SS_NOPREFIX,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(win32_loop_label, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	g_win32_loop_result = CreateWindowExW(WS_EX_CLIENTEDGE, L"Edit", L"0",
		WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(g_win32_loop_result, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	kill_loop_button = CreateWindowW(L"Button", L"Kill Libuv Loop",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		0, 0, 0, 0, hwnd, (HMENU)MESSAGE_ID_KILL_UV, NULL, NULL);
	SendMessageW(kill_loop_button, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	uv_interval_slider = CreateWindowExW(0, TRACKBAR_CLASSW, NULL,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS | TBS_AUTOTICKS | TBS_NOTIFYBEFOREMOVE,
		0, 0, 0, 0, hwnd, (HMENU)MESSAGE_ID_UV_INTERVAL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(uv_interval_slider, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));
	SendMessageW(uv_interval_slider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(100, 2000));
	SendMessageW(uv_interval_slider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)1000);
	SendMessageW(uv_interval_slider, TBM_SETPAGESIZE, 0, (LPARAM)100);
	SendMessageW(uv_interval_slider, TBM_SETLINESIZE, 0, (LPARAM)100);
	SendMessageW(uv_interval_slider, TBM_SETTICFREQ, (WPARAM)100, 0);

	uv_interval_slider_label = CreateWindowExW(0, L"Static", L"Interval:", SS_RIGHT | WS_CHILD | WS_VISIBLE,
		0, 0, 0, 0, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(uv_interval_slider_label, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	win32_interval_slider = CreateWindowExW(0, TRACKBAR_CLASSW, NULL,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | TBS_TOOLTIPS | TBS_AUTOTICKS | TBS_NOTIFYBEFOREMOVE,
		0, 0, 0, 0, hwnd, (HMENU)MESSAGE_ID_UV_INTERVAL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(win32_interval_slider, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));
	SendMessageW(win32_interval_slider, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(100, 2000));
	SendMessageW(win32_interval_slider, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)1000);
	SendMessageW(win32_interval_slider, TBM_SETPAGESIZE, 0, (LPARAM)100);
	SendMessageW(win32_interval_slider, TBM_SETLINESIZE, 0, (LPARAM)100);
	SendMessageW(win32_interval_slider, TBM_SETTICFREQ, (WPARAM)100, 0);

	win32_interval_slider_label = CreateWindowExW(0, L"Static", L"Interval:", SS_RIGHT | WS_CHILD | WS_VISIBLE,
		0, 0, 0, DEFAULT_CONTROL_H, hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
	SendMessageW(win32_interval_slider_label, WM_SETFONT, (WPARAM)hMessageFont, (LPARAM)MAKELONG(TRUE, 0));

	return true;
}

bool win32uv_window_on_resize(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int window_w = LOWORD(lParam);
	int window_h = HIWORD(lParam);

	MoveWindow(kill_loop_button, window_w - (BUTTON_W * 2) - PADDING, window_h - DEFAULT_CONTROL_H - PADDING, BUTTON_W * 2, DEFAULT_CONTROL_H, TRUE);

	int rowY = PADDING;
	int x = PADDING;

	MoveWindow(timer_count_label, x, rowY + PADDING_LABEL_TEXT, window_w - PADDING * 2, DEFAULT_CONTROL_H, TRUE);

	rowY += DEFAULT_CONTROL_H + PADDING;
	MoveWindow(uv_loop_label, x, rowY + PADDING_LABEL_TEXT, 35, DEFAULT_CONTROL_H, TRUE);
	x += 35 + PADDING_SM;
	MoveWindow(g_uv_loop_result, x, rowY, 100, DEFAULT_CONTROL_H, TRUE);
	x += 100 + PADDING;
	MoveWindow(uv_interval_slider_label, x, rowY + PADDING_LABEL_TEXT, 50, DEFAULT_CONTROL_H, TRUE);
	x += 50 + PADDING_SM;
	MoveWindow(uv_interval_slider, x, rowY, window_w - PADDING - x, DEFAULT_CONTROL_H, TRUE);

	rowY += DEFAULT_CONTROL_H + PADDING;
	x = PADDING;
	MoveWindow(win32_loop_label, x, rowY + PADDING_LABEL_TEXT, 35, DEFAULT_CONTROL_H, TRUE);
	x += 35 + PADDING_SM;
	MoveWindow(g_win32_loop_result, x, rowY, 100, DEFAULT_CONTROL_H, TRUE);
	x += 100 + PADDING;
	MoveWindow(win32_interval_slider_label, x, rowY + PADDING_LABEL_TEXT, 50, DEFAULT_CONTROL_H, TRUE);
	x += 50 + PADDING_SM;
	MoveWindow(win32_interval_slider, x, rowY, window_w - PADDING - x, DEFAULT_CONTROL_H, TRUE);

	return true;
}

bool win32uv_window_on_command(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// current control is (HWND)lParam

	if (LOWORD(wParam) == MESSAGE_ID_KILL_UV)
	{
		EnableWindow(kill_loop_button, false);
		EnableWindow(uv_loop_label, false);
		EnableWindow(g_uv_loop_result, false);
		EnableWindow(uv_interval_slider_label, false);
		EnableWindow(uv_interval_slider, false);
		SendMessageW(kill_loop_button, WM_SETTEXT, (WPARAM)NULL, (LPARAM)(L"Libuv Loop Killed"));
		win32uv_destroy();
		return false;
	}

	return true;
}

bool win32uv_window_on_hscroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == uv_interval_slider || (HWND)lParam == win32_interval_slider)
	{
		UINT timer_interval;
		if (LOWORD(wParam) == SB_THUMBPOSITION || LOWORD(wParam) == SB_THUMBTRACK)
		{
			timer_interval = (HIWORD(wParam) + 50) / 100 * 100;
			SendMessageW((HWND)lParam, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)timer_interval);
		}
		else
		{
			timer_interval = (UINT)SendMessageW((HWND)lParam, TBM_GETPOS, 0, 0);
		}

		if ((HWND)lParam == uv_interval_slider)
			timer_set_interval_uv(timer_interval);
		else
			timer_set_interval_win32(timer_interval);
	}
	return true;
}

void win32uv_window_getminmaxinfo(LPMINMAXINFO minmaxinfo)
{
	minmaxinfo->ptMinTrackSize.x = 350;
	minmaxinfo->ptMinTrackSize.y = 200;
}
