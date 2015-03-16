//////////////////////////////////////////////////////////////////
//
//
//*****************************************											
//	高等電腦圖學定義表頭檔, Global.h - v0.1	
//  author : Y.Y.L Date : 02/27/03
//*****************************************
//----------------------------------------------------------------


#ifndef _GLOBAL_H_
#define _GLOBAL_H_

// INCLUDE ////////////////////////////////////////////////////////

#ifdef WIN32
// Windows & STL includes
#include <windows.h>
#include <iostream>
#include <fstream.h>
#include <assert.h>

#else
// Standard ANSI-C includes
#include <stdio.h>

#endif

// DirectX includes
#include "d3d9.h"
#include "d3dx9.h"
#include "dinput.h"


// Custom Engine includes
#include "resource.h"
#include "StructDef.h"
#include "..\CG\Win32\GSSystem.h"
#include "..\CG\DXD\GSDXInput.h"
#include "..\CG\DXD\GSDXGraphic.h"
#include "..\CG\Lib\GS2D.h"
#include "..\CG\Lib\GS3D.h"

#pragma warning( disable: 4127 )
#pragma inline_depth( 255 )
#pragma inline_recursion( on )

//#pragma comment(lib,"CV/GSCV.lib")

// DEFINE	////////////////////////////////////////////////////////
// 大小比較 assertion
//////////////////////////////////////////////////////////////////
#ifdef WIN32
#define cassert(exp,description) (void)( (exp) || ( MessageBox( NULL, #description, "Assert", 0 ) ) )
#else
#define cassert(exp,description) (void)( (exp) || ( fprintf(stderr, "Assert: %s ", #description ) ) )
#endif

//////////////////////////////////////////////////////////////////
// MessageBox
//////////////////////////////////////////////////////////////////
#define MB(exp) ( MessageBox( NULL, exp, " Warning", MB_OK ) )

//////////////////////////////////////////////////////////////////
// Tool kit
//////////////////////////////////////////////////////////////////
#define Comp(x,y) (int) ( (x > y) ? 1 : 0)


#endif