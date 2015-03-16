
#include "..\..\Common\Global.h"
#include <rmxftmpl.h>
#include <rmxfguid.h>

//	--------------------------------------------------------------------------------
//
//	Class of GSDXGraphic Functioins
//
//	--------------------------------------------------------------------------------
GSDXGraphic::GSDXGraphic()
{
	m_hWnd			= NULL;

	m_pD3D			= NULL;
	m_pD3DDevice	=	NULL;
	m_pSprite		=	NULL;

	m_Width		= 0;
	m_Height	= 0;
	m_BPP		= 0;

	m_Windowed	= TRUE;
	m_ZBuffer	= FALSE;
	m_HAL		= FALSE;

}

GSDXGraphic::~GSDXGraphic()
{ Shutdown();	}

BOOL GSDXGraphic::Init()
{
  Shutdown();
	if((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
			return FALSE;
	return TRUE;
}

BOOL GSDXGraphic::SetMode(HWND hWnd,BOOL Windowed,BOOL UseZBuffer,long Width,long Height, char BPP)
{
	D3DPRESENT_PARAMETERS					d3dpp;
	D3DFORMAT								Format, AltFormat;
	RECT									WndRect,ClientRect;
	long									WndWidth,WndHeight;

	// 錯誤檢查
	if((m_hWnd = hWnd) == NULL)		return FALSE;
	if( m_pD3D == NULL )			return FALSE;

	// 取得目前顯示模式的格式
	if(FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))
		return FALSE;

	// 設定 Width
	if(!Width){
		if(Windowed == FALSE)							// 是否全螢幕
			m_Width = m_d3ddm.Width;
		else{
			GetClientRect(m_hWnd,&ClientRect);
			m_Width = ClientRect.right;}
	}
	else{	
		m_Width = Width;	
	}

	// 設定 Height
	if(!Height){
		if(Windowed == FALSE)
			m_Height = m_d3ddm.Height;
		else{
			GetClientRect(m_hWnd, &ClientRect);
			m_Height = ClientRect.bottom;}
	}
	else{ 
		m_Height = Height;	
	}

	// 設定 BPP
	if(!(m_BPP = BPP) || Windowed == TRUE){
		if(!(m_BPP = GetFormatBPP(m_d3ddm.Format)))
			return FALSE;	}

	// 調整client視窗
	if(Windowed == TRUE)
	{
		// Get the size of the window
		GetWindowRect(m_hWnd,&WndRect);
		GetClientRect(m_hWnd,&ClientRect);
		// Set to fullscreen
		//	SetWindowLong( m_hWnd, GWL_STYLE, WS_POPUP|WS_SYSMENU|WS_VISIBLE );
		WndWidth	= (WndRect.right	- (ClientRect.right  - m_Width))	- WndRect.left;
		WndHeight	=	(WndRect.bottom	- (ClientRect.bottom - m_Height))	- WndRect.top;
		MoveWindow(m_hWnd,WndRect.left,WndRect.top,WndWidth,WndHeight,TRUE);
	}
	
	m_HAL = FALSE;				//	先初始硬體加速為false
	ZeroMemory(&d3dpp,sizeof(d3dpp));

	// 設定為視窗/全螢幕
	if((m_Windowed = Windowed) == TRUE)
	{
		d3dpp.Windowed					=	TRUE;
		d3dpp.SwapEffect				=	D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat			=	m_d3ddm.Format;
		
		if(CheckFormat(m_d3ddm.Format,TRUE,TRUE) == TRUE)		
			m_HAL		=		TRUE; 
		else
			if(CheckFormat(m_d3ddm.Format,TRUE,FALSE) == FALSE)
				return FALSE;
	}
	else
	{
		d3dpp.Windowed		=	FALSE;
		d3dpp.SwapEffect	= D3DSWAPEFFECT_FLIP;

		d3dpp.BackBufferWidth		=	m_Width;
		d3dpp.BackBufferHeight		=	m_Height;
													
		d3dpp.FullScreen_RefreshRateInHz		=	D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval		=	D3DPRESENT_INTERVAL_DEFAULT;	
		// or D3DPRESENT_INTERVAL_DEFAULT or D3DPRESENT_INTERVAL_IMMEDIATE

		// 計算顯示格式
		if(m_BPP == 32){
			Format		=	D3DFMT_X8R8G8B8;
			AltFormat	=	D3DFMT_X8R8G8B8;
		}else 	if(m_BPP == 24){
			Format		=	D3DFMT_R8G8B8;
			AltFormat	= D3DFMT_R8G8B8;
		}else		if(m_BPP == 16){
			Format		=	D3DFMT_R5G6B5;
			AltFormat	= D3DFMT_X1R5G5B5;
		}else		if(m_BPP == 8){
			Format		= D3DFMT_P8;
			AltFormat	= D3DFMT_P8;
		}

		// 檢查HAL裝置
		if(CheckFormat(Format,FALSE,TRUE) == TRUE)
			m_HAL	=	TRUE;
		else
			if(CheckFormat(AltFormat,FALSE,TRUE) == TRUE){
				m_HAL = TRUE;
				Format = AltFormat;}
			else
				if(CheckFormat(Format,FALSE,FALSE) == FALSE)
					if(CheckFormat(AltFormat,FALSE,FALSE) == FALSE)	
						return FALSE;
					else	
						Format = AltFormat;
			d3dpp.BackBufferFormat = Format;
	}
		
	// 設定 Zbuffer 格式 - 16bit
	if((m_ZBuffer = UseZBuffer) == TRUE)
	{
		d3dpp.EnableAutoDepthStencil	=	TRUE;
		d3dpp.AutoDepthStencilFormat	=	D3DFMT_D16;
	}else
		d3dpp.EnableAutoDepthStencil	= FALSE;

	// 建立D3DDevice物件
	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
		hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,&m_pD3DDevice)))
	{
		// 試著去建立沒有支援ZBuffer的物件
		if(m_ZBuffer == TRUE)
		{
			m_ZBuffer = FALSE;
			d3dpp.EnableAutoDepthStencil	= FALSE;

			if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
					(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
					hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,
					&d3dpp,&m_pD3DDevice)))
					return FALSE;
		}
		else
			return FALSE;
	}
	
	// 啟動紋理 rendering stages 並 filter 型態
	m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_pD3DDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_MODULATE);


	// 計算方位基本比例
	float Aspect = (float)m_Height / (float)m_Width;
	SetPerspective(D3DX_PI/4,Aspect,1.0f,1000.0f);

	
    // Turn on the zbuffer
    m_pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    // Turn on ambient lighting 
    m_pD3DDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );
	return TRUE;
}

