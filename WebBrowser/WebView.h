#pragma once
#include "wke.h"
#include "RenderGDI.h"

#define wkeWebViewClassName L"WKE_WEBVIEW_CLASSNAME"
class WebView
{
public:
	WebView(void){m_pRender = new CRenderGDI; m_cursorInfoType = 0;}
	~WebView(void){if(NULL != m_pRender) delete m_pRender;}
	BOOL RegisterControlerClass();
	BOOL Create(int x, int y, int nWeight, int nHeight, HWND hParent);

	void resizeSubViews(int x, int y, int nWeight, int nHeight);
	LRESULT webViewClassWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	__inline operator wkeWebView(){return m_hWebView;}
	__inline operator HWND(){return m_hWnd;}
protected:
	//ÏûÏ¢º¯Êý
	LRESULT OnCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	LRESULT OnPaint(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnChar(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSetCursor(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam);
	LRESULT OnIMEStartComposition(WPARAM wParam, LPARAM lParam);
	friend void onPaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
protected:
	HWND m_hParent; //¸¸´°¿Ú¾ä±ú
	wkeWebView m_hWebView; //wkeWebView ¾ä±ú
	HWND m_hWnd; //ÈÝÄÉwkeWebView ´°¿Ú¾ä±ú
	CRenderGDI *m_pRender;
	int m_cursorInfoType;
};

LRESULT CALLBACK webViewWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void onPaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);