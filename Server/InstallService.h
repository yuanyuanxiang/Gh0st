// InstallService.h: interface for the InstallService class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INSTALLSERVICE_H__CC062F67_F199_45A8_9C6D_296960666AE3__INCLUDED_)
#define AFX_INSTALLSERVICE_H__CC062F67_F199_45A8_9C6D_296960666AE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

BOOL NtStartService(LPCTSTR lpService);
BOOL NtStopService(LPCTSTR lpService);
BOOL NtInstallService(LPCTSTR strServiceName, //��������
                      LPCTSTR strDisplayName, //������ʾ����
                      LPCTSTR strDescription,//��������
                      LPCTSTR strPathName,   //��ִ���ļ���·��
                      LPCTSTR Dependencies,//ָ�������÷���ǰ�����������ķ���������,һ��ΪNULL
                      BOOLEAN KernelDriver, //�Ƿ�װ��������
                      ULONG   StartType		//��������
                     );
BOOL NtInstallSvchostService(LPCTSTR strServiceName,
                             LPCTSTR strServiceDisp,
                             LPCTSTR strServiceDesc,
                             LPCTSTR strDllPath);
void NtUninstallService(LPCTSTR ServiceName);
HANDLE RunInActiveSession(LPCTSTR lpCommandLine);
void DeleteSelf();
#endif // !defined(AFX_INSTALLSERVICE_H__CC062F67_F199_45A8_9C6D_296960666AE3__INCLUDED_)
