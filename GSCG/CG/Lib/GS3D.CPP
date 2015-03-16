

#include "..\..\Common\Global.h"

// Screen data
char s_data[4*SCREEN_WIDTH*SCREEN_WIDTH];

///////////////////////////////////////////////////////////////////////
// 
///////////////////////////////////////////////////////////////////////
void CCamera::Update()
{
    D3DXMatrixLookAtLH( &m_mView, &m_vFrom, &m_vLookat, &m_vUp );
    D3DXMatrixPerspectiveFovLH( &m_mProj, m_fFovy, m_fAspect, m_fNear, m_fFar );
	m_mVP = m_mView * m_mProj;

	D3DXMatrixInverse( &m_mView_Inv, NULL, &m_mView);
	D3DXMatrixInverse( &m_mVP_Inv, NULL, &m_mVP);
}

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
GS3D::GS3D(){
};
GS3D::~GS3D(){
};

BOOL GS3D::Init(HWND thWnd, HINSTANCE thInst){
	hWnd = thWnd;
	hInst = thInst;

	BG_COLOR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return TRUE;
}

VOID GS3D::ShutDown(){

}

// --------------------------
// ray tracing 所需要的物件
// --------------------------
VOID GS3D::CreateObject(){

//------------------------------------------------------------
// 自訂物件
//------------------------------------------------------------
// 球
OBJ_DATA sphere_data[] = {
{
	OBJ_TYPE_SPHERE,
	{
		{0.01f, 0.05f, 0.02f },
		{0.01f, 0.05f, 0.02f },
		{0.02f, 0.02f, 0.02f },
		8.0f,
		0.3f, 0.0f, 5.5f,
	},{{0,0,0},{0,0,0},{0,0,0}},{
		{40.0, 100, 350},
		60.0f,
	}
},{
	OBJ_TYPE_SPHERE,
	{
		{0.2f, 0.2f, 0.2f},
		{0.2f, 0.2f, 0.2f},
		{0.05f, 0.05f, 0.05f },
		8.0f,
		2.01f, 0.00f, 0.85f,
	},{{0,0,0},{0,0,0},{0,0,0}},{
		{160.5, 50, 300},
		40.0f,
	}
},{
	OBJ_TYPE_SPHERE,
	{
		{0.02f, 0.03f, 0.03f},
		{0.02f, 0.03f, 0.03f},
		{0.1f, 0.1f, 0.1f },
		8.0f,
		0.15f, 0.00f, 0.9f,
	},{{0,0,0},{0,0,0},{0,0,0}},{
		{280.5, 50, 300},
		40.0f,
	}
},{
	OBJ_TYPE_SPHERE,
	{
		{0.02f, 0.02f, 0.02f},
		{0.02f, 0.2f, 0.2f},
		{0.10f, 0.10f, 0.10f },
		8.0f,
		0.05f, 0.00f, 5.0f,
	},{{0,0,0},{0,0,0},{0,0,0}},{
		{420.5, 100, 150},
		80.0f,
	}
}};

int i,j,f=0,t=0,h=110,v=190;
OBJ_DATA floor_data[30];

for (i=0;i<570;i+=190)
for (j=0;j<550;j+=110){
	// 第一塊
	floor_data[t].type = OBJ_TYPE_TRIANGLE;
	floor_data[t].material.COLOR_AMB[0]= 0.05f;
	floor_data[t].material.COLOR_AMB[1]= 0.01f;
	floor_data[t].material.COLOR_AMB[2]= 0.01f;
	floor_data[t].material.COLOR_DIF[0]= 0.05f;
	floor_data[t].material.COLOR_DIF[1]= 0.01f;
	floor_data[t].material.COLOR_DIF[2]= 0.01f;
	floor_data[t].material.COLOR_SPE[0]= f;
	floor_data[t].material.COLOR_SPE[1]= f;
	floor_data[t].material.COLOR_SPE[2]= f;
	floor_data[t].material.reflection = 0.3;
	floor_data[t].material.refraction = 0.0;
	floor_data[t].material.diffuse    = 5.7;
	floor_data[t].material.speq_power=	32.0f;

	// 順時針
	floor_data[t].triangle.x0[0] = j;
	floor_data[t].triangle.x0[1] = 0.0f;
	floor_data[t].triangle.x0[2] = i;

	floor_data[t].triangle.x1[0] = j+h;
	floor_data[t].triangle.x1[1] = 0.0f;
	floor_data[t].triangle.x1[2] = i;

	floor_data[t].triangle.x2[0] = j;
	floor_data[t].triangle.x2[1] = 0.0f;
	floor_data[t].triangle.x2[2] = i+v;

	t++;
	// 第二塊
	floor_data[t].type = OBJ_TYPE_TRIANGLE;
	floor_data[t].material.COLOR_AMB[0]= 0.02f;
	floor_data[t].material.COLOR_AMB[1]= 0.02f;
	floor_data[t].material.COLOR_AMB[2]= 0.05f;
	floor_data[t].material.COLOR_DIF[0]= 0.02f;
	floor_data[t].material.COLOR_DIF[1]= 0.02f;
	floor_data[t].material.COLOR_DIF[2]= 0.05f;
	floor_data[t].material.COLOR_SPE[0]= f;
	floor_data[t].material.COLOR_SPE[1]= f;
	floor_data[t].material.COLOR_SPE[2]= f;
	floor_data[t].material.reflection = 0.1;
	floor_data[t].material.refraction = 0.0;
	floor_data[t].material.diffuse    = 5.3;
	floor_data[t].material.speq_power=	32.0f;

	// 順時針
	floor_data[t].triangle.x0[0] = j;
	floor_data[t].triangle.x0[1] = 0.0f;
	floor_data[t].triangle.x0[2] = i+v;

	floor_data[t].triangle.x1[0] = j+h;
	floor_data[t].triangle.x1[1] = 0.0f;
	floor_data[t].triangle.x1[2] = i;

	floor_data[t].triangle.x2[0] = j+h;
	floor_data[t].triangle.x2[1] = 0.0f;
	floor_data[t].triangle.x2[2] = i+v;

	t++;
	f=!f;
}
	// 像機參數設定
	camera.SetFrom  (&D3DXVECTOR3(100,200,-600));
	camera.SetLookAt(&D3DXVECTOR3(200,200,0));
	camera.SetUp    (&D3DXVECTOR3(0,1,0));
	camera.SetFovY  (D3DX_PI/4);
	camera.SetAspect(1.0f);
	camera.SetNear  (0.01f);
	camera.SetFar   (500.0f);
	camera.Update();

	// 宣告物件 -----------------------------
	for(i=0;i<4;i++)
		m_sphere[i].Init(&sphere_data[i]);
	for(i=0;i<30;i++)
		m_floor[i].Init(&floor_data[i]);


	
} // end of CreateObject 

