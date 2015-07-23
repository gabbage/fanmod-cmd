//---------------------------------------------------------------------------
//
// Name:        aboutDlg.cpp
// Author:      Florian Rasche
// Created:     29.11.2005 15:04:05
//
//---------------------------------------------------------------------------

#include "aboutDlg.h"
#include "icon.xpm"
#include "TELEKOM_XPM.xpm"
#include "DFG_XPM.xpm"


// MyCanvas

class MyCanvas: public wxPanel
{
public:
    MyCanvas( wxWindow *parent, wxWindowID id);
    void OnPaint( wxPaintEvent &event );

private:
    DECLARE_EVENT_TABLE()
};

//----------------------------------------------------------------------------
// aboutDlg
//----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(aboutDlg,wxDialog)
      EVT_CLOSE(aboutDlg::aboutDlgClose)
    END_EVENT_TABLE()



aboutDlg::aboutDlg( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxDialog( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

aboutDlg::~aboutDlg() {} 

void aboutDlg::CreateGUIControls(void)
{
    //GUI Items Creation
    program_text = new wxStaticText(this, wxID_ANY, wxT("FANMOD - The FAst Network MOtif Detection tool"));
	program_text->SetFont(wxFont(10, wxDEFAULT, wxNORMAL, wxBOLD, FALSE));    
    
    authors_text1 = new wxStaticText(this, wxID_ANY, wxT("Florian Rasche"));
    authors_text2 = new wxStaticText(this, wxID_ANY, wxT("Sebastian Wernicke"));
    nauty_text1 = new wxStaticText(this, wxID_ANY, wxT("FANMOD uses the Nauty algorithm"));
    nauty_text2 = new wxStaticText(this, wxID_ANY, wxT("© Brendan McKay"));
    nauty_text3 = new wxStaticText(this, wxID_ANY, wxT("http://cs.anu.edu.au/~bdm/nauty/"));
    wxwidgets_text1 = new wxStaticText(this, wxID_ANY, wxT("The user interface was programmed using wxWidgets."));
    wxwidgets_text2 = new wxStaticText(this, wxID_ANY, wxT("http://www.wxwidgets.org/"));
    //wxwidgets_text3 = new wxStaticText(this, wxID_ANY, wxT(""));
    funding_text1 = new wxStaticText(this, wxID_ANY, wxT("Our work is supported by the"));
    funding_text2 = new wxStaticText(this, wxID_ANY, wxT("\"Deutsche Telekom Stiftung\" (www.telekom-stiftung.de)"));
    funding_text3 = new wxStaticText(this, wxID_ANY, wxT("and the \"Deutsche Forschungsgemeinschaft\" (www.dfg.de)."));
    MyCanvas *canvas = new MyCanvas(this, wxID_ANY);
    ok_button = new wxButton(this, wxID_OK, wxT("OK"));      

    // Sizer Declaration
    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer *textSizer = new wxBoxSizer(wxVERTICAL);
      wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	
    // Sizer Configuration
    
    textSizer->Add( program_text, 0, wxALL | wxALIGN_CENTER, 3);
    textSizer->Add( authors_text1, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( authors_text2, 0, wxBOTTOM | wxALIGN_CENTER, 6);
    textSizer->Add( nauty_text1, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( nauty_text2, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( nauty_text3, 0, wxBOTTOM | wxALIGN_CENTER, 6);
    textSizer->Add( wxwidgets_text1, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( wxwidgets_text2, 0, wxBOTTOM | wxALIGN_CENTER, 6);
    //textSizer->Add( wxwidgets_text3, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( funding_text1, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( funding_text2, 0, wxALL | wxALIGN_CENTER, 1);
    textSizer->Add( funding_text3, 0, wxBOTTOM | wxALIGN_CENTER, 3);
    buttonSizer->AddStretchSpacer(1); //The button should be on the right
    buttonSizer->Add(ok_button, 0, wxALL, 3);
    topSizer->Add(textSizer, 0, wxGROW | wxALL, 3);
    topSizer->Add(canvas, 0, wxGROW | wxALL | wxALIGN_CENTER, 3);
    topSizer->Add(buttonSizer, 0, wxGROW | wxALL, 3);
    
    this->SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);
	
    this->SetTitle(wxT("About FANMOD"));
	this->Center();
	this->SetIcon(wxIcon(icon_xpm));

}

void aboutDlg::aboutDlgClose(wxCloseEvent& event)
{
    Destroy();
}
 
BEGIN_EVENT_TABLE(MyCanvas, wxPanel)
  EVT_PAINT(MyCanvas::OnPaint)
END_EVENT_TABLE()

MyCanvas::MyCanvas( wxWindow *parent, wxWindowID id)
        : wxPanel( parent, id, wxDefaultPosition, wxSize(355, 27)) // 170, 55 if images shall be on top of each other
{}

void MyCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc(this);

    wxBitmap telekom(TELEKOM_XPM);
    wxBitmap dfg(DFG_XPM);
    dc.DrawBitmap(dfg, 5, 3, true);
    dc.DrawBitmap(telekom, 185, 0, true); // 5, 30 if images on top
}
