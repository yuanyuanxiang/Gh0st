// MainDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Server.h"
#include "MainDlg.h"


// CMainDlg �Ի���

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CMainDlg::IDD, pParent)
{

}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON1, &CMainDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CMainDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMainDlg ��Ϣ�������


void CMainDlg::OnBnClickedButton1()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    SendMessage(WM_CLOSE,0,0);

}

void CMainDlg::OnBnClickedButton2()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    OnCancel();
}
