
// WebBrowserDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WebBrowser.h"
#include "WebBrowserDlg.h"
#include "WebView.h"
#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CWebBrowserDlg 对话框
CWebBrowserDlg::CWebBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWebBrowserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWebBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWebBrowserDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_BACK, &CWebBrowserDlg::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BTN_FORWARD, &CWebBrowserDlg::OnBnClickedBtnForward)
	ON_BN_CLICKED(IDC_BTN_STOP, &CWebBrowserDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_BTN_REFRESH, &CWebBrowserDlg::OnBnClickedBtnRefresh)
	ON_BN_CLICKED(IDC_BTN_GO, &CWebBrowserDlg::OnBnClickedBtnGo)
	ON_BN_CLICKED(IDC_BTN_PROXY, &CWebBrowserDlg::OnBnClickedBtnProxy)
	ON_WM_CHAR()
END_MESSAGE_MAP()

// CWebBrowserDlg 消息处理程序

BOOL CWebBrowserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	RECT rtClient;
	GetClientRect(&rtClient);
	rtClient.top += 24;
	m_web.Create(rtClient.left, rtClient.top, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, *this);
//	m_web = wkeCreateWebWindow(WKE_WINDOW_TYPE_CONTROL, *this, rtClient.left, rtClient.top, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
	wkeOnTitleChanged(m_web, wkeOnTitleChangedCallBack, this);
	wkeOnURLChanged(m_web, wkeOnURLChangedCallBack, this);
	wkeOnNavigation(m_web, wkeOnNavigationCallBack, this);
	wkeOnCreateView(m_web, onBrowserCreateView, this);
	
	
	wkeLoadURL(m_web, "https://www.baidu.com");
	wkeShowWindow(m_web, TRUE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWebBrowserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWebBrowserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWebBrowserDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	DestroyWindow();
	//CDialog::OnClose();
}

void CWebBrowserDlg::OnBnClickedBtnBack()
{
	// TODO: 在此添加控件通知处理程序代码
	if (wkeCanGoBack(m_web))
	{
		wkeGoBack(m_web);
	}
}

void CWebBrowserDlg::OnBnClickedBtnForward()
{
	// TODO: 在此添加控件通知处理程序代码
	if (wkeCanGoForward(m_web))
	{
		wkeGoForward(m_web);
	}
}

void CWebBrowserDlg::OnBnClickedBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
	wkeStopLoading(m_web);
}

void CWebBrowserDlg::OnBnClickedBtnRefresh()
{
	// TODO: 在此添加控件通知处理程序代码
	wkeReload(m_web);
}

void CWebBrowserDlg::OnBnClickedBtnGo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString csurl;
	GetDlgItem(IDC_EDIT_URL)->GetWindowText(csurl);
	wkeLoadURLW(m_web, csurl);
}

void CWebBrowserDlg::OnBnClickedBtnProxy()
{
	CDlgProxySet dlgProxySet;
	dlgProxySet.DoModal();
	wkeProxy proxy;
	proxy.type = WKE_PROXY_HTTP;
	USES_CONVERSION;
	strcpy_s(proxy.hostname, sizeof(proxy.hostname), T2A(dlgProxySet.csIP));

	proxy.port = dlgProxySet.m_port;
	wkeSetProxy(&proxy);
	// TODO: 在此添加控件通知处理程序代码
}

void _cdecl wkeOnTitleChangedCallBack(wkeWebView webView, void* param, const wkeString title)
{
	CWebBrowserDlg *pDlg = (CWebBrowserDlg*)param;
	if (NULL != pDlg)
	{
		pDlg->SetWindowText(wkeGetStringW(title));
	}
}

void _cdecl wkeOnURLChangedCallBack(wkeWebView webView, void* param, const wkeString url)
{
	CWebBrowserDlg *pDlg = (CWebBrowserDlg*)param;
	if (NULL != pDlg)
	{
		pDlg->GetDlgItem(IDC_EDIT_URL)->SetWindowTextW(wkeGetStringW(url));
	}
}

bool _cdecl wkeOnNavigationCallBack(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url)
{
	const wchar_t* urlStr = wkeGetStringW(url);
	if (wcsstr(urlStr, L"exec://") == urlStr) {
		PROCESS_INFORMATION processInfo = { 0 };
		STARTUPINFOW startupInfo = { 0 };
		startupInfo.cb = sizeof(startupInfo);
		BOOL succeeded = CreateProcessW(NULL, (LPWSTR)urlStr + 7, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
		if (succeeded) {
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
		}
		return false;
	}

	return true;
}

wkeWebView _cdecl onBrowserCreateView(wkeWebView webView, void* param, wkeNavigationType navType, const wkeString urlStr, const wkeWindowFeatures* features)
{
	const wchar_t* url = wkeGetStringW(urlStr);

	wkeWebView newWindow = wkeCreateWebWindow(WKE_WINDOW_TYPE_POPUP, NULL, features->x, features->y, features->width, features->height);
	wkeShowWindow(newWindow, true);
	return newWindow;
}

void CWebBrowserDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	unsigned int flags = 0;
	if (nFlags & KF_REPEAT)
		flags |= WKE_REPEAT;
	if (nFlags & KF_EXTENDED)
		flags |= WKE_EXTENDED;

	wkeFireKeyPressEvent(m_web, nChar, flags, false);
	CDialog::OnChar(nChar, nRepCnt, nFlags);
}
