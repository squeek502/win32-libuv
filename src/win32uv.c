#include "uv.h"
#include <stdbool.h>
#include "win32uv.h"

static uv_async_t dummy_async_handle;
static uv_sem_t polling_sem;
static uv_thread_t polling_thread;
static bool polling_thread_closed = true;
static void win32uv_polling_thread_run(void* data);

extern HWND g_window;

void win32uv_init()
{
	// keep the loop alive and allow waking up the polling thread
	uv_async_init(uv_default_loop(), &dummy_async_handle, NULL);

	uv_sem_init(&polling_sem, 0);
	uv_thread_create(&polling_thread, win32uv_polling_thread_run, NULL);
	polling_thread_closed = false;
}

static void win32uv_stop_polling()
{
	polling_thread_closed = true;
	uv_sem_post(&polling_sem);
	// wake up polling thread
	uv_async_send(&dummy_async_handle);

	uv_thread_join(&polling_thread);

	uv_sem_destroy(&polling_sem);
	uv_close((uv_handle_t*)&dummy_async_handle, NULL);
}

static void win32uv_close_loop_walk_cb(uv_handle_t* handle, void* arg)
{
	if (!uv_is_closing(handle))
		uv_close(handle, NULL);
}

static void win32uv_close_loop()
{
	uv_stop(uv_default_loop());
	uv_walk(uv_default_loop(), win32uv_close_loop_walk_cb, NULL);

	for (;;)
		if (!uv_run(uv_default_loop(), UV_RUN_DEFAULT))
			break;

	uv_loop_close(uv_default_loop());
}

void win32uv_destroy()
{
	if (!polling_thread_closed)
		win32uv_stop_polling();
	win32uv_close_loop();
}

void win32uv_run_loop_once()
{
	int r = uv_run(uv_default_loop(), UV_RUN_NOWAIT);
	uv_sem_post(&polling_sem);
}

static void win32uv_poll()
{
	// If there are other kinds of events pending, uv_backend_timeout will
	// instruct us not to wait.
	DWORD bytes, timeout;
	ULONG_PTR key;
	OVERLAPPED* overlapped;

	timeout = uv_backend_timeout(uv_default_loop());

	GetQueuedCompletionStatus(uv_default_loop()->iocp, &bytes, &key, &overlapped, timeout);

	// Give the event back so libuv can deal with it.
	if (overlapped != NULL)
		PostQueuedCompletionStatus(uv_default_loop()->iocp, bytes, key, overlapped);
}

static void win32uv_post_message()
{
	PostMessageW(g_window, WIN32UV_MESSAGE_ID, (WPARAM)NULL, (LPARAM)NULL);
}

static void win32uv_polling_thread_run(void* data)
{
	while (true)
	{
		uv_sem_wait(&polling_sem);
		if (polling_thread_closed)
			break;

		win32uv_poll();
		if (polling_thread_closed)
			break;

		win32uv_post_message();
	}
}
