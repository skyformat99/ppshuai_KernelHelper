// CAnimationDialog.cpp : implementation file
//

#include "Player.h"
#include "AnimationDialog.h"
#include "afxdialogex.h"


#define DEFAULT_IMAGES_COUNTER	5 //750ms
#define DEFAULT_ELAPSE_TIMEOUT	750 //750ms
#define IDC_TIMER_EVENT			1001 //�¼���Ϣ��ʶ����

// CAnimationDialog dialog

IMPLEMENT_DYNAMIC(CAnimationDialog, CDialogEx)

CAnimationDialog::CAnimationDialog(CWnd* pParent /*=NULL*/, 
	LPCTSTR pImagesName/* = _T("")*/, LPCTSTR pShowTips/* = _T("")*/)
	: CDialogEx(IDD_ANIMATION_DIALOG, pParent)
	, m_nElapse(DEFAULT_ELAPSE_TIMEOUT)
	, m_nCount(DEFAULT_IMAGES_COUNTER)
	, m_nIndex(0)
{
	m_wstrImagesName = pImagesName;
	m_wstrShowTips = pShowTips;
	m_rect.left = m_rect.top = 0;
	m_rect.right = m_rect.bottom = 360;
}

CAnimationDialog::~CAnimationDialog()
{
}

void CAnimationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnimationDialog, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAnimationDialog message handlers
///////////////////////////////////////////////////////////
//���´����ʵ�ֵĹ����Ǵ�ָ����·���ж�ȡͼƬ������ʾ����
//
void ImagesRenderDisplay(HDC hDC, RECT * pRect, _TCHAR * tImagePath)
{
	HANDLE hFile = NULL;

	DWORD dwReadedSize = 0; //����ʵ�ʶ�ȡ���ļ���С
	IStream *pIStream = NULL;//����һ��IStream�ӿ�ָ�룬��������ͼƬ��
	IPicture *pIPicture = NULL;//����һ��IPicture�ӿ�ָ�룬��ʾͼƬ����
	VOID * pImageMemory = NULL;
	HGLOBAL hImageMemory = NULL;
	OLE_XSIZE_HIMETRIC hmWidth = 0;
	OLE_YSIZE_HIMETRIC hmHeight = 0;
	LARGE_INTEGER liFileSize = { 0, 0 };

	hFile = ::CreateFile(tImagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);//��ָ����·��szImagePath�ж�ȡ�ļ����
	if (hFile != INVALID_HANDLE_VALUE)
	{
		liFileSize.LowPart = ::GetFileSize(hFile, (DWORD *)&liFileSize.HighPart); //���ͼƬ�ļ��Ĵ�С����������ȫ���ڴ�
		hImageMemory = ::GlobalAlloc(GMEM_MOVEABLE, liFileSize.QuadPart); //��ͼƬ����ȫ���ڴ�
		if (hImageMemory)
		{
			pImageMemory = ::GlobalLock(hImageMemory); //�����ڴ�

			::ReadFile(hFile, pImageMemory, liFileSize.QuadPart, &dwReadedSize, NULL); //��ȡͼƬ��ȫ���ڴ浱��

			::CreateStreamOnHGlobal(hImageMemory, false, &pIStream); //��ȫ���ڴ��ʹ��IStream�ӿ�ָ��
			if (pIStream)
			{
				::OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture));//��OleLoadPicture���IPicture�ӿ�ָ��
				if (pIPicture)														  //�õ�IPicture COM�ӿڶ���󣬾Ϳ��Խ��л��ͼƬ��Ϣ����ʾͼƬ�Ȳ���
				{
					pIPicture->get_Width(&hmWidth); //�ýӿڷ������ͼƬ�Ŀ�͸�
					pIPicture->get_Height(&hmHeight); //�ýӿڷ������ͼƬ�Ŀ�͸�
					pIPicture->Render(hDC, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, 0, hmHeight, hmWidth, -hmHeight, NULL); //��ָ����DC�ϻ��ͼƬ
				}
			}
		}
	}

	if (hImageMemory)
	{
		::GlobalUnlock(hImageMemory); //�����ڴ�

		::GlobalFree(hImageMemory); //�ͷ�ȫ���ڴ�
		hImageMemory = NULL;
	}
	if (pIPicture)
	{
		pIPicture->Release(); //�ͷ�pIPicture
		pIPicture = NULL;
	}
	if (pIStream)
	{
		pIStream->Release(); //�ͷ�pIStream
		pIStream = NULL;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile); //�ر��ļ����
		hFile = INVALID_HANDLE_VALUE;
	}
}
///////////////////////////////////////////////////////////
//���´����ʵ�ֵĹ����Ǵ�ָ����·���ж�ȡͼƬ������ʾ����
//
void ImagesDisplayScreen(SIZE & szImageSize, _TCHAR * tImagePath)
{
	HANDLE hFile = NULL;

	DWORD dwReadedSize = 0; //����ʵ�ʶ�ȡ���ļ���С
	IStream *pIStream = NULL;//����һ��IStream�ӿ�ָ�룬��������ͼƬ��
	IPicture *pIPicture = NULL;//����һ��IPicture�ӿ�ָ�룬��ʾͼƬ����
	VOID * pImageMemory = NULL;
	HGLOBAL hImageMemory = NULL;
	OLE_XSIZE_HIMETRIC hmWidth = 0;
	OLE_YSIZE_HIMETRIC hmHeight = 0;
	LARGE_INTEGER liFileSize = { 0, 0 };

	hFile = ::CreateFile(tImagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);//��ָ����·��szImagePath�ж�ȡ�ļ����
	if (hFile != INVALID_HANDLE_VALUE)
	{
		liFileSize.LowPart = ::GetFileSize(hFile, (DWORD *)&liFileSize.HighPart); //���ͼƬ�ļ��Ĵ�С����������ȫ���ڴ�
		hImageMemory = ::GlobalAlloc(GMEM_MOVEABLE, liFileSize.QuadPart); //��ͼƬ����ȫ���ڴ�
		if (hImageMemory)
		{
			pImageMemory = ::GlobalLock(hImageMemory); //�����ڴ�

			::ReadFile(hFile, pImageMemory, liFileSize.QuadPart, &dwReadedSize, NULL); //��ȡͼƬ��ȫ���ڴ浱��

			::CreateStreamOnHGlobal(hImageMemory, false, &pIStream); //��ȫ���ڴ��ʹ��IStream�ӿ�ָ��
			if (pIStream)
			{
				::OleLoadPicture(pIStream, 0, false, IID_IPicture, (LPVOID*)&(pIPicture));//��OleLoadPicture���IPicture�ӿ�ָ��
				if (pIPicture)														  //�õ�IPicture COM�ӿڶ���󣬾Ϳ��Խ��л��ͼƬ��Ϣ����ʾͼƬ�Ȳ���
				{
					pIPicture->get_Width(&hmWidth); //�ýӿڷ������ͼƬ�Ŀ�͸�
					pIPicture->get_Height(&hmHeight); //�ýӿڷ������ͼƬ�Ŀ�͸�
					szImageSize.cx = hmWidth;
					szImageSize.cy = hmHeight;
				}
			}
		}
	}

	if (hImageMemory)
	{
		::GlobalUnlock(hImageMemory); //�����ڴ�

		::GlobalFree(hImageMemory); //�ͷ�ȫ���ڴ�
		hImageMemory = NULL;
	}
	if (pIPicture)
	{
		pIPicture->Release(); //�ͷ�pIPicture
		pIPicture = NULL;
	}
	if (pIStream)
	{
		pIStream->Release(); //�ͷ�pIStream
		pIStream = NULL;
	}
	if (hFile != INVALID_HANDLE_VALUE)
	{
		::CloseHandle(hFile); //�ر��ļ����
		hFile = INVALID_HANDLE_VALUE;
	}
}

