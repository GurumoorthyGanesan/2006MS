//////////////////////////////////////////////////////////////////
//
//
/*************************************													
//	�t�ή֤�, System_Core.h - v0.1	
//  author : Y.Y.L Date : 02/27/03
/*************************************/
//-----------------------------------------------------------------

#ifndef _SYSTEM_CORE_H
#define _SYSTEM_CORE_H


class GSSysMgt
{
private:
	HINSTANCE	m_hInst;						// ����handle
	HWND		m_hWnd;							// ����handle



	WNDCLASSEX  m_Winclassex;						// �������O���c
protected:
	char        m_Class[MAX_PATH];
	char        m_Caption[MAX_PATH];

	DWORD		m_Style;							// �����Φ�
	DWORD		m_XPos;								// X coordinate 
	DWORD		m_YPos;								// Y coordinate
	DWORD		m_Width;							// width 
	DWORD		m_Height;							// height
public:
	GSSysMgt() {Create();}				
	~GSSysMgt(){Destory();}				
	

	// �إ�/�R��/Run window
	void		Create();
	void		Destory();
	BOOL Run();																					// ���� ����

    inline HWND			GethWnd()	{return m_hWnd;}				// �^�� HWND
	inline HINSTANCE	GethInst(){return m_hInst;}				// �^�� HINSTANCE
	inline void ShowMouse(BOOL Show=TRUE) 							// ��ܩ�����MOUSE
	{ShowCursor(Show);}

	// Message handle
	virtual FAR PASCAL MsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{	return DefWindowProc(hWnd,uMsg,wParam,lParam);}

	//	flow control
	virtual BOOL Init()						{return TRUE;}
	virtual BOOL FrameProcess()		{return TRUE;}
	virtual BOOL Shutdown()				{return TRUE;}

};

static GSSysMgt *g_GSSysMgt = NULL;
static long FAR PASCAL  cGSSysMgtWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK ConnectDialogProc(HWND hWnd, UINT uMsg,WPARAM wParam, LPARAM lParam);
#endif