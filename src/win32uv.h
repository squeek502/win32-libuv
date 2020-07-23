#ifndef _WIN32_UV_H_
#define _WIN32_UV_H_

// Arbitrary value within the WM_USER range of 0x0400-0x7FFF
#define WIN32UV_MESSAGE_ID 0x7487

void win32uv_init();
void win32uv_destroy();
void win32uv_run_loop_once();

#endif