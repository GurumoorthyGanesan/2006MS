

#include "..\..\Common\Global.h"

vertex2D::vertex2D(){
	x = 1; y = 1;
	link = NULL;
}

GS2D::GS2D(){
};
GS2D::~GS2D(){
};

BOOL GS2D::Init(HWND thWnd, HINSTANCE thInst){
	hWnd = thWnd;
	hInst = thInst;

	//red, orange, yellow, green, blue, cyan, and purple
    COLORREF col[8]= { RGB(255,255,25),RGB(255,0,0),RGB(255,165,0),RGB(255,255,0),	\
	RGB(0,255,0),RGB(0,0,255),RGB(0,255,255),RGB(128,0,128)};

	// Clip½d³ò
	min_clip_x=100,min_clip_y=100,max_clip_x=250,max_clip_y=250;
	return TRUE;
}

// allocate the edge point to single link list structure
void GS2D::Alloc(int x,int y,vertex2D *e){
	vertex2D *t = new vertex2D;
	t->x = x; t->y = y;
	if (ptr){
		t->link = e->link;
		e->link = t;
	}else{
		t->link = NULL;
		ptr = t;
	}
}

void GS2D::DrawNostril(int x,int y,COLORREF col){
	HDC hdc = GetDC(hWnd);
	for (int i=0;i<NostrilWidth;i++){
		SetPixel(hdc,x+i,y+i, col);
		SetPixel(hdc,x-i,y+i, col);
		SetPixel(hdc,x+i,y-i, col);
		SetPixel(hdc,x-i,y-i, col);
	}
	 ReleaseDC(hWnd, hdc);
}
void GS2D::DrawPixel(int x,int y,COLORREF col){
	 HDC hdc = GetDC(hWnd);
	 SetPixel(hdc,x,y, col);
	// Sleep(1);
	 ReleaseDC(hWnd, hdc);
}

void GS2D::Line(int xa,int ya,int xb,int yb,COLORREF col){
	int t;
	if (xa==xb) { MyMidpointLineSE(xa,ya,xb,yb,col);}//MB("¤À¥À¬°¹s");
	//  compare with ya,yb values, if ya is bigger than yb, do swap  
	if (Comp(ya,yb)){	
		t=yb;yb=ya;ya=t;
		t=xb;xb=xa;xa=t;
	}
	// compute the line slope
	float m =(float) (yb-ya) / (xb-xa);
	// decide to m values
	if (m > 0){
		// turn E
		if (m <= 1)
			MyMidpointLineE(xa,ya,xb,yb,col);
		else 
			MyMidpointLineSE(xa,ya,xb,yb,col);
	}
	else{	// m < 0
		// turn W
		if (m >= -1)
			MyMidpointLineW(xa,ya,xb,yb,col);
		else
			MyMidpointLineSW(xa,ya,xb,yb,col);
	}
}

void GS2D::MyMidpointLineE(int xa,int ya,int xb,int yb,COLORREF color){

	int dx,dy,dd,IncrE,IncrSE,x,y;
	dx = xb - xa;	dy = yb - ya;
	dd = 2 * dy - dx;
	IncrE = 2 * dy;
	IncrSE = 2 * (dy - dx);
	x = xa; y = ya;
	DrawPixel(x,y, color);
	while (x < xb){
		if (dd <= 0){
			dd += IncrE;
			x++;
		}else{
			dd += IncrSE;
			x++; y++;
		}
		DrawPixel(x,y, color);
	}
}

void GS2D::MyMidpointLineSE(int xa,int ya,int xb,int yb,COLORREF color){
	int dx,dy,dd,IncrS,IncrSE,x,y;
	dx = xb - xa;	dy = yb - ya;
	dd = - dy - 2 * dx;
	IncrS = -(2 * dx);
	IncrSE = 2 * (dy - dx);
	x = xa; y = ya;
	DrawPixel(x,y, color);

	while (y < yb){
		if (dd <= 0){
			dd += IncrSE;
			x++; y++;
		}else{
			dd += IncrS;
			y++;
			
		}
		DrawPixel(x,y, color);
	}
}

