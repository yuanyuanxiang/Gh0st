#pragma once
#include "afxcmn.h"


// CUrlDlg �Ի���

class CUrlDlg : public CDialog
{
    DECLARE_DYNAMIC(CUrlDlg)

public:
    CUrlDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
    void OnReceiveComplete();


// �Ի�������
    enum { IDD = IDD_DLG_URLHISTORY };
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    virtual void PostNcDestroy();
protected:

    HICON m_hIcon;
    ClientContext* m_pContext;
    CIOCPServer* m_iocpServer;
    CString m_strHost;

    DECLARE_MESSAGE_MAP()
    BOOL m_bOnClose;
    void ShowUrlList();
    void GetUrlList();
public:
    CHeaderCtrl   m_header;
    CListCtrl     m_list_url;
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
