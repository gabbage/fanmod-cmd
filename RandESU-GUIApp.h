//---------------------------------------------------------------------------
//
// Name:        RandESU-GUIApp.h
// Author:      Florian Rasche
// Created:     20.09.2005 10:57:43
//
//---------------------------------------------------------------------------

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

class randesuFrmApp:public wxApp
{
public:
	bool OnInit();
	int OnExit();
};

 