// 釋放COM
BOOL GSDXGraphic::Shutdown()
{
  ReleaseCOM(m_pSprite);
  ReleaseCOM(m_pD3DDevice);
  ReleaseCOM(m_pD3D);

  return TRUE;
}

// 取得顯示卡的支援的個數
long GSDXGraphic::GetNumDisplayModes(D3DFORMAT Format)
{
  if(m_pD3D == NULL)
    return 0;

  return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,Format);
}

// 取得顯示模式的資訊
BOOL GSDXGraphic::GetDisplayModeInfo(D3DFORMAT Format,long Num, D3DDISPLAYMODE *Mode)
{
  long Max;

  if(m_pD3D == NULL)
    return FALSE;

  Max = GetNumDisplayModes(Format);
  if(Num >= Max)
    return FALSE;

  if(FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,Format,Num, Mode)))
    return FALSE;

  return TRUE;
}

// 取得所支援BPP的格式
char GSDXGraphic::GetFormatBPP(D3DFORMAT Format)
{
  switch(Format) {
		// 32 bit modes
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
      return 32;break;

    // 24 bit modes
    case D3DFMT_R8G8B8:
      return 24; break;

    // 16 bit modes
    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
      return 16; break;

    // 8 bit modes
    case D3DFMT_A8P8:
    case D3DFMT_P8:
      return 8; break;

    default:
       return 0;
  }
}

// 檢查是否支援硬體加速 
inline BOOL GSDXGraphic::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL)
{
  if(FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,
		(HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
		Format, Format,Windowed)))
		return FALSE;

	return TRUE;
}
 
// 啟動/關閉 Scene
BOOL GSDXGraphic::BeginScene()
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->BeginScene()))
    return FALSE;

  return TRUE;
}

BOOL GSDXGraphic::EndScene()
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->EndScene()))
    return FALSE;

  return TRUE;
}

// 啟動/關閉 sprite
BOOL GSDXGraphic::BeginSprite()
{
  if(m_pSprite == NULL)
    return FALSE;

  if(FAILED(m_pSprite->Begin()))
    return FALSE;

  return TRUE;
}

