//////////////////////////////////////////////////////////////////
//
//
/*************************************													
//	3D engine, GS3D.h - v0.1	
//  author : Y.Y.L Date : 12/10/04
/*************************************/
//-----------------------------------------------------------------

#ifndef _GS3D_H
#define _GS3D_H

class CCamera
{
private:
	D3DXMATRIX  m_mView;
	D3DXMATRIX  m_mProj;
	D3DXMATRIX  m_mVP;
	D3DXMATRIX  m_mVP_Inv;
	D3DXMATRIX  m_mView_Inv;

	D3DXVECTOR3 m_vFrom;
	D3DXVECTOR3 m_vLookat;
	D3DXVECTOR3 m_vUp;

	float	m_fFovy;
	float	m_fAspect;
	float   m_fNear;
	float   m_fFar;

public:

	void Update();
	void SetFrom  (const D3DXVECTOR3 *p){m_vFrom  =*p;}
	void SetLookAt(const D3DXVECTOR3 *p){m_vLookat=*p;}
	void SetUp    (const D3DXVECTOR3 *p){m_vUp    =*p;}
	void SetFovY  (float val){m_fFovy   = val;}
	void SetAspect(float val){m_fAspect = val;}
	void SetNear  (float val){m_fNear   = val;}
	void SetFar   (float val){m_fFar    = val;}

	inline const D3DXMATRIX *GetViewInverse() const {return &m_mView_Inv;}
	inline const D3DXMATRIX *GetViewProjInverse() const {return &m_mVP_Inv;}
	inline void GetFrom (D3DXVECTOR3 *dest) const {*dest = m_vFrom;}
};


class GS3D{

protected:
	HWND hWnd;
	HINSTANCE hInst;
private :

	CTriangle	m_room[10],m_floor[30];
	CSphere		m_sphere[2];
	CCamera		camera;
public :
	GS3D();
	~GS3D();
	BOOL Init(HWND hWnd, HINSTANCE hInst);
	VOID ShutDown();

	// ray tracing 
	D3DXVECTOR3 BG_COLOR;;
	VOID CreateObject();
	VOID RTRender();
	D3DXVECTOR3 *GetColor(D3DXVECTOR3 *dest, float x, float y);
	bool GetColor(D3DXVECTOR3 *dest, D3DXVECTOR4 *x, D3DXVECTOR4 *v,int depth);
	BOOL m_RayTracing_Lighting;
};
#endif 