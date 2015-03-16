//////////////////////////////////////////////////////////////////
//
//
/*************************************													
//	系統核心, System_Core.h - v0.1	
//  author : Y.Y.L Date : 02/27/03
/*************************************/
//-----------------------------------------------------------------

#ifndef _SYSTEM_CORE_H
#define _SYSTEM_CORE_H


class GSSysMgt
{
private:
	HINSTANCE	m_hInst;						// 實體handle
	HWND		m_hWnd;							// 視窗handle



	WNDCLASSEX  m_Winclassex;						// 視窗類別結構
protected:
	char        m_Class[MAX_PATH];
	char        m_Caption[MAX_PATH];

	DWORD		m_Style;							// 視窗形式
	DWORD		m_XPos;								// X coordinate 
	DWORD		m_YPos;								// Y coordinate
	DWORD		m_Width;							// width 
	DWORD		m_Height;							// height
public:
	GSSysMgt() {Create();}				
	~GSSysMgt(){Destory();}				
	

	// 建立/摧毀/Run window
	void		Create();
	void		Destory();
	BOOL Run();																					// 執行 視窗

    inline HWND			GethWnd()	{return m_hWnd;}				// 回傳 HWND
	inline HINSTANCE	GethInst(){return m_hInst;}				// 回傳 HINSTANCE
	inline void ShowMouse(BOOL Show=TRUE) 							// 顯示或隱藏MOUSE
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