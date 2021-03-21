#include "Window.h"

#ifndef __Window
#define __Window
int gl_screen_exit;
int gl_screen_keys[512];
static HWND screen_handle;
static HDC screen_dc;			// ÅäÌ×µÄ HDC
static HBITMAP screen_hb;		// DIB
static HBITMAP screen_ob;		// ÀÏµÄ BITMAP
#endif // !__Window

int Window::screen_init(float w, float h, const TCHAR* title)
{
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
		NULL, NULL, NULL, NULL, _T("SoftwareRenderer") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		w * h * 4, 0, 0, 0, 0 } };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	screen_close();

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	if (!RegisterClass(&wc)) return -1;

	screen_handle = CreateWindow(_T("SoftwareRenderer"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
	if (screen_handle == NULL) return -2;

	screen_exit = 0;
	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if (screen_hb == NULL) return -3;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
	screen_fb = (unsigned char*)ptr;
	screen_width = w;
	screen_height = h;
	screen_pitch = w * 4;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);
	screen_dispatch();

	memset(gl_screen_keys, 0, sizeof(int) * 512);
	memset(screen_fb, 0, w * h * 4);

	this->screen_exit = &gl_screen_exit;
	this->screen_keys = gl_screen_keys;

	return 0;
}

Window::Window()
{
	screen_exit = nullptr;
	screen_keys = nullptr;
	screen_handle = NULL;
	screen_dc = NULL;
	screen_hb = NULL;
	screen_ob = NULL;
	screen_fb = NULL;
}

int Window::screen_close(void)
{
	if (screen_dc) {
		if (screen_ob) {
			SelectObject(screen_dc, screen_ob);
			screen_ob = NULL;
		}
		DeleteDC(screen_dc);
		screen_dc = NULL;
	}
	if (screen_hb) {
		DeleteObject(screen_hb);
		screen_hb = NULL;
	}
	if (screen_handle) {
		CloseWindow(screen_handle);
		screen_handle = NULL;
	}
	return 0;
}

void Window::screen_dispatch(void) {
	MSG msg;
	while (1) {
		if (!PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, NULL, 0, 0)) break;
		DispatchMessage(&msg);
	}
}

void Window::screen_update(void)
{
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_width, screen_height, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	screen_dispatch();
}

LRESULT Window::screen_events(HWND hWnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE: gl_screen_exit = 1; break;
	case WM_KEYDOWN: gl_screen_keys[wParam & 511] = 1; break;
	case WM_KEYUP: gl_screen_keys[wParam & 511] = 0; break;
	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
