//---------------------------------------------------------------------------
//
// Name:        randesuFrm.h
// Author:      Florian Rasche
// Created:     20.09.2005 10:57:43
//
//---------------------------------------------------------------------------
#ifndef __randesuFrm_HPP_
#define __randesuFrm_HPP_

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/gauge.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/gbsizer.h>
#include <wx/panel.h>
#include <wx/filename.h>

#include <wx/frame.h>
#include "esuThread.h"
#include "ResultDialog.h"
#include "aboutDlg.h"


//Dialog Style
	#undef randesuFrm_STYLE
	#define randesuFrm_STYLE wxTAB_TRAVERSAL | wxDEFAULT_FRAME_STYLE

class randesuFrm : public wxFrame
{
private:
    DECLARE_EVENT_TABLE()
public:
    randesuFrm( wxWindow *parent, wxWindowID id = 1, const wxString &title = wxT("RandESU-GUI"),
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = randesuFrm_STYLE);
    virtual ~randesuFrm();
public:
  //GUI Control Declaration
	wxButton *exit_button;
	wxButton *about_button;
	wxTextCtrl *result_ctrl;
	wxButton *load_button;
	wxButton *result_button;
	wxStaticBox *results_box;
    wxStaticText *fixed_time_text;
    wxStaticText *time_text;
    wxStaticText *actual_perc_text;
	wxGauge *actual_gauge;
	wxStaticText *progresstext;
    wxStaticText *total_perc_text;
	wxGauge *total_gauge;
	wxStaticText *totalprogress_statictext;
	wxButton *start_button;
	wxStaticBox *algorithm_box;
	wxRadioButton *csv_button;
	wxRadioButton *textout_button;
	wxBitmapButton *outfile_button;
	wxTextCtrl *outfile_ctrl;
	wxStaticText *outfile_statictext;
	wxStaticBox *outfile_frame;
	wxCheckBox *directed_button;
	wxCheckBox *colored_vertices_button;
	wxCheckBox *colored_edges_button;
    wxCheckBox *dump_checkbox;
	wxBitmapButton *openfile_button;
	wxTextCtrl *infile_ctrl;
	wxStaticText *infile_statictext;
	wxStaticBox *infile_frame;
	wxTextCtrl *attempts_ctrl;
	wxStaticText *attempts_statictext;
	wxTextCtrl *exchanges_ctrl;
	wxStaticText *exchanges_statictext;
	wxTextCtrl *rnets_ctrl;
	wxStaticText *rnets_statictext;
	wxCheckBox *reest_subgr_box;
	wxCheckBox *regard_ec_box;
	wxCheckBox *regard_vc_box;
    wxComboBox *rtype_box;
	wxStaticText *rtype_statictext;
	wxStaticBox *rnets_box;
	wxTextCtrl **param_ctrls;
	wxRadioButton *sampling_button;
	wxStaticText *samples_statictext;
	wxRadioButton *full_enum_button;
	wxTextCtrl *treesamples_ctrl;
	wxComboBox *subgr_size_box;
	wxStaticText *subgr_statictext;
	wxStaticBox *options_box;
	wxStaticBox *setup_box;
	wxPanel *ThePanel;

