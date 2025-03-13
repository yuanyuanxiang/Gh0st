
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#ifdef _DEBUG
// ����ڴ�й©���谲װVLD��������ע�ʹ���
// �����װ��VLD, �뽫��װ·����ӵ���������: ����Ϊ"VLDPATH", ·��Ϊ"D:\Program Files (x86)\Visual Leak Detector"
// �����ʵ�ʰ�װĿ¼��дVLDPATH. �����ֶ��༭ÿ����Ŀ�ļ���ͷ�ļ�Ŀ¼�Ϳ�Ŀ¼. �й�����VLD�����Ϣ��ο���������.
// VS2017��ǰ�汾��VLD: https://kinddragon.github.io/vld
// VS2019ʹ�õ�VLD��֧��������VS�汾, �Ƽ���: https://github.com/oneiric/vld/releases/tag/v2.7.0
// ���Ҫ���ܿض˳���ŵ�����������������, ��ʹ��Releaseģʽ���ɵĳ���, �Խ����VLD������; ��������Ҫ��VLD����ļ�һͬ����.
// ����VLD��ʾ����й¶������׷�ٲ��˺������ö�ջ�������������ʹ�÷��ŷ������Զ����ء������������Գ����ʱ�������
// ȷ����ĵ��Թ��ߣ��� Visual Studio �� WinDbg�������˷��ŷ�������
// ���ŷ��������Զ�����ȱʧ�ķ����ļ������� dbghelp.pdb�������仺�浽���ط���·����
// ���÷��ŷ��������� Visual Studio Ϊ�������� Visual Studio �У��� ���� > ѡ�� > ���š�
// ��ѡ Microsoft Symbol Servers. ָ�����Ż���Ŀ¼������ "C:\Symbols"��
// ����ʱ��ȱʧ�ķ��ţ��� dbghelp.pdb�����Զ����ص�����Ŀ¼��
#include "vld.h"
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��




#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include "ClientSocket.h"


struct DLL_INFO {
    TCHAR LoginAddr[100];    //���ߵ�ַ
    UINT LoginPort;
    TCHAR ServiceName[50];//��������
    TCHAR ServiceDisplayName[50]; //������ʾ
    TCHAR ServiceDescription[150];  //��������
    TCHAR UpGroup[32];       //����
    TCHAR strRemark[32];     //��ע
    BOOL NoDelete;         //TRUE-��ɾ����FALSE-ɾ��
    BOOL InshallShare;     //TRUE-��װ�������FALSE-�½�����
    BOOL NoInstall;        // TRUE ��ɫ��װ, FALSE ����
    TCHAR strPath[100];     //��װ·��
};




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


