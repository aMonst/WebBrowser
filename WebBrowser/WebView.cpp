#include "stdafx.h"
#include "WebView.h"

#include <windows.h>
#include <Imm.h>
#pragma comment(lib, "Imm32.lib")

BOOL WebView::RegisterControlerClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_DBLCLKS;// CS_HREDRAW | CS_VREDRAW; // 
	wcex.lpfnWndProc = webViewWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = AfxGetInstanceHandle();
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = wkeWebViewClassName;
	wcex.hIconSm = 0;

	return !!RegisterClassEx(&wcex);
}

BOOL WebView::Create(int x, int y, int nWeight, int nHeight, HWND hParent)
{
	m_hWebView = wkeCreateWebView();
	wkeSetTransparent(m_hWebView, false);
	wkeOnPaintUpdated(m_hWebView, onPaintUpdatedCallback, this);
	if (!m_hWebView)
	{
		return FALSE;
	}
	
	if (!RegisterControlerClass())
	{
		return FALSE;
	}

	m_hWnd = CreateWindow(wkeWebViewClassName, 0,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		x, y, nWeight, nHeight,
		hParent,
		0,
		AfxGetInstanceHandle(), 0);

	if (INVALID_HANDLE_VALUE == m_hWnd)
	{
		return FALSE;
	}

	m_hParent = hParent;

	wkeSetHandle(m_hWebView, m_hWnd);
	resizeSubViews(x, y, nWeight, nHeight);
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
	assert(NULL != m_pRender);
	return m_pRender->init(m_hWnd);
}

LRESULT CALLBACK webViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	WebView* pView = (WebView*)GetWindowLong(hWnd, GWL_USERDATA);
	if (pView)
	{
		return pView->webViewClassWndProc(uMsg, wParam, lParam);
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void WebView::resizeSubViews(int x, int y, int nWeight, int nHeight)
{
	if (m_hWebView)
	{
		MoveWindow(m_hWnd, x, y, nWeight, nHeight, TRUE);

		wkeResize(m_hWebView, nWeight, nHeight);
	}
}

LRESULT WebView::webViewClassWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SETCURSOR:
		return OnSetCursor(wParam, lParam);
	case WM_COMMAND:
		return OnCommand(wParam, lParam);
	case WM_SIZE:
		return OnSize(wParam, lParam);
	case WM_PAINT:
		return OnPaint(wParam, lParam);
	case WM_KEYDOWN:
		return OnKeyDown(wParam, lParam);
	case WM_KEYUP:
		return OnKeyUp(wParam, lParam);
	case WM_CHAR:
		return OnChar(wParam, lParam);
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		return OnMouseEvent(uMsg, wParam, lParam);

	case WM_MOUSEWHEEL:
		return OnMouseWheel(wParam, lParam);
	case WM_SETFOCUS:
		return OnSetFocus(wParam, lParam);

	case WM_KILLFOCUS:
		return OnKillFocus(wParam, lParam);
	case WM_IME_STARTCOMPOSITION:
		return OnIMEStartComposition(wParam, lParam);
	case WM_GETDLGCODE:
		return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;
	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT WebView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	SendMessage(m_hParent, WM_COMMAND, wParam, lParam);
	return 0;
}

LRESULT WebView::OnSize(WPARAM wParam, LPARAM lParam)
{
	if (NULL != m_hWebView && NULL != m_pRender)
	{
		wkeResize(m_hWebView, LOWORD(lParam), HIWORD(lParam));
		m_pRender->resize(LOWORD(lParam), HIWORD(lParam));
	}

	return 0;
}

LRESULT WebView::OnPaint(WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintInfo;
	BeginPaint(m_hWnd, &paintInfo);
	m_pRender->renderOnWindowPaint(m_hWebView, paintInfo.hdc);
	EndPaint(m_hWnd, &paintInfo);
	return 0;
}

LRESULT WebView::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	unsigned int virtualKeyCode = wParam;
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	wkeFireKeyDownEvent(m_hWebView, virtualKeyCode, flags, false);
	return 0;
}

LRESULT WebView::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	unsigned int virtualKeyCode = wParam;
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	wkeFireKeyUpEvent(m_hWebView, virtualKeyCode, flags, false);
	return 0;
}

