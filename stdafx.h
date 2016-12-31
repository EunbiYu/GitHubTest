// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>


// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <xnamath.h>
//#include <DirectXMath.h>
//#include <DirectXColors.h>
//#include <DirectXPackedVector.h>
//#include <DirectXCollision.h>
//
//using namespace DirectX;
//using namespace DirectX::PackedVector;

#include <D3D11.h>
#include <d3dx11.h>
#include <mmsystem.h>
#include <math.h>
#include <d3dcompiler.h>
#include <D3DX10math.h>
#include <initguid.h>



//STL


// 화면크기
#define FRAME_BUFFER_WIDTH		640
#define FRAME_BUFFER_HEIGHT		480

//VS_SLOT 번호 지정
#define VS_SLOT_CAMERA			0x00
#define VS_SLOT_WORLD_MATRIX	0x01

//PS_SLOT 번호 지정
#define RANDOM_COLOR XMVECTOR((rand() * 0xFFFFFF) / RAND_MAX)

DEFINE_GUID(WKPDID_D3DDebugObjectName,
	0xa67e850a, 0xaa61, 0x4e0a, 0xbb, 0xf5, 0xac, 0xad, 0x32, 0xae, 0xf, 0xc);
