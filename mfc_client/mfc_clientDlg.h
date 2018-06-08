
// mfc_clientDlg.h : 头文件
//

#pragma once

// Cmfc_clientDlg 对话框
class Cmfc_clientDlg : public CDialogEx
{
// 构造
public:
	Cmfc_clientDlg(CWnd* pParent = NULL);	// 标准构造函数

	//////////////////////////////////////////////////////
	void ThirdPartInit();
	/////////////////////////////////////////////////////
// 对话框数据
	enum { IDD = IDD_MFC_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonLogclean();
	afx_msg void OnBnClickedButtonLogpause();
	afx_msg void OnSelchangeComboConnmode();
	afx_msg void OnBnClickedButtonStarstream();
	afx_msg void OnBnClickedButtonConndev();
	afx_msg void OnSelchangeComboLoglevel();
};
