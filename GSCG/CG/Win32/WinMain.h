//////////////////////////////////////////////////////////////////
//
//
/*************************************									
//	主程式WinMain, WinMain.h - v0.1	
//    author : Y.Y.L Date : 02/27/03
//************************************/
//-----------------------------------------------------------------

#ifndef GS_GSMain
#define GS_GSMain

#include "..\..\Common\Global.h"

///////////////////////////////////////////////////////////
// GSMain
//
//
class GSMain: public GSSysMgt
{
private:
	int				CountClick;
	GS2D			m_gs2D;
	GS3D			m_gs3D;
	vertex2D		*m_vertex,*ptr;			// 輔助用
	sPolygon2D		m_Polygon2D;
    sPoint2D		ctrlPts[100];
	CUSTOMVERTEX4F  *m_CVBP,*m_CVBP_Ptr;

	// Direct Graphic Hardware 
	GSDXGraphic			m_gsD3DX;
	GSDXVertexBuffer	m_gsVertexBuffer;
	GSDXFont			m_gsFont;
	GSDXMesh			m_gsMesh;

	// 裝置系統物件
    cInput          m_Input;
    cInputDevice    m_Keyboard;
	cInputDevice    m_Mouse;

	// 圖形介面
	GSDXGraphic		m_Dxg;

	BOOL Initialize();          // 初始相關設定
 
public:
	GSMain();
	~GSMain();

    

    FAR PASCAL MsgProc(HWND hWnd, UINT uMsg,                  \
                       WPARAM wParam, LPARAM lParam);

	BOOL Init();						// 過載 Init()
	BOOL FrameProcess();		// 過載 FrameProcess()
	BOOL Shutdown();				// 過載 Shutdown()
	void ClearUp();

	// 2D draw function
	void Bresenham();
	void DrawPolygon2D();
	void Cohen_Sutherland();
	void BezierPireer();
	void BezierDeCasteljau();
	void ReBezier(int index);
	void CursorCoordinate();

	// 3D draw function
	// --ray tracing --
	void RTLighting();

};


int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev,          \
                   LPSTR szCmdLine, int nCmdShow);
#endif