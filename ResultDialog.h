//---------------------------------------------------------------------------
//
// Name:        ResultDialog.h
// Author:      Florian Rasche
// Created:     19.10.2005 11:37:57
//
//---------------------------------------------------------------------------
#ifndef __RESULTDIALOG_HPP_
#define __RESULTDIALOG_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

// GUI includes
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/image.h>
#include <wx/gbsizer.h>
#include <wx/utils.h>
#include <wx/progdlg.h>
#include <wx/colordlg.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <fstream>
using std::ifstream; using std::ofstream;
#include <sstream>
using std::istringstream; using std::getline; using std::stringstream;
#include <cmath>
#include <wx/dialog.h>

//Compatibility for 2.4.x code
#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

//Dialog Style Start
	#undef ResultDialog_STYLE
	#define ResultDialog_STYLE wxDEFAULT_DIALOG_STYLE
//Dialog Style End

typedef unsigned long long uint64;
enum sort_constants { z_score = 0, p_value, freq, rand_mean, rand_sd };
const unsigned long inf_field[2]={0x00000000, 0x7ff00000};
const double Inf = *(double*) inf_field;


struct motif {
    string id;   
    string adj;
    double freq;
    double rand_mean;
    double rand_sd;
    double p_value;
    double z_score;
    short dangling_edges;
};

class ResultDialog : public wxDialog
{
public:
    ResultDialog( wxWindow *parent, const wxString & resultfile_p);
    virtual ~ResultDialog();

public:

  //GUI Control Declaration Start
  wxStaticBox *file_box;
  wxStaticText *file_text;
  wxTextCtrl *file_ctrl;
  wxBitmapButton *file_button;
  wxStaticText *motifs_text;
  wxTextCtrl *motifs_ctrl;
  wxCheckBox *pictures;
  wxStaticBox *filter_box;
  wxCheckBox *z_score_text;
  wxTextCtrl *z_score_ctrl;
  wxCheckBox *p_value_text;
  wxTextCtrl *p_value_ctrl;
  wxCheckBox *frequency_text;
  wxTextCtrl *frequency_ctrl;
  wxStaticText *percentage_text;
  wxCheckBox *number_text;
  wxTextCtrl *number_ctrl;
  wxStaticText *times_text;
  wxCheckBox *dangling_text;
  wxTextCtrl *dangling_min_ctrl;
  wxStaticText *dangling_middle_text;
  wxTextCtrl *dangling_max_ctrl;
  wxStaticText *dangling_end_text;
  wxRadioBox *sort_box;
  wxRadioBox *undef_box;
  wxStaticBox *vertexColorBox;
  wxStaticText **vertexNumbers;
  wxBitmapButton **vertexColorButtons;
  wxStaticText **edgeNumbers;
  wxBitmapButton **edgeColorButtons;
  wxStaticBox *edgeColorBox;
  wxButton *generate_button;
  wxButton *close_button;  
  //GUI Control Declaration End

private:
    DECLARE_EVENT_TABLE()

public:

	enum {

   ID_Z_SCORE_TEXT,
   ID_P_VALUE_TEXT,
   ID_FREQUENCY_TEXT,
   ID_NUMBER_TEXT,
   ID_DANGLING_TEXT,
   ID_SORT_BOX,
   ID_FILE_BUTTON,
   ID_GENERATE_BUTTON,
   ID_VERTEX_COLORS,
   ID_VERTEX_COLORS_END = ID_VERTEX_COLORS + 15,
   ID_EDGE_COLORS,
   ID_EDGE_COLORS_END = ID_EDGE_COLORS + 6,
   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum

public:
    
    void generate_buttonClick(wxCommandEvent& event);    
    void z_score_textClick(wxCommandEvent& event);    
    void p_value_textClick(wxCommandEvent& event);    
    void frequency_textClick(wxCommandEvent& event);    
    void number_textClick(wxCommandEvent& event);    
    void dangling_textClick(wxCommandEvent& event);    
    void file_buttonClick(wxCommandEvent& event);
    void ChooseVertexColor(wxCommandEvent& event);
    void ChooseEdgeColor(wxCommandEvent& event);
    void ResultDialogClose(wxCloseEvent& event);
    void CreateGUIControls(void);

private:
    void InitColors();
    bool ReadResults();
    void SortData(sort_constants sort);    
    bool DrawGraphs(const double & z_score_filter, const double & p_value_filter,
                    const double &  frequency_filter, const long & number_filter,
                    const long & d_min_filter, const long & d_max_filter);    
    bool GenerateHTML(const long & motifs_per_file,
                      const double & z_score_filter, const double & p_value_filter,
                      const double &  frequency_filter, const long & number_filter,
                      const long & d_min_filter, const long & d_max_filter);
    void StartHtmlFile(ofstream & out, int file_number);
    void FinishHtmlFile(ofstream & out, int file_number, bool is_last);
    void WriteIndexFile(int data_files, double z_score_filter, 
            double p_value_filter, double frequency_filter, long motifs_per_file, 
            long number_filter, long d_min_filter, long d_max_filter);
    wxString HTML_Color(const wxColour & color);
    
    void DrawGraph(wxDC *dc, string adj, int *x_coords, int *y_coords);
    // Draws an arrow on the given DC, Tip at x1,y1.
    void DrawNode(wxDC *dc, int x, int y, const int & color);
    void DrawEdge(wxDC *dc, int x1, int y1, int x2, int y2, const int & color, bool shift);
    void DrawArrow(wxDC *dc, int x1, int y1, int x2, int y2);
    const wxFileName resultfile;
    wxFileName htmlfile;
    bool pics; // Are pictures drawn or not
	int G_N, number_v_colors, number_e_colors; //Size of subgraphs in resultfile
    string summary1, summary2;	// result summary strings
	long number_of_subgraphs;
    bool ignore_undef; // are undef. Z-Scores ignored?
    static const int radius = 5; // radius of the picture circles
    vector<string> headers; // headers of the table
    vector<motif> data;   // data-sets of the table
    wxPen *vertex_pens;
    wxBrush *vertex_brushes;    
    wxPen *edge_pens;
    wxBrush *edge_brushes;
};

bool compare(const motif & a, const motif & b);

// Gets the Result summary from the file
wxString GetSummaryFromFile(const wxString & filename);

#endif
