//---------------------------------------------------------------------------
//
// Name:        RandESU-GUIApp.cpp
// Author:      Florian Rasche
// Created:     20.09.2005 10:57:43
//
//---------------------------------------------------------------------------

#include "RandESU-GUIApp.h"
#include "randesuFrm.h"

IMPLEMENT_APP(randesuFrmApp)

bool randesuFrmApp::OnInit()
{
    randesuFrm *myFrame = new  randesuFrm(NULL);
    SetTopWindow(myFrame);
    myFrame->Show(TRUE);		
    return TRUE;
}
 
int randesuFrmApp::OnExit()
{
	return 0;
}
