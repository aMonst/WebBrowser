
// WebBrowserDlg.h : 头文件
//

#pragma once
#include "WebView.h"
#include "DlgProxySet.h"

// CWebBrowserDlg 对话框
class CWebBrowserDlg : public CDialog
{
// 构造
public:
	CWebBrowserDlg(CWnd* pParent = NULL);	// 标准构造函数
	
// 对话框数据
	enum { IDD = IDD_WEBBROWSER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	void WaitForClose();
//	WebView m_web;
	wkeWebView m_web;
// 实现
protected:
	HICON m_hIcon;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedBtnForward();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnRefresh();
	afx_msg void OnBnClickedBtnGo();
	afx_msg void OnBnClickedBtnProxy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

void _cdecl wkeOnTitleChangedCallBack(wkeWebView webView, void* param, const wkeString title);
void _cdecl wkeOnURLChangedCallBack(wkeWebView webView, void* param, const wkeString url);
bool _cdecl wkeOnNavigationCallBack(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url);
wkeWebView _cdecl onBrowserCreateView(wkeWebView webView, void* param, wkeNavigationType navType, const wkeString urlStr, const wkeWindowFeatures* features);