#pragma once


// CMainDlg �Ի���

class CMainDlg : public CDialog
{
    DECLARE_DYNAMIC(CMainDlg)

public:
    CMainDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CMainDlg();

// �Ի�������
    enum { IDD = IDD_DLG_MAIN };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
};