void GS2D::MyMidpointLineW(int xa,int ya,int xb,int yb,COLORREF color){

	int dx,dy,dd,IncrW,IncrSW,x,y;
	dx = xb - xa;	dy = yb - ya;
	dd = - (2 * dy) - dx;
	IncrW = 2 * -dy;
	IncrSW = 2 * (- dy - dx);
 	x = xa; y = ya;
	DrawPixel(x,y, color);

	while (x > xb){
		if (dd > 0){
			dd += IncrW;
			x--;
		}else{
			dd += IncrSW;
			x--; y++;
		}
		DrawPixel(x,y, color);
	}
}

void GS2D::MyMidpointLineSW(int xa,int ya,int xb,int yb,COLORREF color){

	int dx,dy,dd,IncrS,IncrSW,x,y;
	dx = xb - xa;	dy = yb - ya;
	dd = (- 2 * dy) - dx;
	IncrS = -(2 * dx);
	IncrSW = 2 * (-dy - dx);
	x = xa; y = ya;
	DrawPixel(x,y, color);

	while (y < yb){
		if (dd > 0){
			dd += IncrSW;
			x--; y++;
		}else{
			dd += IncrS;
			y++;
		}
		DrawPixel(x,y, color);
	}
}

void GS2D::Polygon2D(sPolygon2D *poly){
	for (int index=1;index<poly->num_verts;index++ ){
		 Line(poly->vlist[index].x,
					 poly->vlist[index].y ,
					 poly->vlist[index+1].x,
					 poly->vlist[index+1].y,
					 poly->color);
	}
	Line(poly->vlist[1].x ,
				poly->vlist[1].y ,
				poly->vlist[index].x,
				poly->vlist[index].y ,
				poly->color);
}

bool GS2D::Cohen_Sutherland(int &x1,int &y1,int &x2,int &y2){
	HDC hdc = GetDC(hWnd);
	Rectangle(hdc,min_clip_x,min_clip_y,max_clip_x,max_clip_y);
	int xc1=x1, yc1=y1, xc2=x2, yc2=y2;
	int p1_code=0, p2_code=0;

// determine codes for p1 and p2
if (y1 < min_clip_y)	p1_code|=CLIP_CODE_N;
else
if (y1 > max_clip_y)	p1_code|=CLIP_CODE_S;

if (x1 < min_clip_x)	p1_code|=CLIP_CODE_W;
else
if (x1 > max_clip_x)	p1_code|=CLIP_CODE_E;

if (y2 < min_clip_y)	p2_code|=CLIP_CODE_N;
else
if (y2 > max_clip_y)	p2_code|=CLIP_CODE_S;

if (x2 < min_clip_x)	p2_code|=CLIP_CODE_W;
else
if (x2 > max_clip_x)	p2_code|=CLIP_CODE_E;

// try and trivially reject
if ((p1_code & p2_code)) 	return(0);

// test for totally visible, if so leave points untouched
if (p1_code==0 && p2_code==0)	return(1);

// determine end clip point for p1
switch(p1_code)	  {
	  case CLIP_CODE_C: break;

	  case CLIP_CODE_N:{
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
			} break;
	  case CLIP_CODE_S:{
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
		   } break;

	  case CLIP_CODE_W:{
		   xc1 = min_clip_x;
		   yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
		   } break;
		
	  case CLIP_CODE_E:{
		   xc1 = max_clip_x;
		   yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
		   } break;

	// these cases are more complex, must compute 2 intersections
	  case CLIP_CODE_NE:{
		   // north hline intersection
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);

		   // test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
				{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
				} // end if

		   } break;
	  
	  case CLIP_CODE_SE:{
		   // south hline intersection
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	

		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  // east vline intersection
			  xc1 = max_clip_x;
			  yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
			  } // end if

		   } break;
	    
	  case CLIP_CODE_NW:{
		   // north hline intersection
		   yc1 = min_clip_y;
		   xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  xc1 = min_clip_x;
		      yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			  } // end if

		   } break;
	  	  
	  case CLIP_CODE_SW:{
		   // south hline intersection
		   yc1 = max_clip_y;
		   xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc1 < min_clip_x || xc1 > max_clip_x)
		      {
			  xc1 = min_clip_x;
		      yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			  } // end if

		   } break;

	  default:break;

	  } // end switch

