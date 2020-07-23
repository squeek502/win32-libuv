#include "win32uv.h"
#include "uv.h"
#include <windows.h>
#include <stdbool.h>
#include "win32uv_window.h"
#include "win32uv_timers.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND g_window;
HWND g_uv_loop_result;
HWND g_win32_loop_result;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nCmdShow)
{
	g_window = win32uv_window_init(hInstance, WndProc);

	win32uv_init();
	win32uv_run_loop_once();

	win32uv_timers_init();

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		if (IsDialogMessage(g_window, &msg))
			continue;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	win32uv_destroy();

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg) {

		case WIN32UV_MESSAGE_ID:
			win32uv_run_loop_once();
			return 0;

		case WM_CREATE:
			win32uv_window_on_create(hwnd, msg, wParam, lParam);
			break;

		case WM_SIZE:
			win32uv_window_on_resize(hwnd, msg, wParam, lParam);
			break;

		case WM_COMMAND:
			if (!win32uv_window_on_command(hwnd, msg, wParam, lParam))
			{
				return 0;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_HSCROLL:
			win32uv_window_on_hscroll(hwnd, msg, wParam, lParam);
			break;

		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
			SetBkMode((HDC)wParam, TRANSPARENT);
			return (LRESULT) GetSysColorBrush(COLOR_WINDOW);

		case WM_GETMINMAXINFO:
			win32uv_window_getminmaxinfo((LPMINMAXINFO)lParam);
			break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}