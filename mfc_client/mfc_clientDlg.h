
// mfc_clientDlg.h : ͷ�ļ�
//

#pragma once

// Cmfc_clientDlg �Ի���
class Cmfc_clientDlg : public CDialogEx
{
// ����
public:
	Cmfc_clientDlg(CWnd* pParent = NULL);	// ��׼���캯��

	//////////////////////////////////////////////////////
	void ThirdPartInit();
	/////////////////////////////////////////////////////
// �Ի�������
	enum { IDD = IDD_MFC_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	afx_msg void OnBnClickedButton1();
};
