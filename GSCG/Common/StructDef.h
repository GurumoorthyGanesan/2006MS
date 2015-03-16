

#ifndef _StructDef_H_
#define _StructDef_H_


#define SCREEN_WIDTH	512	
#define SCREEN_HEIGHT	512
// struct of vertex
class vertex2D{
public:
	int x,y;
	vertex2D *link;
	vertex2D();

};

class sPolygon2D{
public:
	int num_verts;
	DWORD color;
	vertex2D *vlist;
};

class sPoint2D{
public:
	double h, v; 
};

class CUSTOMVERTEX4F{
public:
    FLOAT x, y, z, rhw; // The transformed position for the vertex.
    DWORD color;        // The vertex color.
//	CUSTOMVERTEX4F *link;
};
#define D3DFVF_CUSTOMVERTEX4F (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

// ---------------------------------------------------------------------------
// ¦Û­qpolygonª«¥ó
// ---------------------------------------------------------------------------
enum {
	OBJ_TYPE_SPHERE = 0,
	OBJ_TYPE_TRIANGLE,
};

typedef struct{
	float	COLOR_AMB[4];	// ambient light
	float	COLOR_DIF[4];	// diffuse light
	float	COLOR_SPE[4];	// specular light
	float	speq_power;		// specular weight
	float   reflection;     // surface reflection ratio
	float   refraction;		// surface refraction ratio
	float   diffuse;		// diffuse reflection ratio
}Material;

// ---------------------------------------------------------------------------
typedef struct{
	int type;

	Material material;
	
	struct{
		float x0[3];	// vertex 1
		float x1[3];	// vertex 2
		float x2[3];	// vertex 3
	} triangle;
	struct {
		float center[3];// x,y,z ¤¤¤ß
		float radius;	// ¥b®|
	} sphere;

}OBJ_DATA;

// ---------------------------------------------------------------------------
class CObject
{
public:
	enum {
		INFINTY_DIST = 10000,	//'µL­­¤j'ªºµ{«×ªº¶ZÂ÷—£
	};
	int m_type;					// ª«¥ó«¬ºA
	Material m_material;		// ª«¥óªºmaterial

	void Init(OBJ_DATA *pData);
	float IsAcross(D3DXVECTOR4 *n, D3DXVECTOR4 *p, const D3DXVECTOR4 *x, const D3DXVECTOR4 *v){return INFINTY_DIST;}
	D3DXVECTOR3 *GetColor(D3DXVECTOR3 *dest, float LN, float HN );
};
// ---------------------------------------------------------------------------
// ²y
// ---------------------------------------------------------------------------
class CSphere : public CObject
{
private:
	D3DXVECTOR4 center;	// ¤¤¤ß¦ì¸m
	float radius_sq;	// ¥b®|­È
public:
	CSphere();

	void Init(OBJ_DATA *pData);
	float IsAcross(D3DXVECTOR4 *n, D3DXVECTOR4 *p, const D3DXVECTOR4 *x, const D3DXVECTOR4 *v);
};

// ---------------------------------------------------------------------------
// ¤T¨¤§Î
// ---------------------------------------------------------------------------
class CTriangle : public CObject
{
private:
	D3DXVECTOR4 pos[3];			// ³»ÂI®y¼Ð
	D3DXVECTOR4 normal;			// ªk¦V¶q
public:
	CTriangle();

	void Init(OBJ_DATA *pData);
	float IsAcross(D3DXVECTOR4 *n, D3DXVECTOR4 *p, const D3DXVECTOR4 *x, const D3DXVECTOR4 *v);
};

#endif