// determine clip point for p2
switch(p2_code)
	  {
	  case CLIP_CODE_C: break;

	  case CLIP_CODE_N:{
		   yc2 = min_clip_y;
		   xc2 = x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2);
		   } break;

	  case CLIP_CODE_S:{
		   yc2 = max_clip_y;
		   xc2 = x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2);
		   } break;

	  case CLIP_CODE_W:{
		   xc2 = min_clip_x;
		   yc2 = y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2);
		   } break;
		
	  case CLIP_CODE_E:{
		   xc2 = max_clip_x;
		   yc2 = y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2);
		   } break;

		// these cases are more complex, must compute 2 intersections
	  case CLIP_CODE_NE:{
		   // north hline intersection
		   yc2 = min_clip_y;
		   xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);

		   // test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
				{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
				} // end if

		   } break;
	  
	  case CLIP_CODE_SE:{
		   // south hline intersection
		   yc2 = max_clip_y;
		   xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	

		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  // east vline intersection
			  xc2 = max_clip_x;
			  yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
			  } // end if

		   } break;
	    
	  case CLIP_CODE_NW: {
		   // north hline intersection
		   yc2 = min_clip_y;
		   xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  xc2 = min_clip_x;
		      yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			  } // end if

		   } break;
	  	  
	  case CLIP_CODE_SW:{
		   // south hline intersection
		   yc2 = max_clip_y;
		   xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	
		   
		   // test if intersection is valid, of so then done, else compute next
		   if (xc2 < min_clip_x || xc2 > max_clip_x)
		      {
			  xc2 = min_clip_x;
		      yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			  } // end if

		   } break;
	
	  default:break;

	  } // end switch

// do bounds check
if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
	(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
	(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
	(yc2 < min_clip_y) || (yc2 > max_clip_y) )
	{
	return(0);
	} // end if

// store vars back
x1 = xc1;
y1 = yc1;
x2 = xc2;
y2 = yc2;

return(1);
}


// to caculate the middle point of two points
void GS2D::middle(sPoint2D &p0, sPoint2D &p1,sPoint2D &p01)
{
	p01.h = (p0.h+p1.h)/2;
	p01.v = (p0.v+p1.v)/2;
}

// recursive function to implement deCasteljau algorithm
void GS2D::recursive_bezier(sPoint2D &p1,sPoint2D &h1,sPoint2D &h2,sPoint2D &p2,COLORREF color)
{
    if(fabs(p1.h - p2.h) <=1 && fabs(p1.v - p2.v) <=1) {
		Line((int)p1.h, (int)p1.v, (int)p2.h, (int)p2.v, color);
	}
	else {
	  sPoint2D a, b1, b2, c1, c2, d;
	  middle(h1, h2, a);
	  middle(p1, h1, b1);
	  middle(h2, p2, b2);
	  middle(b1, a, c1);
	  middle(a, b2, c2);
	  middle(c1, c2, d);
	  recursive_bezier(p1, b1, c1, d, color);
	  recursive_bezier(d, c2, b2, p2, color);
	}
}

// function to draw Bezier curve
void GS2D::DrawBezierDeCasteljau(sPoint2D *bezier,COLORREF color)
{
	recursive_bezier(bezier[0], bezier[1],
	       bezier[2], bezier[3], color);
}


void GS2D::binomialCoeffs(int n,int *c){
	int j,k;
	for (k=0;k<=n;k++){
		c[k] = 1;
		for (j=n;j>=k+1;j--) c[k] *= j ;
		for (j=n-k;j >=2;j--) c[k] /= j;
	}
}

void GS2D::computeBezPt(double u, sPoint2D *bezPt,int nCtrlPoint,sPoint2D *CtrlPoint,int *c){
	int k,n = nCtrlPoint - 1;
	double bezBlendFcn=0.0;
	bezPt->h = bezPt->v = 0.0;
	for (k=0;k<nCtrlPoint;k++){
		bezBlendFcn = (double)(c[k] * pow(u,k) * pow(1-u,n-k));
		bezPt->h += (CtrlPoint[k].h * bezBlendFcn);
		bezPt->v += (CtrlPoint[k].v * bezBlendFcn);
	}
}

void GS2D::DrawBezierPierre(int n,sPoint2D *ctrlPts,int nBezCurPts){
	int *c = new int[n];
	double u=0.0;
	sPoint2D bezCurvePt;
	binomialCoeffs(n - 1,c);
	for (int k=0;k <= nBezCurPts;k++){
		u = (double) k / (double) nBezCurPts;
		computeBezPt(u,&bezCurvePt,n,ctrlPts,c);
		DrawPixel((int)bezCurvePt.h,(int)bezCurvePt.v,RGB(255,0,0));
	}
    delete [] c;
}