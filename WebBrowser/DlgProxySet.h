#pragma once


// CDlgProxySet 对话框

class CDlgProxySet : public CDialog
{
	DECLARE_DYNAMIC(CDlgProxySet)

public:
	CDlgProxySet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgProxySet();
	CString csIP;
// 对话框数据
	enum { IDD = IDD_DLG_PROXY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	DWORD m_IP;
	UINT m_port;
};
