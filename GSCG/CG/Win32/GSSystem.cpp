
#include "..\..\Common\Global.h"

/////////////////////////////////////////////////////////////////////////////////
//
// GSSysMgt Functions	
// ------------------------------------------------------------------------------
//
// ��l�Ƶ����򥻬[�c

void GSSysMgt::Create()
{
	// �x�s instatnce handle
  g_GSSysMgt = this;

  // ���instance handle
  m_hInst = GetModuleHandle(NULL);

  // �]�w window class and caption
	strcpy(m_Class,"Advance Computer Graphic");
	strcpy(m_Caption,"Advance Computer Graphic");

  // �]�w window style, position, width, height
  m_Style  = 	WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  m_XPos   = 0;		m_YPos   = 0;
  m_Width  = 0;		m_Height = 0;

   // �]�w WNDCLASSEX ���c
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
// �g�������[�c
void GSSysMgt::Destory()
{	PostQuitMessage(0);}

// ------------------------------------------------------------------------------
//
// ���������messageloop
BOOL GSSysMgt::Run()
{
	MSG Msg;

	// ���U�������O
	cassert(RegisterClassEx(&m_Winclassex),"���U��������..");

	// �إߵ���
	cassert(m_hWnd = CreateWindow(
		m_Class,m_Caption,m_Style,
		m_XPos,m_YPos,
		m_Width,m_Height,
		NULL,LoadMenu(GethInst(),"MainMenu"),m_hInst,NULL),"�غc��������..");

	ShowWindow(m_hWnd,SW_NORMAL);					// ���,��s����
	UpdateWindow(m_hWnd);			

	CoInitialize(NULL);										// ��l�� COM

	 // �i���l��
  if(Init() == FALSE)  {
	MB("���������l�ƥ���..");
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
	
	Shutdown();								// ����
	CoUninitialize();					// ����COM
	UnregisterClass(m_Class,m_hInst);	// �������������U
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