BOOL GSDXGraphic::EndSprite()
{
  if(m_pSprite == NULL)
    return FALSE;

  if(FAILED(m_pSprite->End()))
    return FALSE;

  return TRUE;
}

// 清除present
BOOL GSDXGraphic::Display()
{
  if(m_pD3DDevice == NULL)
    return FALSE;

  if(FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))
    return FALSE;

  return TRUE;
}

// 清除color & ZBuffer
BOOL GSDXGraphic::Clear(long Color,float ZBuffer)
{
	if(m_pD3DDevice == NULL)
		return FALSE;
	
	// 判斷ZBuffer是否為False，若是只清除Color部份
	if(m_ZBuffer == FALSE)
		return ClearDisplay(Color);
	
	// 清除Color/ZBuffer
	if(FAILED(m_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
					Color,ZBuffer,0)))
					return FALSE;

	return TRUE;
}

// 清除color部份
BOOL GSDXGraphic::ClearDisplay(COLORREF Color)
{
  if(m_pD3DDevice == NULL)
    return FALSE;

	if(FAILED(m_pD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,Color,1.0f,0)))
		return FALSE;

  return TRUE; 
}  

// 清除ZBuffer
BOOL GSDXGraphic::ClearZBuffer(float ZBuffer)
{
  if(m_pD3DDevice == NULL || m_ZBuffer == FALSE)
    return FALSE;

  if(FAILED(m_pD3DDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,ZBuffer,0)))
		return FALSE;

  return TRUE;
}

// 設定 perspective
BOOL GSDXGraphic::SetPerspective(float FOV, float Aspect, float Near, float Far)
{
  D3DXMATRIX matProjection;

  if(m_pD3DDevice == NULL)
    return FALSE;

   D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    m_pD3DDevice->SetTransform( D3DTS_VIEW, &matView );


  D3DXMatrixPerspectiveFovLH(&matProjection,FOV,Aspect,Near,Far);
	if(FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION,&matProjection)))
		return FALSE;

  return TRUE;
}

//	------------------------------------------------------------------------------
//
//	Class of VertexBuffer function
//
//	------------------------------------------------------------------------------

GSDXVertexBuffer::GSDXVertexBuffer()
{
	m_Graphics	= NULL;
	m_pVB				=	NULL;
	m_NumVertices	=	0;
	m_FVF				=	0;
	m_Locked		=	FALSE;
	m_Ptr				=	NULL;
}

GSDXVertexBuffer::~GSDXVertexBuffer()
{	Free();}

IDirect3DVertexBuffer9 *GSDXVertexBuffer::GetVertexBufferCOM()
{		return m_pVB;}

unsigned long GSDXVertexBuffer::GetVertexSize()
{	return D3DXGetFVFVertexSize(m_FVF);}

unsigned long GSDXVertexBuffer::GetNumVertices()
{	return m_NumVertices;}

void GSDXVertexBuffer::SetNumVertices(unsigned	long n)
{	m_NumVertices = n;}

BOOL GSDXVertexBuffer::Create(GSDXGraphic *Graphics,unsigned long NumVertices,DWORD Descriptor,long VertexSize)
{
	Free();

	if((m_Graphics = Graphics) == NULL)
		return FALSE;
	if((m_Graphics->GetDeviceCOM()) == NULL)
		return FALSE;
	// 判斷Vertices個數,FVF,vertexSize
	if(!(m_NumVertices = NumVertices) || !(m_FVF = Descriptor) || !(m_VertexSize = VertexSize))
		return FALSE;

	// 建立VertexBuffer
	if (FAILED(m_Graphics->GetDeviceCOM()->CreateVertexBuffer( m_NumVertices * m_VertexSize,0,m_FVF,
							D3DPOOL_DEFAULT, &m_pVB,NULL)))
		return FALSE;

	return TRUE;
}

BOOL GSDXVertexBuffer::Free()
{
  Unlock();
	ReleaseCOM(m_pVB);
	m_pDisShader = NULL;
	m_Graphics = NULL;
	m_NumVertices = 0;
	m_FVF			=	0;
	m_Locked	=	FALSE;
	m_Ptr			=	NULL;

	return TRUE;
}

