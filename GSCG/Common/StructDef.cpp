

#include "Global.h"


// ---------------------------------------------------------------------------
// object 
// ---------------------------------------------------------------------------
void CObject::Init(OBJ_DATA *pData)
{
	m_type=pData->type;

	this->m_material = pData->material;
}
// ---------------------------------------------------------------------------
D3DXVECTOR3 *CObject::GetColor(D3DXVECTOR3 *dest, float LN, float HN )
{
	*dest = *(D3DXVECTOR3*)&this->m_material.COLOR_AMB
		  + *(D3DXVECTOR3*)&this->m_material.COLOR_DIF * LN
		  + *(D3DXVECTOR3*)&this->m_material.COLOR_SPE * powf(HN, this->m_material.speq_power);

	return dest;
}
// ---------------------------------------------------------------------------
// sphere
// ---------------------------------------------------------------------------
CSphere::CSphere(){
}
// ---------------------------------------------------------------------------
void CSphere::Init(OBJ_DATA *pData)
{
	((CObject*)this)->Init(pData);

	this->center.x = pData->sphere.center[0];
	this->center.y = pData->sphere.center[1];
	this->center.z = pData->sphere.center[2];

	radius_sq = pData->sphere.radius;
	radius_sq *= radius_sq;
}
// ---------------------------------------------------------------------------
float CSphere::IsAcross(D3DXVECTOR4 *n, D3DXVECTOR4 *p, const D3DXVECTOR4 *x, const D3DXVECTOR4 *v)
{
	// --- �ӼƾǱ��˭p�� --- 
	FLOAT t, tp, tn;
	// �D�� D
	D3DXVECTOR4 xc = (*x)-center;
	FLOAT xc2 = D3DXVec3Dot((D3DXVECTOR3 *)&xc, (D3DXVECTOR3 *)&xc);
	FLOAT vxc = D3DXVec3Dot((D3DXVECTOR3 *)v, (D3DXVECTOR3 *)&xc);
	FLOAT D = sqrtf(vxc*vxc-xc2+radius_sq); // ²�ƭp��
	
	if(D<0) return - INFINTY_DIST;

	// �D�� t
	tp = -vxc+D;
	tn = -vxc-D;

	if(tn<0){
		if(tp<0) return -INFINTY_DIST;
			t = tp;
	}else{
		t = tn;
	}

	*p = (*x)+t*(*v);
	*n = *p-center;
	// normalize 
	D3DXVec3Normalize((D3DXVECTOR3*)n, (D3DXVECTOR3*)n);

	return t;
}

// ---------------------------------------------------------------------------
// triangle
// ---------------------------------------------------------------------------
CTriangle::CTriangle(){
}
// ---------------------------------------------------------------------------
void CTriangle::Init(OBJ_DATA *pData)
{
	((CObject*)this)->Init(pData);

	// vector of vertex
	this->pos[0] = D3DXVECTOR4(	pData->triangle.x0[0],
								pData->triangle.x0[1],
								pData->triangle.x0[2],
								1.0f);
	this->pos[1] = D3DXVECTOR4(	pData->triangle.x1[0],
								pData->triangle.x1[1],
								pData->triangle.x1[2],
								1.0f);
	this->pos[2] = D3DXVECTOR4(	pData->triangle.x2[0],
								pData->triangle.x2[1],
								pData->triangle.x2[2],
								1.0f);

	D3DXVECTOR4 t0 = this->pos[1] - this->pos[0];
	D3DXVECTOR4 t1 = this->pos[2] - this->pos[0];
	// �~�n�D�otriangle�k�V�q(n)
	D3DXVec3Cross((D3DXVECTOR3*)&normal, (D3DXVECTOR3*)&t1, (D3DXVECTOR3*)&t0);
	// normalize
	D3DXVec3Normalize((D3DXVECTOR3*)&normal, (D3DXVECTOR3*)&normal);
	normal.w = 0;
}
// ---------------------------------------------------------------------------
float CTriangle::IsAcross(D3DXVECTOR4 *n, D3DXVECTOR4 *p, const D3DXVECTOR4 *x, const D3DXVECTOR4 *v)
{
	FLOAT t;

	// --- �ӼƾǱ��˭p�� --- 
	D3DXVECTOR4 xp = this->pos[0]-(*x);
	FLOAT xpn = D3DXVec3Dot((D3DXVECTOR3 *)&xp, (D3DXVECTOR3 *)&normal);
	FLOAT vn  = D3DXVec3Dot((D3DXVECTOR3 *)v, (D3DXVECTOR3 *)&normal);
	// normal vector too small
	if(-0.00001f<=vn) return -INFINTY_DIST;

	t = xpn/vn;
	if(t<0) return -INFINTY_DIST;
	
	*p = (*x)+t*(*v);
	*n = normal;

	D3DXVECTOR4 d0, d1;
	D3DXVECTOR3 c;
	// w.r.t normal of vertex 0 
	d0 = (*p)-this->pos[0];
	d1 = this->pos[1]-this->pos[0];
	D3DXVec3Cross(&c, (D3DXVECTOR3*)&d1, (D3DXVECTOR3*)&d0);
	if(D3DXVec3Dot(&c, (D3DXVECTOR3 *)&normal)>0)return -INFINTY_DIST;
	// w.r.t normal of vertex 1 
	d0 = (*p)-this->pos[1];
	d1 = this->pos[2]-this->pos[1];
	D3DXVec3Cross(&c, (D3DXVECTOR3*)&d1, (D3DXVECTOR3*)&d0);
	if(D3DXVec3Dot(&c, (D3DXVECTOR3 *)&normal)>0)return -INFINTY_DIST;
	// w.r.t normal of vertex 2 
	d0 = (*p)-this->pos[2];
	d1 = this->pos[0]-this->pos[2];
	D3DXVec3Cross(&c, (D3DXVECTOR3*)&d1, (D3DXVECTOR3*)&d0);
	if(D3DXVec3Dot(&c, (D3DXVECTOR3 *)&normal)>0)return -INFINTY_DIST;
	
	return t;
}