// ---------------------------------------------------------------------------
void GS3D::RTRender()
{
	CreateObject();
	HDC hdc = GetDC(hWnd);
	for(int j=0;j<SCREEN_HEIGHT;j++){
	for(int i=0;i<SCREEN_WIDTH ;i++){
		D3DXVECTOR3 col;
		GetColor(&col, ((float)i+0.5f)/(float)SCREEN_WIDTH  \
					 , ((float)j+0.5f)/(float)SCREEN_HEIGHT);
		s_data[4*(j*SCREEN_WIDTH+i)+0]=(char)(255.9*min(1,col.x));// R
		s_data[4*(j*SCREEN_WIDTH+i)+1]=(char)(255.9*min(1,col.y));// G
		s_data[4*(j*SCREEN_WIDTH+i)+2]=(char)(255.9*min(1,col.z));// B

		SetPixel(hdc,i,j,RGB(s_data[4*(j*SCREEN_WIDTH+i)+0],s_data[4*(j*SCREEN_WIDTH+i)+1],s_data[4*(j*SCREEN_WIDTH+i)+2]));

	}
	ReleaseDC(hWnd, hdc);
	}
}

// ---------------------------------------------------------------------------
bool GS3D::GetColor(D3DXVECTOR3 *dest, D3DXVECTOR4 *x, D3DXVECTOR4 *v,int depth)
{
	D3DXVECTOR4 light_pos   = D3DXVECTOR4(278.f, 548.8f, 279.5f,1); //光源位置
	D3DXVECTOR3 light_color = D3DXVECTOR3( 10.f,   10.0f,   5.0f);  //光源color

	float ttmp;
	D3DXVECTOR4 ptmp,ntmp;
	int i;
	
	float t = CObject::INFINTY_DIST;	// 最適當距離
	D3DXVECTOR4 p;						// 最近切點
	D3DXVECTOR4 n,r;					// 切點的法向量
	CObject *pObj = NULL;

	// tracing level
	const int DEPTH_MAX = 3;
	if(DEPTH_MAX <= depth) {*dest = D3DXVECTOR3(0,0,0) ; return FALSE;}

	// 球面偵測
	for(i = 0; i<4; i++){
		ttmp = m_sphere[i].IsAcross(&ntmp, &ptmp, x, v);
		if(0<=ttmp && ttmp<t){
			t = ttmp;
			n = ntmp;
			p = ptmp;
			pObj = &m_sphere[i];
		}
	}
	// 地板
	for(i = 0; i<30; i++){
		ttmp = m_floor[i].IsAcross(&ntmp, &ptmp, x, v);
		if(0<=ttmp && ttmp<t){
			t = ttmp;
			n = ntmp;
			p = ptmp;
			pObj = &m_floor[i];
		}
	}

	if( NULL == pObj ){
		*dest = BG_COLOR;
		return TRUE;
	}
	// Phong model 反射計算顏色 
	// 計算 diffuse
	D3DXVECTOR3 diffuse_color;
	float cd = pObj->m_material.diffuse;

	if (0.0f < cd){
		D3DXVECTOR4 l = light_pos - p;
		float L2 = D3DXVec3Length((D3DXVECTOR3 *) &l);
		D3DXVec3Normalize((D3DXVECTOR3 *)&l, (D3DXVECTOR3 *)&l);

		D3DXVECTOR3 dir, H;
		// 視線的計算
		camera.GetFrom(&dir);
		dir = dir - *(D3DXVECTOR3 *)&p;
		D3DXVec3Normalize(&dir, &dir);
		// 一半矢量
		H = dir+*(D3DXVECTOR3 *)&l;
		D3DXVec3Normalize((D3DXVECTOR3 *)&H, (D3DXVECTOR3 *)&H);

		float LN = D3DXVec3Dot((D3DXVECTOR3 *)&l, (D3DXVECTOR3 *)&n);
		float HN = D3DXVec3Dot((D3DXVECTOR3 *)&H, (D3DXVECTOR3 *)&n);
		if(HN<0) HN=0;
		if(0 < LN){
			float d;
			for (i=0;i<4;i++){
				d = m_sphere[i].IsAcross(&n,&r,&p,&l);
				if (0.001 * L2 <= d && d < 0.99*L2)
					LN *=0.0f;
			}
		
			for (i=0;i<30;i++){
				d = m_floor[i].IsAcross(&n, &r, &p, &l);
				if(0.001 * L2 <= d && d < 0.99f*L2)
					LN*=0.0f;
			}
		}else{ LN = 0; }
		// resursive ray tracing (D3DXVECTOR3D)
		pObj->GetColor(&diffuse_color, LN, HN);
		// 光源的顏色的反映
		dest->x *= light_color.x;
		dest->y *= light_color.y;
		dest->z *= light_color.z;
		// 光的強度適合的補正
		*dest *= min(1.5f, 500000.0f/(10000.0f+L2)); // 根據距離的補正
		*dest *= min(1, l.y+0.1f);	// 把適合于光做成cosθ的函數
	}else
		diffuse_color = D3DXVECTOR3(0,0,0);

	float cr = pObj->m_material.reflection;
	D3DXVECTOR3 reflect_color;
	if(0.0f<cr){
		D3DXVECTOR4 r = *v - 2.0f*D3DXVec3Dot((D3DXVECTOR3 *)&n, (D3DXVECTOR3 *)v) * n;
		D3DXVECTOR4 pos = p + 0.01f*n;
		if(!GetColor( &reflect_color, &pos, &r, depth+1))
			cr = 0;
	}else
		reflect_color=D3DXVECTOR3(0,0,0);

	*dest = cd * diffuse_color + cr * reflect_color;
	return true;
}
// ---------------------------------------------------------------------------
D3DXVECTOR3* GS3D::GetColor(D3DXVECTOR3 *dest, float x, float y)
{
	D3DXVECTOR4 ray_start_proj = D3DXVECTOR4(-2*x+1, -2*y+1, 0, 1);	// 前方 FOV 平面
	D3DXVECTOR4 ray_eye        = D3DXVECTOR4(0, 0, 0, 1);			// 照相機的位置
	D3DXVECTOR4 ray_start;		// ray 起始點向量
	D3DXVECTOR4 ray_to;			// ray 朝向向量
	D3DXVECTOR4 ray_dir;		// ray 方向向量
	D3DXMATRIX mInv;
	
	// --- 從投影空間世界座標變換視點 ----
	// 推算ray 起始位置
	D3DXVec4Transform( &ray_start, &ray_eye,        camera.GetViewInverse() );
	// 推算ray 朝向
	D3DXVec4Transform( &ray_to,    &ray_start_proj, camera.GetViewProjInverse() );
	// w = 1 ,在投影空間上扔下(1 : 1)
	D3DXVec4Scale( &ray_to,    &ray_to,    1.0f/ray_to.w   ); 
	// ray 方向向量
	ray_dir = ray_to - ray_start;
	D3DXVec4Normalize(&ray_dir, &ray_dir); // normalize
	
	// 顏色計算
	GetColor(dest, &ray_start, &ray_dir,0);

	return dest;
}