BOOL GSDXVertexBuffer::Set(unsigned long FirstVertex,unsigned long NumVertices,void *VertexList)
{
	if(m_Graphics == NULL || VertexList == NULL || m_pVB == NULL)
		return FALSE;
	if(m_Graphics->GetDeviceCOM() ==	NULL)
		return FALSE;

	// lock the vertex buffer
	if(Lock(FirstVertex,NumVertices) == FALSE)
		return FALSE;

	// 複製vertex到vertex buffer
	memcpy(m_Ptr,VertexList,NumVertices * m_VertexSize);

	if(Unlock() == FALSE)
		return FALSE;

	return TRUE;
}

BOOL GSDXVertexBuffer::Render(unsigned long FirstVertex,unsigned long NumPrimitives,DWORD Type)
{
	if(m_Graphics->GetDeviceCOM() == NULL || m_pVB == NULL)
		return FALSE;

	// 設定 vertex buffer 給裝置資料Stream
	m_Graphics->GetDeviceCOM()->SetStreamSource(0,m_pVB,0,m_VertexSize);
	// 設定目前的vertex shader到先前建立的vertex buffer
	m_Graphics->GetDeviceCOM()->SetVertexShader(m_pDisShader);
	m_Graphics->GetDeviceCOM()->SetFVF( D3DFVF_CUSTOMVERTEX4F );

	// 產生多邊形
	m_Graphics->GetDeviceCOM()->DrawPrimitive((D3DPRIMITIVETYPE)Type,FirstVertex,NumPrimitives);

	return TRUE;
}

BOOL GSDXVertexBuffer::Lock(unsigned long FirstVertex,unsigned long NumVertices)
{
	if(m_pVB == NULL)
		return FALSE;
	if(FAILED(m_pVB->Lock( FirstVertex * m_VertexSize,
								NumVertices * m_VertexSize,(void**) &m_Ptr,
								D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD)))
		return FALSE;

	m_Locked = FALSE;

	return TRUE;
}

BOOL GSDXVertexBuffer::Unlock()
{
	if(m_pVB == NULL)
		return FALSE;
	if(FAILED(m_pVB->Unlock()))
		return FALSE;

	m_Locked = FALSE;

	return TRUE;
}

BOOL GSDXVertexBuffer::IsLoaded()
{	return (m_pVB == NULL ) ? FALSE : TRUE; }

void *GSDXVertexBuffer::GetPtr()
{	return (void*) m_Ptr;}

//	------------------------------------------------------------------------------
//
//  Class of GSDXFont Function
//
//	------------------------------------------------------------------------------
GSDXFont::GSDXFont()
{
  m_Font = NULL;
}

GSDXFont::~GSDXFont()
{
  Free();
}

ID3DXFont *GSDXFont::GetFontCOM()
{
  return m_Font;
}

BOOL GSDXFont::Free()
{
  ReleaseCOM(m_Font);
  return TRUE;
}

BOOL GSDXFont::Begin()
{
  if(m_Font == NULL)
    return FALSE;
  if(FAILED(m_Font->Begin()))
    return FALSE;
  return TRUE;
}

BOOL GSDXFont::End()
{
  if(m_Font == NULL)
    return FALSE;
  if(FAILED(m_Font->End()))
    return FALSE;
  return TRUE;
}

// 建立字型
BOOL GSDXFont::Create(GSDXGraphic *Graphics,char *Name,long Size, BOOL Bold,BOOL Italic, BOOL	Underline , BOOL Strikeout)
{
	LOGFONT	lf;

	if(Graphics == NULL || Name == NULL)
		return FALSE;
	if(Graphics->GetDeviceCOM() == NULL)
		return FALSE;

	// 清除font 結構
	ZeroMemory(&lf,sizeof(LOGFONT));

	// 設定字型名稱興高度
	strcpy(lf.lfFaceName,Name);
	lf.lfHeight		=		- Size;
	lf.lfWeight		=		(Bold == TRUE) ? 700 : 0;
	lf.lfItalic		=		Italic;
	lf.lfUnderline	=		Underline;
	lf.lfStrikeOut	=		Strikeout;

	//	建立字型物件
	if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(),&lf,&m_Font)))
		return FALSE;

	return TRUE;
}

// 列印字型物件
BOOL	GSDXFont::Print( char *Text,long XPos,long YPos,long Width,long Height ,D3DCOLOR Color, DWORD Format)
{
	RECT	Rect;

	if(m_Font == NULL)
		return FALSE;

	if(!Width)	Width = 65535;
	if(!Height) Height = 65536;

	Rect.left = XPos;
	Rect.top	=	YPos;
	Rect.right	=	Rect.left + Width;
	Rect.bottom = Rect.top	+	Height;
	if(FAILED(m_Font->DrawText(Text,-1,&Rect,Format,Color)))
		return FALSE;

	return TRUE;
}