BOOL CAnimationDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowLong(this->m_hWnd, GWL_STYLE, GetWindowLong(this->m_hWnd, GWL_STYLE) & (~WS_CAPTION));
	::SetWindowPos(this->m_hWnd, HWND_NOTOPMOST, 0, 0, 
		m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, SWP_HIDEWINDOW);
	CenterWindowInScreen(this->m_hWnd);
	
	_TCHAR tImagePath[MAX_PATH] = { 0 };
	_stprintf_s(tImagePath, sizeof(tImagePath) / sizeof(_TCHAR), _T("%s%d.jpg"), m_wstrImagesName.c_str(), m_nIndex++);
	RECT rc = { 0 };
	GetClientRect(&rc);
	ImagesRenderDisplay(m_hRenderDC, &rc, tImagePath);

	m_nIndex %= m_nCount;

	::ShowWindow(this->m_hWnd, SW_SHOW);
	::SetForegroundWindow(this->m_hWnd);

	m_hRenderDC = ::GetDC(this->m_hWnd);//�豸����

	::SetTimer(this->m_hWnd, IDC_TIMER_EVENT, m_nElapse, NULL);

	return TRUE;
}

void CAnimationDialog::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case IDC_TIMER_EVENT:
	{
		_TCHAR tImagePath[MAX_PATH] = { 0 };
		_stprintf_s(tImagePath, sizeof(tImagePath) / sizeof(_TCHAR), _T("%s%d.jpg"), m_wstrImagesName.c_str(), m_nIndex++);
		RECT rc = { 0 };
		GetClientRect(&rc);
		ImagesRenderDisplay(m_hRenderDC, &rc, tImagePath);

		m_nIndex %= m_nCount;
		//::KillTimer(this->m_hWnd, nIDEvent);
	}
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CAnimationDialog::StartAnimation()
{
	this->DoModal();
}

void CAnimationDialog::StartAnimation(int nElapse, RECT * pRect)
{
	m_nElapse = nElapse;
	memcpy(&m_rect, pRect, sizeof(m_rect));	
	this->DoModal();
}

void CAnimationDialog::CloseAnimation()
{
	::KillTimer(this->m_hWnd, IDC_TIMER_EVENT);
	this->OnCancel();//ֹͣ��ȡ�����е��û�����
	this->DestroyWindow();//���ٴ���
	this->PostModal();//֪ͨ�����˳���Ϣѭ��
}