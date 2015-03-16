
#include "..\..\Common\Global.h"

/////////////////////////////////////////////////////////////////////////////////
//
// GSSysMgt Functions	
// ------------------------------------------------------------------------------
//
// 初始化視窗基本架構

void GSSysMgt::Create()
{
	// 儲存 instatnce handle
  g_GSSysMgt = this;

  // 獲取instance handle
  m_hInst = GetModuleHandle(NULL);

  // 設定 window class and caption
	strcpy(m_Class,"Advance Computer Graphic");
	strcpy(m_Caption,"Advance Computer Graphic");

  // 設定 window style, position, width, height
  m_Style  = 	WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  m_XPos   = 0;		m_YPos   = 0;
  m_Width  = 0;		m_Height = 0;

   // 設定 WNDCLASSEX 結構
  ZeroMemory(&m_Winclassex,sizeof(m_Winclassex));	
	
  m_Winclassex.cbSize			= sizeof(WNDCLASSEX);
  m_Winclassex.style			= CS_CLASSDC;
  m_Winclassex.lpfnWndProc		= cGSSysMgtWindowProc;
  m_Winclassex.cbClsExtra		= 0;
  m_Winclassex.cbWndExtra		= 0;
  m_Winclassex.hInstance		= m_hInst;
  m_Winclassex.hIcon			= LoadIcon(NULL,IDI_APPLICATION);
  m_Winclassex.hCursor			= LoadCursor(NULL,IDC_ARROW);
  m_Winclassex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
  m_Winclassex.lpszMenuName		= NULL;
  m_Winclassex.lpszClassName	= m_Class;
  m_Winclassex.hIconSm			= LoadIcon(NULL,IDI_APPLICATION);

}
// ------------------------------------------------------------------------------
//
// 墔毀視窗架構
void GSSysMgt::Destory()
{	PostQuitMessage(0);}

// ------------------------------------------------------------------------------
//
// 執行視窗的messageloop
BOOL GSSysMgt::Run()
{
	MSG Msg;

	// 註冊視窗類別
	cassert(RegisterClassEx(&m_Winclassex),"註冊視窗失敗..");

	// 建立視窗
	cassert(m_hWnd = CreateWindow(
		m_Class,m_Caption,m_Style,
		m_XPos,m_YPos,
		m_Width,m_Height,
		NULL,LoadMenu(GethInst(),"MainMenu"),m_hInst,NULL),"建構視窗失敗..");

	ShowWindow(m_hWnd,SW_NORMAL);					// 顯示,更新視窗
	UpdateWindow(m_hWnd);			

	CoInitialize(NULL);										// 初始化 COM

	 // 進行初始化
  if(Init() == FALSE)  {
	MB("全部物件初始化失敗..");
	}


  // Message loop
  ZeroMemory(&Msg, sizeof(MSG));
  while(Msg.message != WM_QUIT) {
    if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
    if(FrameProcess() == FALSE)
      break;
  }
	
	Shutdown();								// 結束
	CoUninitialize();					// 釋放COM
	UnregisterClass(m_Class,m_hInst);	// 取消視窗的註冊
	return TRUE;
}

// End GSSysMgt Functions ////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
// 
// Message process
// ------------------------------------------------------------------------------
//
long FAR PASCAL cGSSysMgtWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    default: return g_GSSysMgt->MsgProc(hWnd, uMsg, wParam, lParam);
  }
}
//End Message processes ////////////////////////////////////////////////////////////

