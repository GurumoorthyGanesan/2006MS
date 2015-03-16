
//////////////////////////////////////////////////////////////////
//
//
/*************************************											
//	繪圖引擎, DXGraphics_Engine.h - v0.1	
//    author : Y.Y.L Date : 02/27/03
//************************************/
//-----------------------------------------------------------------


#ifndef _GSDXGraphic_H_
#define _GSDXGraphic_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }


class GSDXGraphic{

private:
	HWND					m_hWnd;						// 主視窗的 handle
	IDirect3D9				*m_pD3D;					// D3D 物件
	IDirect3DDevice9		*m_pD3DDevice;				// Device 物件
	ID3DXSprite				*m_pSprite;					// 2-D sprite 物件

	D3DDISPLAYMODE			m_d3ddm;					// 顯示模式的屬性

	BOOL							m_Windowed;			// 用來control 視窗的模式
	BOOL							m_ZBuffer;			// 用於zbuffer
	BOOL							m_HAL;				// 硬體加速

	long							m_Width;	
	long							m_Height;
	long							m_BPP;
public:

// Lifetime
	GSDXGraphic();
	~GSDXGraphic();

	BOOL				Init();
	BOOL				Shutdown();
// Query

	// 去收回COM 的 interfaces
	inline IDirect3D9				*GetDirect3DCOM	(void ) const	 {return m_pD3D;}			
	inline IDirect3DDevice9			*GetDeviceCOM	(void ) const  {return m_pD3DDevice;}
	inline ID3DXSprite				*GetSpriteCOM	(void ) const  { return m_pSprite;}
	// Query/bpp/HAL/ZBuffer
	inline long	GetWidth	(void ) const	{return m_Width;}
	inline long	GetHeight	(void ) const	{return m_Height;}
	inline char GetBPP		(void ) const	{return m_BPP;}
	inline BOOL	GetHAL		(void ) const	{return m_HAL;}
	inline BOOL GetZBuffer	(void ) const	{return m_ZBuffer;}

	// 用於初始顯示時的特別屬性設定
	BOOL		SetMode(HWND hWnd, BOOL Windowed = TRUE,
						BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0,
						char BPP	= 0);

	// 取得支援多少種模式與其資訊
	long		GetNumDisplayModes(D3DFORMAT Format);
	BOOL		GetDisplayModeInfo(D3DFORMAT Format,long Num, D3DDISPLAYMODE *Mode);
	char		GetFormatBPP(D3DFORMAT Format);			// 取得BPP
	BOOL		CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);	// 判斷是否支援硬碟加速
	BOOL		SetPerspective(float FOV, float Aspect, float Near, float Far);

	BOOL		BeginScene();
	BOOL		EndScene();
	BOOL		BeginSprite();
	BOOL		EndSprite();
	BOOL		Display();													// flip

	// 清除顯示/zbuffer
	BOOL		Clear(long Color = 0,float ZBuffer = 1.0f);		// 混合
	BOOL		ClearDisplay(COLORREF Color = 0);									// COLOR
	BOOL		ClearZBuffer(float ZBuffer = 1.0f);						// ZBUFFER
	
};

// 頂點緩衝
class GSDXVertexBuffer
{
private:
	GSDXGraphic									*m_Graphics;
	IDirect3DVertexBuffer9						*m_pVB;							// Vertex buffer COM
	IDirect3DVertexShader9						*m_pDisShader;

	DWORD											m_NumVertices;			// Vertices 數目
	DWORD											m_VertexSize;				// vertex大小
	DWORD											m_FVF;							// FVF描述

	BOOL											m_Locked;						// check 是否鎖定buffer
	char											*m_Ptr;							// 指向buffer

public:
	GSDXVertexBuffer();
	~GSDXVertexBuffer();

	// 用來取得COM,SIZE,FVF和vertices的個數
	IDirect3DVertexBuffer9			*GetVertexBufferCOM();
	unsigned	long							GetVertexSize();
	unsigned	long							GetVertexFVF();
	unsigned	long							GetNumVertices();
	void							SetNumVertices(unsigned	long n);

	// 建立/釋放vertex buffer
	BOOL				Create(GSDXGraphic *Graphics, unsigned long NumVertices,
									DWORD	Descriptor, long VertexSize);
	BOOL				Free();

	BOOL				IsLoaded();				// 用來判定是否配置buffer

	// 複製一系列的vertices到buffer
	BOOL				Set(unsigned long FirstVertex,
									unsigned long NumVertices,void *VertexList);
	BOOL				Render(unsigned long FirstVertex,unsigned long NumPrimitives,
									DWORD	Type);
	// lock / unlock a vertex buffer
	BOOL				Lock(unsigned long FirstVertex = 0,unsigned long NumVertices = 0);
	BOOL				Unlock();

	void				*GetPtr();		// 回傳被lock的vertexbuffer指標

};

// 字型
class GSDXFont
{
  private:
    ID3DXFont *m_Font;				// 字型物件

  public:
    GSDXFont();
    ~GSDXFont();

    ID3DXFont *GetFontCOM();		// 回傳字型 COM

		// 建立/釋放字型
    BOOL Create(GSDXGraphic *Graphics, char *Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();

		// 連續的字型動作 
    BOOL Begin();
    BOOL End();
    BOOL Print(char *Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);
};


// x mesh
class GSDXMesh
{
  private:
	LPD3DXMESH              m_pMesh; // Our mesh object in sysmem
	D3DMATERIAL9*           m_pMeshMaterials; // Materials for our mesh
	LPDIRECT3DTEXTURE9*     m_pMeshTextures; // Textures for our mesh
	DWORD                   m_dwNumMaterials;   // Number of mesh materials

  public:
    GSDXMesh();
    ~GSDXMesh();

    BOOL Create(GSDXGraphic *Graphics,char *xFile);
    BOOL Free();
	VOID Cleanup();
	VOID RenderMesh(GSDXGraphic *Graphics);
	BOOL	        m_Load_mesh;
};

#endif