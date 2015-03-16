//////////////////////////////////////////////////////////////////
//
//
/*************************************													
//	2D engine, GS2D.h - v0.1	
//  author : Y.Y.L Date : 09/27/04
/*************************************/
//-----------------------------------------------------------------

#ifndef _GS2D_H
#define _GS2D_H


#define RED			1
#define ORANGE		2
#define YELLOW		3
#define GREEN		4
#define BLUE		5
#define CYAN		6
#define PURPLE		7

#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001
#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

#define MAXVERT			10     // maximum number of control points
#define NostrilWidth	3	   // maximum number of setpixel width


class GS2D{
protected:
	HWND hWnd;
	HINSTANCE hInst;

private :
	int				CountClick;
	int				min_clip_x,min_clip_y,max_clip_x,max_clip_y;
	vertex2D		*m_vertex,*ptr;			// 輔助用


//	int numcpts; 
//	Point cpts[ 1+MAXVERT ]; 

public :
	GS2D();
	~GS2D();


	BOOL Init(HWND hWnd, HINSTANCE hInst);
	void Line(int xa,int ya,int xb,int yb,COLORREF col);

	void MyMidpointLineE(int xa,int ya,int xb,int yb,COLORREF col);
	void MyMidpointLineSE(int xa,int ya,int xb,int yb,COLORREF col);
	void MyMidpointLineSW(int xa,int ya,int xb,int yb,COLORREF col);
	void MyMidpointLineW(int xa,int ya,int xb,int yb,COLORREF col);
	

	void DrawPixel(int x,int y,COLORREF col);
	void DrawNostril(int x,int y,COLORREF col=RGB(255,255,0));
	void Alloc(int x,int y,vertex2D *e);
	void Polygon2D(sPolygon2D *poly);

	bool Cohen_Sutherland(int &xa,int &ya,int &xb,int &yb);
	
	//
	// Curver
	//
	// Pierre -------------------
	void binomialCoeffs(int n,int *c);
	void computeBezPt(double u, sPoint2D *bezPt,int nCtrlPoint,sPoint2D *CtrlPoint,int *c);
	void DrawBezierPierre(int n,sPoint2D *ctrlPts,int nBezCurPts);

	// DeCasteljau --------------
	void middle(sPoint2D &p0, sPoint2D &p1, sPoint2D &p01);
	void DrawBezierDeCasteljau(sPoint2D *bezier,COLORREF color);
	void recursive_bezier(sPoint2D &p1,sPoint2D &h1,sPoint2D &h2,sPoint2D &p2, COLORREF color);


	// 2D 繪圖開關
	BOOL			m_Bresenham;
	BOOL			m_DrawPolygon2D;
	BOOL			m_Cohen_Sutherland;
	BOOL			m_Bezier_Pierre;
	BOOL	        m_Bezier_DeCasteljau;
};
#endif 