#pragma once



// CBuildDlg ������ͼ

class CBuildDlg : public CFormView
{
    DECLARE_DYNCREATE(CBuildDlg)

public:
    CBuildDlg();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
    virtual ~CBuildDlg();
//	BOOL CreateServer(BUILD_INFO *pBuildInfo,LPSTR szPath);
public:
    enum { IDD = IDD_DLG_BUILD };
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

    DECLARE_MESSAGE_MAP()
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
public:
    CBrush  m_brush;
    CXTPButton m_btnbuild;
    CXTPButton m_ck1;
    CXTPButton m_ck2;
    virtual void OnInitialUpdate();
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnBnClickedBtnBuild();
};


