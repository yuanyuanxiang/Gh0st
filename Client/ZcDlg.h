#pragma once


// CZcDlg �Ի���

class CZcDlg : public CDialogEx
{
    DECLARE_DYNAMIC(CZcDlg)

public:
    CZcDlg(CWnd* pParent = NULL);   // ��׼���캯��
    virtual ~CZcDlg();

// �Ի�������
    enum { IDD = IDD_DLG_ZC };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
};
