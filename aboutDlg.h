//---------------------------------------------------------------------------
//
// Name:        aboutDlg.h
// Author:      Florian Rasche
// Created:     29.11.2005 15:04:05
//
//---------------------------------------------------------------------------
#ifndef __ABOUTDLG_HPP_
#define __ABOUTDLG_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif


#include <wx/dialog.h>

//Compatibility for 2.4.x code
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

class aboutDlg : public wxDialog
{
public:
    aboutDlg( wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxT("About FANMOD"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE);
    virtual ~aboutDlg();

public:

  ////GUI Control Declaration Start
  wxButton *ok_button;
  //wxHtmlWindow* html;
  wxStaticText *program_text;
  wxStaticText *authors_text1;
  wxStaticText *authors_text2;
  wxStaticText *nauty_text1;
  wxStaticText *nauty_text2;
  wxStaticText *nauty_text3;
  wxStaticText *wxwidgets_text1;
  wxStaticText *wxwidgets_text2;
  wxStaticText *wxwidgets_text3;
  wxStaticText *funding_text1;
  wxStaticText *funding_text2;
  wxStaticText *funding_text3;
  
private:
    DECLARE_EVENT_TABLE()

public:
	enum {

   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum

public:
    void OnPaint(wxPaintEvent& event);    
    void aboutDlgClose(wxCloseEvent& event);
    void CreateGUIControls(void);

};


#endif
 
 
 
 
