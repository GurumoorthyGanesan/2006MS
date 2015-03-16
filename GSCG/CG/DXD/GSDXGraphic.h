
//////////////////////////////////////////////////////////////////
//
//
/*************************************											
//	ø�Ϥ���, DXGraphics_Engine.h - v0.1	
//    author : Y.Y.L Date : 02/27/03
//************************************/
//-----------------------------------------------------------------


#ifndef _GSDXGraphic_H_
#define _GSDXGraphic_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }


class GSDXGraphic{

private:
	HWND					m_hWnd;						// �D������ handle
	IDirect3D9				*m_pD3D;					// D3D ����
	IDirect3DDevice9		*m_pD3DDevice;				// Device ����
	ID3DXSprite				*m_pSprite;					// 2-D sprite ����

	D3DDISPLAYMODE			m_d3ddm;					// ��ܼҦ����ݩ�

	BOOL							m_Windowed;			// �Ψ�control �������Ҧ�
	BOOL							m_ZBuffer;			// �Ω�zbuffer
	BOOL							m_HAL;				// �w��[�t

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

	// �h���^COM �� interfaces
	inline IDirect3D9				*GetDirect3DCOM	(void ) const	 {return m_pD3D;}			
	inline IDirect3DDevice9			*GetDeviceCOM	(void ) const  {return m_pD3DDevice;}
	inline ID3DXSprite				*GetSpriteCOM	(void ) const  { return m_pSprite;}
	// Query/bpp/HAL/ZBuffer
	inline long	GetWidth	(void ) const	{return m_Width;}
	inline long	GetHeight	(void ) const	{return m_Height;}
	inline char GetBPP		(void ) const	{return m_BPP;}
	inline BOOL	GetHAL		(void ) const	{return m_HAL;}
	inline BOOL GetZBuffer	(void ) const	{return m_ZBuffer;}

	// �Ω��l��ܮɪ��S�O�ݩʳ]�w
	BOOL		SetMode(HWND hWnd, BOOL Windowed = TRUE,
						BOOL UseZBuffer = FALSE, long Width = 0, long Height = 0,
						char BPP	= 0);

	// ���o�䴩�h�ֺؼҦ��P���T
	long		GetNumDisplayModes(D3DFORMAT Format);
	BOOL		GetDisplayModeInfo(D3DFORMAT Format,long Num, D3DDISPLAYMODE *Mode);
	char		GetFormatBPP(D3DFORMAT Format);			// ���oBPP
	BOOL		CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);	// �P�_�O�_�䴩�w�Х[�t
	BOOL		SetPerspective(float FOV, float Aspect, float Near, float Far);

	BOOL		BeginScene();
	BOOL		EndScene();
	BOOL		BeginSprite();
	BOOL		EndSprite();
	BOOL		Display();													// flip

	// �M�����/zbuffer
	BOOL		Clear(long Color = 0,float ZBuffer = 1.0f);		// �V�X
	BOOL		ClearDisplay(COLORREF Color = 0);									// COLOR
	BOOL		ClearZBuffer(float ZBuffer = 1.0f);						// ZBUFFER
	
};

// ���I�w��
class GSDXVertexBuffer
{
private:
	GSDXGraphic									*m_Graphics;
	IDirect3DVertexBuffer9						*m_pVB;							// Vertex buffer COM
	IDirect3DVertexShader9						*m_pDisShader;

	DWORD											m_NumVertices;			// Vertices �ƥ�
	DWORD											m_VertexSize;				// vertex�j�p
	DWORD											m_FVF;							// FVF�y�z

	BOOL											m_Locked;						// check �O�_��wbuffer
	char											*m_Ptr;							// ���Vbuffer

public:
	GSDXVertexBuffer();
	~GSDXVertexBuffer();

	// �ΨӨ��oCOM,SIZE,FVF�Mvertices���Ӽ�
	IDirect3DVertexBuffer9			*GetVertexBufferCOM();
	unsigned	long							GetVertexSize();
	unsigned	long							GetVertexFVF();
	unsigned	long							GetNumVertices();
	void							SetNumVertices(unsigned	long n);

	// �إ�/����vertex buffer
	BOOL				Create(GSDXGraphic *Graphics, unsigned long NumVertices,
									DWORD	Descriptor, long VertexSize);
	BOOL				Free();

	BOOL				IsLoaded();				// �ΨӧP�w�O�_�t�mbuffer

	// �ƻs�@�t�C��vertices��buffer
	BOOL				Set(unsigned long FirstVertex,
									unsigned long NumVertices,void *VertexList);
	BOOL				Render(unsigned long FirstVertex,unsigned long NumPrimitives,
									DWORD	Type);
	// lock / unlock a vertex buffer
	BOOL				Lock(unsigned long FirstVertex = 0,unsigned long NumVertices = 0);
	BOOL				Unlock();

	void				*GetPtr();		// �^�ǳQlock��vertexbuffer����

};

// �r��
class GSDXFont
{
  private:
    ID3DXFont *m_Font;				// �r������

  public:
    GSDXFont();
    ~GSDXFont();

    ID3DXFont *GetFontCOM();		// �^�Ǧr�� COM

		// �إ�/����r��
    BOOL Create(GSDXGraphic *Graphics, char *Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);
    BOOL Free();

		// �s�򪺦r���ʧ@ 
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