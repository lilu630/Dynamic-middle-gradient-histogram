
// ShowHist.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CShowHistApp:
// 有关此类的实现，请参阅 ShowHist.cpp
//

class CShowHistApp : public CWinApp
{
public:
	CShowHistApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CShowHistApp theApp;