
#include "WinMain.h"

OPENFILENAME	g_ofn;            // Open/Save dialog data
char			xFile[MAX_PATH];      // Filename for character file
GSMain            *g_Application;	
	
CUSTOMVERTEX4F vertices[] =
{
    { 150.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, // x, y, z, rhw, color
    { 250.0f, 250.0f, 0.5f, 1.0f, 0xff00ff00, },
    {  50.0f, 250.0f, 0.5f, 1.0f, 0xff00ffff, },
};

GSMain::GSMain()
{
	// 設定視窗形式，位置，長寬
	m_XPos	= 0;
	m_YPos	= 0;
	m_Width  = SCREEN_WIDTH; 
	m_Height = SCREEN_HEIGHT;
	m_Style  = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	strcpy(m_Class,"GSMain");
	strcpy(m_Caption,"Goodspeed's CG");

	// load the menu resource
	HMENU hmenuhandle = LoadMenu(GethInst(), "MainMenu");

	// attach the menu to the window
	SetMenu(GethWnd(), hmenuhandle);

	
	// 功能開關
	ClearUp();
}

GSMain::~GSMain(){
	Shutdown();
}

BOOL GSMain::Init()
{
  if(Initialize() == FALSE) {
    MB("未 能 初 始 化 完 成");
    return FALSE;
  }
  return TRUE;
}

BOOL GSMain::Shutdown()
{
  // 關閉 圖形裝置
  m_Dxg.Shutdown();
  m_gsD3DX.Shutdown();
  m_gsFont.Free();
  m_gsMesh.Free();

  // 關閉 輸入裝置
  m_Keyboard.Free();
  m_Mouse.Free();
  m_Input.Shutdown();


  return TRUE;
}
BOOL GSMain::Initialize()
{
  ShowMouse(TRUE);


  // 
  // computer graphic
  //
  m_vertex = new vertex2D;
  ptr = new vertex2D;

  m_CVBP = new CUSTOMVERTEX4F;
  m_CVBP_Ptr = new CUSTOMVERTEX4F;
  m_CVBP_Ptr = NULL;
  // 圖形裝置
  m_Dxg.Init();
  

  // 初始化輸入裝置
  m_Input.Init(GethWnd(), GethInst());
  m_Keyboard.Create(&m_Input, KEYBOARD);
  m_Mouse.Create(&m_Input, MOUSE, TRUE);

  m_gs2D.Init(GethWnd(), GethInst());
  m_gs3D.Init(GethWnd(), GethInst());
  // 3D
  m_gsD3DX.Init();
  m_gsD3DX.SetMode(GethWnd(),TRUE,TRUE,m_Width,m_Height);
  m_gsD3DX.SetPerspective(0.6021124f, 1.333333f, 1.0f, 1000.0f);

  // font
  m_gsFont.Create(&m_gsD3DX, "Arial", 12, TRUE);

  return TRUE;
}


BOOL GSMain::FrameProcess()
{
  // 接受本地裝置的動作
  m_Keyboard.Acquire(TRUE);  
  m_Mouse.Acquire(TRUE);     
  m_Keyboard.Read();
  m_Mouse.Read();

  if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
    return FALSE;

  	if (m_gs3D.m_RayTracing_Lighting)	m_gs3D.RTRender();
	m_gs3D.m_RayTracing_Lighting = FALSE;

/*	m_gsD3DX.Clear();
	if (m_gsD3DX.BeginScene()==TRUE){
		// CG -------------------------
		if (m_gs2D.m_Bresenham) Bresenham();					// 啟動畫線
		if (m_gs2D.m_DrawPolygon2D) DrawPolygon2D();			// 啟動畫多邊形
		if (m_gs2D.m_Cohen_Sutherland) Cohen_Sutherland();
		if (m_gs2D.m_Bezier_Pierre) BezierPireer(); 
		if (m_gs2D.m_Bezier_DeCasteljau) BezierDeCasteljau();

		if (m_gsMesh.m_Load_mesh)	m_gsMesh.RenderMesh(&m_gsD3DX);
	
		if (m_gs3D.m_RayTracing_Lighting)	m_gs3D.RTRender();
		
		CursorCoordinate();		// 繪出cursor座標
		m_gsD3DX.EndScene();
	}
	m_gsD3DX.Display();*/
  return TRUE;
}

void GSMain::BezierDeCasteljau(){
	if (m_Mouse.GetButtonState(0)){
		if (CountClick<4){
			m_gs2D.DrawPixel(m_Mouse.m_XPos,m_Mouse.m_YPos,RGB(255,255,0));
			ctrlPts[CountClick].h = (double)m_Mouse.m_XPos;
			ctrlPts[CountClick].v = (double)m_Mouse.m_YPos;
			CountClick++;
			Sleep(300);
		}else MB("最多四點");
	}
// sPoint2D &bezier,COLORREF color
	if (m_Mouse.GetButtonState(1)){
		for (int i=0;i<CountClick;i++)
		if ((ctrlPts[i].h + 5) >= m_Mouse.m_XPos &&  \
			(ctrlPts[i].v + 5) >= m_Mouse.m_YPos &&
			(ctrlPts[i].h - 5) <= m_Mouse.m_XPos &&  \
			(ctrlPts[i].v - 5) <= m_Mouse.m_YPos){
			ctrlPts[i].h = m_Mouse.m_XPos;
			ctrlPts[i].v = m_Mouse.m_YPos;
			m_gs2D.DrawBezierDeCasteljau(ctrlPts,RGB(255,0,0));
		}
	}

    if (CountClick==4)
			m_gs2D.DrawBezierDeCasteljau(ctrlPts,RGB(255,0,0));
	for (int i=0;i<CountClick;i++)
		m_gs2D.DrawNostril((int)ctrlPts[i].h,(int)ctrlPts[i].v);
}

void GSMain::BezierPireer()
{
	if (m_Mouse.GetButtonState(0)){
		m_gs2D.DrawPixel(m_Mouse.m_XPos,m_Mouse.m_YPos,RGB(255,255,0));
		ctrlPts[CountClick].h = (double)m_Mouse.m_XPos;
		ctrlPts[CountClick].v = (double)m_Mouse.m_YPos;
		CountClick++;
		Sleep(300);
	}

	if (m_Mouse.GetButtonState(1)){
		for (int i=0;i<CountClick;i++)
		if ((ctrlPts[i].h + 5) >= m_Mouse.m_XPos &&  \
			(ctrlPts[i].v + 5) >= m_Mouse.m_YPos &&
			(ctrlPts[i].h - 5) <= m_Mouse.m_XPos &&  \
			(ctrlPts[i].v - 5) <= m_Mouse.m_YPos){
			ctrlPts[i].h = m_Mouse.m_XPos;
			ctrlPts[i].v = m_Mouse.m_YPos;
			m_gs2D.DrawBezierPierre(CountClick,ctrlPts,1000);
		}
	}

    m_gs2D.DrawBezierPierre(CountClick,ctrlPts,1000);

	for (int i=0;i<CountClick;i++)
		m_gs2D.DrawNostril((int)ctrlPts[i].h,(int)ctrlPts[i].v);
    
}

void GSMain::Cohen_Sutherland(){
	if (m_Mouse.GetButtonState(0)){
		m_gs2D.DrawPixel(m_Mouse.m_XPos,m_Mouse.m_YPos,RGB(255,255,0));
		m_gs2D.Alloc(m_Mouse.m_XPos,m_Mouse.m_YPos,m_vertex);
		Sleep(300);
	}

	if (m_Mouse.GetButtonState(1)){
		m_vertex = m_vertex->link;
		while(m_vertex->link){
			if (m_gs2D.Cohen_Sutherland(m_vertex->x,m_vertex->y,\
				m_vertex->link->x,m_vertex->link->y))
			m_gs2D.Line(m_vertex->x,m_vertex->y, \
				m_vertex->link->x,m_vertex->link->y,RGB(255,255,0));	

			m_vertex = m_vertex->link;
		}
		Sleep(1000);
	}
}

void GSMain::DrawPolygon2D(){
	if (m_Mouse.GetButtonState(0)){
		m_gs2D.DrawNostril(m_Mouse.m_XPos,m_Mouse.m_YPos);
		m_gs2D.Alloc(m_Mouse.m_XPos,m_Mouse.m_YPos,m_vertex);
		CountClick++;
		Sleep(300);
	}
	if (m_Mouse.GetButtonState(1)){
		m_Polygon2D.num_verts = CountClick;
		m_Polygon2D.color = RGB(255,0,0);
		m_Polygon2D.vlist = new vertex2D[m_Polygon2D.num_verts];
		m_vertex = m_vertex->link;
		while(CountClick){
			m_Polygon2D.vlist[CountClick].x = m_vertex->x;	
			m_Polygon2D.vlist[CountClick].y = m_vertex->y;	
			m_vertex = m_vertex->link;
			CountClick--;
		}
		
		m_gs2D.Polygon2D(&m_Polygon2D);
		Sleep(1000);
	}
}

void GSMain::Bresenham(){


	if (m_Mouse.GetButtonState(0)){
		m_gs2D.DrawNostril(m_Mouse.m_XPos,m_Mouse.m_YPos);
		m_gs2D.Alloc(m_Mouse.m_XPos,m_Mouse.m_YPos,m_vertex);
		Sleep(300);
	}

	if (m_Mouse.GetButtonState(1)){
		
		m_vertex = m_vertex->link;
		while(m_vertex->link){
			m_gs2D.Line(m_vertex->x,m_vertex->y, \
				m_vertex->link->x,m_vertex->link->y,RGB(255,255,0));	

		m_vertex = m_vertex->link;
		}
		Sleep(1000);
	}
}

void GSMain::ClearUp(){

	CountClick = 0;				// 計算 選取點數

	// 2d
	m_gs2D.m_Bresenham = FALSE;
	m_gs2D.m_DrawPolygon2D = FALSE;
	m_gs2D.m_Cohen_Sutherland = FALSE;
	m_gs2D.m_Bezier_Pierre = FALSE;
	m_gs2D.m_Bezier_DeCasteljau = FALSE;

	// mesh
	m_gsMesh.m_Load_mesh	= FALSE;

	// 3d
	m_gs3D.m_RayTracing_Lighting = FALSE;
	
}

void GSMain::CursorCoordinate(){
  char buffer[32];
  sprintf(buffer,"Mouse (X,Y) = (%d,%d)      ",m_Mouse.m_XPos,m_Mouse.m_YPos);
  m_gsFont.Print(buffer,0,0);
}

BOOL Load()
{
  g_ofn.lpstrFile   = xFile;
  g_ofn.lpstrTitle  = "Load .x File";
  g_ofn.lpstrFilter = "DirectX .x Files (*.x)\0*.x\0All Files (*.*)\0*.*\0\0";
  g_ofn.lpstrDefExt = "Load .x File";

  // Ask for filename
  if(!GetOpenFileName(&g_ofn))
    return FALSE;

  return TRUE;
}

BOOL CALLBACK LoadXFile(HWND hWnd, UINT uMsg,         \
                                WPARAM wParam, LPARAM lParam){

	 switch(uMsg) {
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hWnd, IDC_XFILEPATH),   "");
		return  TRUE;
		break;

	case WM_COMMAND:
	{
		switch(LOWORD(wParam)) {
			case IDC_LOAD :
				Load();
				SetWindowText(GetDlgItem(hWnd, IDC_XFILEPATH),xFile);
				return TRUE;
			break;
			case IDC_OK :
			GetWindowText(GetDlgItem(hWnd,IDC_XFILEPATH),xFile,MAX_PATH);
			EndDialog(hWnd, TRUE);
			return TRUE;
			break;
			case IDC_CLEAR:
				SetWindowText(GetDlgItem(hWnd, IDC_XFILEPATH),   "");
				return  TRUE;
				break;
			}
		}
	 case WM_CREATE:
		// Initialize the save/load dialog box info
		ZeroMemory(&g_ofn, sizeof(OPENFILENAME));
		 // Setup the open dialog info
		g_ofn.hwndOwner   = hWnd;
		g_ofn.lStructSize = sizeof(OPENFILENAME);
		g_ofn.nMaxFile = MAX_PATH;
		g_ofn.nMaxFileTitle = MAX_PATH;
		g_ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;

		// Set default .x filename
		strcpy(xFile, "tiger.x");

      break;
	}
		
	return  FALSE;
}