    wxString program_name;

public:
    //Check that #define IDs do not conflict with these here!
	enum {
ID_EXIT_BUTTON = 1016,
ID_ABOUT_BUTTON = 1015,
ID_RESULT_CTRL = 1060,
ID_LOAD_BUTTON = 1014,
ID_RESULT_BUTTON = 1012,
ID_RESULTS_LINE = 1011,
ID_RESULTS_STATICTEXT = 1010,
ID_ACTUAL_GAUGE = 1058,
ID_PROGRESSTEXT = 1059,
ID_TOTAL_GAUGE = 1057,
ID_TOTALPROGRESS_STATICTEXT = 1056,
ID_START_BUTTON = 1009,
ID_ALGORITHM_LINE = 1006,
ID_ALGORITHM_STATICTEXT = 1005,
ID_CSV_BUTTON = 1052,
ID_TEXTOUT_BUTTON = 1051,
ID_OUTFILE_BUTTON = 1062,
ID_OUTFILE_CTRL = 1049,
ID_OUTFILE_STATICTEXT = 1048,
ID_OUTFILE_FRAME = 1008,
ID_COLORED_VERTICES_BUTTON = 1047,
ID_DIRECTED_BUTTON = 1046,
ID_OPENFILE_BUTTON = 1061,
ID_INFILE_CTRL = 1045,
ID_INFILE_STATICTEXT = 1043,
ID_INFILE_FRAME = 1007,
ID_ATTEMPTS_CTRL = 1042,
ID_ATTEMPTS_STATICTEXT = 1038,
ID_EXCHANGES_CTRL = 1041,
ID_EXCHANGES_STATICTEXT = 1037,
ID_RNETS_CTRL = 1040,
ID_RNETS_STATICTEXT = 1036,
ID_REEST_SUBGR_BOX = 1033,
ID_RTYPE_BOX = 1034,
ID_RTYPE_STATICTEXT = 1031,
ID_RNETS_BOX = 1004,
ID_PARAM7_CTRL = 1028,
ID_PARAM6_CTRL = 1027,
ID_PARAM5_CTRL = 1026,
ID_PARAM4_CTRL = 1025,
ID_PARAM3_CTRL = 1024,
ID_PARAM2_CTRL = 1023,
ID_PARAM1_CTRL = 1022,
ID_PARAM0_CTRL = 1021,
ID_SAMPLING_BUTTON = 1020,
ID_SAMPLES_STATICTEXT = 1029,
ID_FULL_ENUM_BUTTON = 1019,
ID_TREESAMPLES_CTRL = 1030,
ID_SUBGR_SIZE_BOX = 1018,
ID_SUBGR_STATICTEXT = 1017,
ID_OPTIONS_BOX = 1003,
ID_SETUP_LINE = 1002,
ID_SETUP_STATICTEXT = 1001,
ID_THEPANEL = 1063,
TIMER_ID = 1064,
ID_COLORED_EDGES_BUTTON = 1065,
ID_DUMP_CHECKBOX = 1066,

   ID_DUMMY_VALUE_ //Dont Delete this DummyValue
   }; //End of Enum

public:
    void CreateGUIControls(void);
    void randesuFrmClose(wxCloseEvent& event);
    // Event handling functions for the controls
	void exit_buttonClick(wxCommandEvent& event);
    void about_buttonClick(wxCommandEvent& event);
    void result_buttonClick(wxCommandEvent& event);
	void subgr_size_boxUpdated(wxCommandEvent& event );
	void openfile_buttonClick(wxCommandEvent& event);
	void outfile_buttonClick(wxCommandEvent& event);
	void load_buttonClick(wxCommandEvent& event);
	void full_enum_buttonClick(wxCommandEvent& event);
	void sampling_buttonClick(wxCommandEvent& event);
    void subgr_sizeUpdated(wxCommandEvent& event);
    void infile_ctrlUpdated(wxCommandEvent& event);
	void directed_buttonClick(wxCommandEvent& event);
	void ec_boxClick(wxCommandEvent& event);
	void vc_boxClick(wxCommandEvent& event);	
    void csv_buttonClick(wxCommandEvent& event);
	void textout_buttonClick(wxCommandEvent& event);
    void dump_boxClick(wxCommandEvent& event);
    // start_buttonClick processes all the inputs 
    // and starts the worker thread.
    void start_buttonClick(wxCommandEvent& event);
    // Updates the time
    void OnTimer(wxTimerEvent& event);
    // Event handling functions for the thread
    void threadFinished(wxCommandEvent& event);
    void threadNoSuccess(wxCommandEvent& event);
    void percentReached(wxCommandEvent& event);
    void netFinished(wxCommandEvent& event);
    void threadInfo(wxCommandEvent& event);



private:
    wxString resultfile;
    int nets_done;
    int total_nets;
    // Status of the worker thread
    bool thread_exists;
    bool thread_paused;
    esuThread *thread;
    // Vars for showing the elapsed time
    wxTimer *timer;
    wxStopWatch *watch;
    void SwitchControls(bool enable);
};

#endif
 
 
 
 