LRESULT WebView::OnChar(WPARAM wParam, LPARAM lParam)
{
	unsigned int charCode = wParam;
	unsigned int flags = 0;
	if (HIWORD(lParam) & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (HIWORD(lParam) & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	wkeFireKeyPressEvent(m_hWebView, charCode, flags, false);
	return 0;
}

LRESULT WebView::OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_cursorInfoType = wkeGetCursorInfoType(m_hWebView);

	if (uMsg == WM_LBUTTONDOWN || uMsg == WM_MBUTTONDOWN || uMsg == WM_RBUTTONDOWN) {
		SetFocus(m_hWnd);
		SetCapture(m_hWnd);
	} else if (uMsg == WM_LBUTTONUP || uMsg == WM_MBUTTONUP || uMsg == WM_RBUTTONUP) {
		ReleaseCapture();
	}

	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	//flags = wParam;

	wkeFireMouseEvent(m_hWebView, uMsg, x, y, flags);

	return 0;
}

LRESULT WebView::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	::GetClientRect(m_hWnd, &rc);

	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_hWnd, &pt);
	if (!::PtInRect(&rc, pt))
		return false;

	HCURSOR hCur = NULL;
	switch (m_cursorInfoType) {
	case WkeCursorInfoPointer:
		hCur = ::LoadCursor(NULL, IDC_ARROW);
		break;
	case WkeCursorInfoIBeam:
		hCur = ::LoadCursor(NULL, IDC_IBEAM);
		break;
	case WkeCursorInfoHand:
		hCur = ::LoadCursor(NULL, IDC_HAND);
		break;
	case WkeCursorInfoWait:
		hCur = ::LoadCursor(NULL, IDC_WAIT);
		break;
	case WkeCursorInfoHelp:
		hCur = ::LoadCursor(NULL, IDC_HELP);
		break;
	case WkeCursorInfoEastResize:
		hCur = ::LoadCursor(NULL, IDC_SIZEWE);
		break;
	case WkeCursorInfoNorthResize:
		hCur = ::LoadCursor(NULL, IDC_SIZENS);
		break;
	case WkeCursorInfoSouthWestResize:
	case WkeCursorInfoNorthEastResize:
		hCur = ::LoadCursor(NULL, IDC_SIZENESW);
		break;
	case WkeCursorInfoSouthResize:
	case WkeCursorInfoNorthSouthResize:
		hCur = ::LoadCursor(NULL, IDC_SIZENS);
		break;
	case WkeCursorInfoNorthWestResize:
	case WkeCursorInfoSouthEastResize:
		hCur = ::LoadCursor(NULL, IDC_SIZENWSE);
		break;
	case WkeCursorInfoWestResize:
	case WkeCursorInfoEastWestResize:
		hCur = ::LoadCursor(NULL, IDC_SIZEWE);
		break;
	case WkeCursorInfoNorthEastSouthWestResize:
	case WkeCursorInfoNorthWestSouthEastResize:
		hCur = ::LoadCursor(NULL, IDC_SIZEALL);
		break;
	default:
		hCur = ::LoadCursor(NULL, IDC_ARROW);
		break;
	}

	if (hCur) {
		::SetCursor(hCur);
	}

	return 0;
}

LRESULT WebView::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);
	ScreenToClient(m_hWnd, &pt);

	int delta = GET_WHEEL_DELTA_WPARAM(wParam);

	unsigned int flags = 0;

	if (wParam & MK_CONTROL)
		flags |= WKE_CONTROL;
	if (wParam & MK_SHIFT)
		flags |= WKE_SHIFT;

	if (wParam & MK_LBUTTON)
		flags |= WKE_LBUTTON;
	if (wParam & MK_MBUTTON)
		flags |= WKE_MBUTTON;
	if (wParam & MK_RBUTTON)
		flags |= WKE_RBUTTON;

	//flags = wParam;

	wkeFireMouseWheelEvent(m_hWebView, pt.x, pt.y, delta, flags);

	return 0;
}

LRESULT WebView::OnSetFocus(WPARAM wParam, LPARAM lParam)
{
	wkeSetFocus(m_hWebView);
	return 0;
}

LRESULT WebView::OnKillFocus(WPARAM wParam, LPARAM lParam)
{
	wkeKillFocus(m_hWebView);
	return 0;
}

LRESULT WebView::OnIMEStartComposition(WPARAM wParam, LPARAM lParam)
{
	wkeRect caret = wkeGetCaretRect(m_hWebView);

	CANDIDATEFORM form;
	form.dwIndex = 0;
	form.dwStyle = CFS_EXCLUDE;
	form.ptCurrentPos.x = caret.x;
	form.ptCurrentPos.y = caret.y + caret.h;
	form.rcArea.top = caret.y;
	form.rcArea.bottom = caret.y + caret.h;
	form.rcArea.left = caret.x;
	form.rcArea.right = caret.x + caret.w;

	HIMC hIMC = ImmGetContext(m_hWnd);
	ImmSetCandidateWindow(hIMC, &form);
	ImmReleaseContext(m_hWnd, hIMC);
	return 0;
}

void onPaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
	WebView* pView = (WebView*)param;
	if (NULL != pView)
	{
		pView->m_pRender->renderOnBlinkPaint(pView->m_hWebView, hdc, x, y, cx, cy);
	}
}