FAR PASCAL GSMain::MsgProc(HWND hWnd, UINT uMsg,                \
                         WPARAM wParam, LPARAM lParam)
{
  switch(uMsg) {
	case WM_COMMAND:
         {
          switch(LOWORD(wParam))
                {
				// handle the FILE menu
                //----------------------------------
				// CG
				case MENU_SCREEN_ID_OPEN:{
					ClearUp();
                     m_gs2D.m_Bresenham =TRUE;
                } break;
				case MENU_SCREEN_ID_EXIT:{
					ClearUp();
                     PostQuitMessage(0);
                } break;
				case GS2D_ID_Bresenham:{
					ClearUp();
                     m_gs2D.m_Bresenham =TRUE;
                } break;

				case GS2D_ID_Polygon2D:{
					ClearUp();
					m_gs2D.m_DrawPolygon2D = TRUE;
				}break;

				case GS2D_ID_CSCLIP:{
					ClearUp();
                     m_gs2D.m_Cohen_Sutherland =TRUE;
                } break;

				case GS2D_ID_Bezier_Pierre:{
					ClearUp();
                     m_gs2D.m_Bezier_Pierre =TRUE;
                } break;

				case GS2D_ID_Bezier_DeCasteljau:{
					ClearUp();
                     m_gs2D.m_Bezier_DeCasteljau =TRUE;
                } break;

				case GSXFile_ID_Load:{
					ClearUp();
					int Result = DialogBox(GethInst(), MAKEINTRESOURCE(IDD_LoadXFile),  \
                    hWnd, LoadXFile);
					 // mesh
					m_gsMesh.Create(&m_gsD3DX,xFile) ;
					m_gsMesh.m_Load_mesh = TRUE;
					// load the .x file
				
                } break;

				case GSRT_ID_LIGHTING :{
					ClearUp();
					m_gs3D.m_RayTracing_Lighting = TRUE;
				}break;


                default: break;
             } 
          } break; // end WM_COMMAND


    case WM_DESTROY:
      PostQuitMessage(0);
      break;
    default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  return 0;
} 

// 程式進入點
int PASCAL WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR szCmdLine,int nShowCmd)
{
	GSMain app;
	return app.Run();
}
