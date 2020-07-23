#ifndef _WIN32_UV_WINDOW_H_
#define _WIN32_UV_WINDOW_H_

#include <stdbool.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define MESSAGE_ID_KILL_UV 1
#define MESSAGE_ID_WIN32_TIMER 2
#define MESSAGE_ID_UV_INTERVAL 3

HWND win32uv_window_init(HINSTANCE hInstance, WNDPROC wndproc);
bool win32uv_window_on_create(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool win32uv_window_on_resize(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool win32uv_window_on_command(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool win32uv_window_on_hscroll(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void win32uv_window_getminmaxinfo(LPMINMAXINFO minmaxinfo);

#endif