//	------------------------------------------------------------------------------
//
//  Class of GSDXXFILE Function
//
//	------------------------------------------------------------------------------
GSDXMesh::GSDXMesh(){
	m_pMesh          = NULL; // Our mesh object in sysmem
	m_pMeshMaterials = NULL; // Materials for our mesh
	m_pMeshTextures  = NULL; // Textures for our mesh
	m_dwNumMaterials = 0L;   // Number of mesh materials
}

GSDXMesh::~GSDXMesh(){
  Free();
  Cleanup();
}


BOOL GSDXMesh::Free(){
  return TRUE;
}

BOOL GSDXMesh::Create(GSDXGraphic *Graphics,char *xFile){
	LPD3DXBUFFER m_D3DXMtrlBuffer;
	
//	char xFile[64] = "data\\tiger.x";
	if(Graphics == NULL || xFile == NULL)
		return FALSE;
    // Load the mesh from the specified file
    if( FAILED( D3DXLoadMeshFromX( xFile, D3DXMESH_SYSTEMMEM, 
                                   Graphics->GetDeviceCOM(), NULL, 
                                   &m_D3DXMtrlBuffer, NULL, &m_dwNumMaterials, 
                                   &m_pMesh ) ) ){
       MB(xFile);
       return FALSE;
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)m_D3DXMtrlBuffer->GetBufferPointer();
    m_pMeshMaterials = new D3DMATERIAL9[m_dwNumMaterials];
    m_pMeshTextures  = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];

    for( DWORD i=0; i<m_dwNumMaterials; i++ )
    {
        // Copy the material
        m_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

        // Set the ambient color for the material (D3DX does not do this)
        m_pMeshMaterials[i].Ambient = m_pMeshMaterials[i].Diffuse;

        m_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL && 
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFile( Graphics->GetDeviceCOM(), 
                                                d3dxMaterials[i].pTextureFilename, 
                                                &m_pMeshTextures[i] ) ) )
            {
                // If texture is not in current folder, try parent folder
                const TCHAR* strPrefix = TEXT("data\\");
                const int lenPrefix = lstrlen( strPrefix );
                TCHAR strTexture[MAX_PATH];
                lstrcpyn( strTexture, strPrefix, MAX_PATH );
                lstrcpyn( strTexture + lenPrefix, d3dxMaterials[i].pTextureFilename, MAX_PATH - lenPrefix );
                // If texture is not in current folder, try parent folder
                if( FAILED( D3DXCreateTextureFromFile( Graphics->GetDeviceCOM(), 
                                                    strTexture, 
                                                    &m_pMeshTextures[i] ) ) ){
                    MB("Could not find texture map");
                }
            }
        }
    }

    // Done with the material buffer
    m_D3DXMtrlBuffer->Release();

    return TRUE;
}

//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID GSDXMesh::Cleanup()
{
    if( m_pMeshMaterials != NULL ) 
        delete[] m_pMeshMaterials;

    if( m_pMeshTextures )
    {
        for( DWORD i = 0; i < m_dwNumMaterials; i++ )
        {
            if( m_pMeshTextures[i] )
                m_pMeshTextures[i]->Release();
        }
        delete[] m_pMeshTextures;
    }
    if( m_pMesh != NULL )
        m_pMesh->Release();
}

VOID GSDXMesh::RenderMesh(GSDXGraphic *Graphics){

	D3DXMATRIXA16 matWorld;
    D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );
    Graphics->GetDeviceCOM()->SetTransform( D3DTS_WORLD, &matWorld );

    // Meshes are divided into subsets, one for each material. Render them in
    // a loop
    for( DWORD i=0; i<m_dwNumMaterials; i++ ){
      // Set the material and texture for this subset
      Graphics->GetDeviceCOM()->SetMaterial( &m_pMeshMaterials[i] );
      Graphics->GetDeviceCOM()->SetTexture( 0, m_pMeshTextures[i] );

	  // g_pd3dDevice->SetTexture( 0, m_pMeshTextures[i] );
        
      // Draw the mesh subset
      m_pMesh->DrawSubset( i );
    }
}
