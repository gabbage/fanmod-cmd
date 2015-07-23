//---------------------------------------------------------------------------
//
// Name:        randesuFrm.cpp
// Author:      Florian Rasche
// Created:     20.09.2005 10:57:43
//
//---------------------------------------------------------------------------

#include "randesuFrm.h"
#include "tooltips.cpp"
#include "openfile_button_XPM.xpm"
#include "icon.xpm"

//----------------------------------------------------------------------------
// randesuFrm
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(randesuFrm,wxFrame)

	EVT_CLOSE(randesuFrm::randesuFrmClose)
	EVT_BUTTON(ID_EXIT_BUTTON,randesuFrm::exit_buttonClick)
	EVT_BUTTON(ID_ABOUT_BUTTON,randesuFrm::about_buttonClick)
	EVT_BUTTON(ID_LOAD_BUTTON,randesuFrm::load_buttonClick)
    EVT_BUTTON(ID_RESULT_BUTTON,randesuFrm::result_buttonClick)
	EVT_BUTTON(ID_OUTFILE_BUTTON,randesuFrm::outfile_buttonClick)
	EVT_BUTTON(ID_OPENFILE_BUTTON,randesuFrm::openfile_buttonClick)
	EVT_RADIOBUTTON(ID_SAMPLING_BUTTON,randesuFrm::sampling_buttonClick)
	EVT_RADIOBUTTON(ID_FULL_ENUM_BUTTON,randesuFrm::full_enum_buttonClick)
	EVT_CHECKBOX(ID_DIRECTED_BUTTON,randesuFrm::directed_buttonClick)
	EVT_CHECKBOX(ID_COLORED_EDGES_BUTTON, randesuFrm::ec_boxClick)
	EVT_CHECKBOX(ID_COLORED_VERTICES_BUTTON,randesuFrm::vc_boxClick)
	EVT_CHECKBOX(ID_DUMP_CHECKBOX,randesuFrm::dump_boxClick)
	EVT_RADIOBUTTON(ID_CSV_BUTTON,randesuFrm::csv_buttonClick)
	EVT_RADIOBUTTON(ID_TEXTOUT_BUTTON,randesuFrm::textout_buttonClick)
	EVT_COMBOBOX(ID_SUBGR_SIZE_BOX, randesuFrm::subgr_sizeUpdated)
	EVT_TEXT(ID_INFILE_CTRL, randesuFrm::infile_ctrlUpdated)
	EVT_BUTTON(ID_START_BUTTON,randesuFrm::start_buttonClick)
	EVT_TIMER(TIMER_ID, randesuFrm::OnTimer) // Updates the time when the timer ticks.
    // The menu events are coming from the worker thread!
	EVT_MENU(ID_THREAD_FINISHED,randesuFrm::threadFinished)
	EVT_MENU(ID_THREAD_NO_SUCCESS,randesuFrm::threadNoSuccess)
	EVT_MENU(ID_PERCENT_REACHED,randesuFrm::percentReached)
	EVT_MENU(ID_NET_FINISHED,randesuFrm::netFinished)
	EVT_MENU(ID_THREAD_INFO,randesuFrm::threadInfo)
	
		
END_EVENT_TABLE()


randesuFrm::randesuFrm( wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &position, const wxSize& size, long style )
    : wxFrame( parent, id, title, position, size, style)
{
    CreateGUIControls();
}

randesuFrm::~randesuFrm() {} 

void randesuFrm::CreateGUIControls(void)
{
    // Set some global vars
    program_name = wxT("FANMOD");
    resultfile = wxT("");
    thread_exists = false; // Status of the worker thread
    thread_paused = false;
    nets_done = 0;
    total_nets = 0;
    timer = new wxTimer(this, TIMER_ID); // Timer for the events every second
    watch = new wxStopWatch();           // watch for the elapsed time

    //GUI Items Creation
	ThePanel = new wxPanel(this, ID_THEPANEL);

    // Creation of visible items
	setup_box = new wxStaticBox(ThePanel, ID_SETUP_STATICTEXT, wxT("SETUP"));
	setup_box->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE));
    setup_box->SetOwnForegroundColour(wxColour("black"));

	options_box = new wxStaticBox(ThePanel, ID_OPTIONS_BOX, wxT("Algorithm options "));

	subgr_statictext = new wxStaticText(ThePanel, ID_SUBGR_STATICTEXT, wxT("Subgraph size:"));
    subgr_statictext->SetToolTip(subgr_size_tooltip);
    
	wxArrayString arrayStringFor_subgr_size_box;
	arrayStringFor_subgr_size_box.Add(wxT("3"));
	arrayStringFor_subgr_size_box.Add(wxT("4"));
	arrayStringFor_subgr_size_box.Add(wxT("5"));
	arrayStringFor_subgr_size_box.Add(wxT("6"));
	arrayStringFor_subgr_size_box.Add(wxT("7"));
	arrayStringFor_subgr_size_box.Add(wxT("8"));
	subgr_size_box = new wxComboBox(ThePanel, ID_SUBGR_SIZE_BOX, wxT("3"), wxDefaultPosition, wxSize(55,-1), arrayStringFor_subgr_size_box, wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT, wxDefaultValidator, wxT("subgr_size_box"));
	subgr_size_box->SetToolTip(subgr_size_tooltip);

	treesamples_ctrl = new wxTextCtrl(ThePanel, ID_TREESAMPLES_CTRL, wxT("100000"), wxDefaultPosition, wxSize(80,-1));
	treesamples_ctrl->SetToolTip(samples_tooltip);

	samples_statictext = new wxStaticText(ThePanel, ID_SAMPLES_STATICTEXT, wxT("Samples to estimate\nnumber of subgraphs:"));
	samples_statictext->SetToolTip(samples_tooltip);

	full_enum_button = new wxRadioButton(ThePanel, ID_FULL_ENUM_BUTTON, wxT("Full enumeration"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, wxT("full_enum_button"));
	full_enum_button->SetValue(true);
	full_enum_button->SetToolTip(full_enum_tooltip);

	sampling_button = new wxRadioButton(ThePanel, ID_SAMPLING_BUTTON, wxT("Sampling probabilities"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("sampling_button"));
	sampling_button->SetToolTip(sampling_tooltip);

	param_ctrls = new wxTextCtrl *[8];
	for (int i=0; i<8; i++){
        param_ctrls[i] = new wxTextCtrl(ThePanel, ID_PARAM0_CTRL+i, wxT("0.5"), wxDefaultPosition, wxSize(36,-1), 0, wxDefaultValidator, wxT("param_ctrl"));
        param_ctrls[i]->Disable();
        param_ctrls[i]->SetToolTip(sampling_param_tooltip);
    }

	rnets_box = new wxStaticBox(ThePanel, ID_RNETS_BOX, wxT("Random networks "));

	rtype_statictext = new wxStaticText(ThePanel, ID_RTYPE_STATICTEXT, wxT("No. of bidir. edges:"));
	rtype_statictext->SetToolTip(random_type_tooltip);

	wxArrayString arrayStringFor_rtype_box;
	arrayStringFor_rtype_box.Add(wxT("No regard"));
	arrayStringFor_rtype_box.Add(wxT("Global const"));
	arrayStringFor_rtype_box.Add(wxT("Local const"));
	rtype_box = new wxComboBox(ThePanel, ID_RTYPE_BOX, wxT("Local const"), wxDefaultPosition, wxDefaultSize, arrayStringFor_rtype_box, wxCB_DROPDOWN | wxCB_READONLY, wxDefaultValidator, wxT("rtype_box"));
	rtype_box->SetToolTip(random_type_tooltip);
    rtype_box->SetSelection(2);

    regard_ec_box = new wxCheckBox(ThePanel, wxID_ANY, wxT("Regard edge colors"));
    regard_ec_box->Disable();
    regard_ec_box->SetToolTip(regard_ec_tooltip);

    regard_vc_box = new wxCheckBox(ThePanel, wxID_ANY, wxT("Regard vertex colors"));
    regard_vc_box->Disable();
    regard_vc_box->SetToolTip(regard_vc_tooltip);

	reest_subgr_box = new wxCheckBox(ThePanel, ID_REEST_SUBGR_BOX, wxT("Reestimate subgraph number "), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("reest_subgr_box"));
	reest_subgr_box->SetToolTip(reest_subgr_tooltip);

	rnets_statictext = new wxStaticText(ThePanel, ID_RNETS_STATICTEXT, wxT("Number of networks:"));
	rnets_statictext->SetToolTip(number_rnets_tooltip);

	rnets_ctrl = new wxTextCtrl(ThePanel, ID_RNETS_CTRL, wxT("1000"), wxDefaultPosition, wxSize(80,-1));
	rnets_ctrl->SetToolTip(number_rnets_tooltip);

	exchanges_statictext = new wxStaticText(ThePanel, ID_EXCHANGES_STATICTEXT, wxT("Exchanges per edge:"));
	exchanges_statictext->SetToolTip(exchanges_tooltip);

	exchanges_ctrl = new wxTextCtrl(ThePanel, ID_EXCHANGES_CTRL, wxT("3"), wxDefaultPosition, wxSize(40,-1));
	exchanges_ctrl->SetToolTip(exchanges_tooltip);

	attempts_statictext = new wxStaticText(ThePanel, ID_ATTEMPTS_STATICTEXT, wxT("Exchange attempts:"));
	attempts_statictext->SetToolTip(attempts_tooltip);

	attempts_ctrl = new wxTextCtrl(ThePanel, ID_ATTEMPTS_CTRL, wxT("3"), wxDefaultPosition, wxSize(40,-1));
	attempts_ctrl->SetToolTip(attempts_tooltip);

	infile_frame = new wxStaticBox(ThePanel, ID_INFILE_FRAME, wxT("Input graph "));

	infile_statictext = new wxStaticText(ThePanel, ID_INFILE_STATICTEXT, wxT("Input file:"));
	infile_statictext->SetToolTip(inputfile_tooltip);

	infile_ctrl = new wxTextCtrl(ThePanel, ID_INFILE_CTRL, wxT(""));
	infile_ctrl->SetToolTip(inputfile_tooltip);

	wxBitmap openfile_button_BITMAP (openfile_button_XPM);
	openfile_button = new wxBitmapButton(ThePanel, ID_OPENFILE_BUTTON, openfile_button_BITMAP, wxDefaultPosition, wxSize(25,25), wxBU_AUTODRAW, wxDefaultValidator, wxT("openfile_button"));
	openfile_button->SetToolTip(inputfile_tooltip);

	directed_button = new wxCheckBox(ThePanel, ID_DIRECTED_BUTTON, wxT("Directed"));
    directed_button->SetValue(true);
	directed_button->SetToolTip(directed_tooltip);
    
	colored_vertices_button = new wxCheckBox(ThePanel, ID_COLORED_VERTICES_BUTTON, wxT("Colored vertices"));
	colored_vertices_button->SetToolTip(colored_vertices_tooltip);

	colored_edges_button = new wxCheckBox(ThePanel, ID_COLORED_EDGES_BUTTON, wxT("Colored edges"));
	colored_edges_button->SetToolTip(colored_edges_tooltip);

	outfile_frame = new wxStaticBox(ThePanel, ID_OUTFILE_FRAME, wxT("Output "));

	outfile_statictext = new wxStaticText(ThePanel, ID_OUTFILE_STATICTEXT, wxT("Output file:"));
	outfile_statictext->SetToolTip(outfile_tooltip);

	outfile_ctrl = new wxTextCtrl(ThePanel, ID_OUTFILE_CTRL, wxT(""));
	outfile_ctrl->SetToolTip(outfile_tooltip);

	outfile_button = new wxBitmapButton(ThePanel, ID_OUTFILE_BUTTON, openfile_button_BITMAP, wxDefaultPosition, wxSize(25,25), wxBU_AUTODRAW, wxDefaultValidator, wxT("outfile_button"));
	outfile_button->SetToolTip(outfile_tooltip);

	textout_button = new wxRadioButton(ThePanel, ID_TEXTOUT_BUTTON, wxT("ASCII-Text"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP, wxDefaultValidator, wxT("textout_button"));
    textout_button->SetValue(true);
	textout_button->SetToolTip(textout_tooltip);

	csv_button = new wxRadioButton(ThePanel, ID_CSV_BUTTON, wxT("CSV"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxT("csv_button"));
	csv_button->SetToolTip(csv_tooltip);

	dump_checkbox = new wxCheckBox(ThePanel, ID_DUMP_CHECKBOX, wxT("Dump subgraphs in file"));
	dump_checkbox->SetToolTip(dump_tooltip);
//TO DO

	algorithm_box = new wxStaticBox(ThePanel, ID_ALGORITHM_STATICTEXT, wxT("ALGORITHM"));
	algorithm_box->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE));
    algorithm_box->SetOwnForegroundColour(wxColour("black"));

	start_button = new wxButton(ThePanel, ID_START_BUTTON, wxT("Start"));
	start_button->SetToolTip(start_button_tooltip);

	totalprogress_statictext = new wxStaticText(ThePanel, ID_TOTALPROGRESS_STATICTEXT, wxT("Total Progress:"));
	totalprogress_statictext->SetToolTip(progress_bar_tooltip);

	total_gauge = new wxGauge(ThePanel, ID_TOTAL_GAUGE, 100, wxDefaultPosition, wxSize(150,17), wxGA_HORIZONTAL | wxGA_SMOOTH, wxDefaultValidator, wxT("total_gauge"));
	total_gauge->SetRange(100);
	total_gauge->SetValue(0);
	total_gauge->SetToolTip(progress_bar_tooltip);

	progresstext = new wxStaticText(ThePanel, ID_PROGRESSTEXT, wxT("Progress original network:"), wxDefaultPosition, wxDLG_UNIT(ThePanel, wxSize(130, -1)));
	progresstext->SetToolTip(progress_bar_tooltip);
 
	total_perc_text = new wxStaticText(ThePanel, wxID_ANY, wxT("0%"), wxDefaultPosition, wxSize(50, -1));
	total_perc_text->SetToolTip(progress_bar_tooltip);

	actual_gauge = new wxGauge(ThePanel, ID_ACTUAL_GAUGE, 100, wxDefaultPosition, wxSize(150,17), wxGA_HORIZONTAL | wxGA_SMOOTH, wxDefaultValidator, wxT("actual_gauge"));
	actual_gauge->SetRange(100);
	actual_gauge->SetValue(0);
	actual_gauge->SetToolTip(progress_bar_tooltip);

	actual_perc_text = new wxStaticText(ThePanel, wxID_ANY, wxT("0%"), wxDefaultPosition, wxSize(50, -1));
	actual_perc_text->SetToolTip(progress_bar_tooltip);

    fixed_time_text = new wxStaticText(ThePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1));
    time_text = new wxStaticText(ThePanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1));

	results_box = new wxStaticBox(ThePanel, ID_RESULTS_STATICTEXT, wxT("RESULTS"));
	results_box->SetFont(wxFont(12, wxSWISS, wxNORMAL, wxBOLD, FALSE));
    results_box->SetOwnForegroundColour(wxColour("black"));

	result_button = new wxButton(ThePanel, ID_RESULT_BUTTON, wxT("HTML-Export"));
	result_button->SetToolTip(detailed_view_tooltip);

	load_button = new wxButton(ThePanel, ID_LOAD_BUTTON, wxT("Load file"));
	load_button->SetToolTip(loadfile_tooltip);

	result_ctrl = new wxTextCtrl(ThePanel, ID_RESULT_CTRL, wxT(""), wxDefaultPosition, wxSize(-1,110), wxTE_READONLY | wxTE_MULTILINE, wxDefaultValidator, wxT("result_ctrl"));
	result_ctrl->AppendText(wxT("Results summary will be shown here..."));

	about_button = new wxButton(ThePanel, ID_ABOUT_BUTTON, wxT("About"));

	exit_button = new wxButton(ThePanel, ID_EXIT_BUTTON, wxT("Exit"));

	// Start sizer declaration
	// Main Sizer (divides in vertical: Setup, Algorithm, Result and buttons)
    wxBoxSizer  *topSizer = new wxBoxSizer( wxVERTICAL );
        // Sizer that ensures enlargement of the Setup static box
        wxStaticBoxSizer *setupboxSizer = new wxStaticBoxSizer(setup_box, wxHORIZONTAL);
        // Flexgrid sizer for the four option boxes
        wxFlexGridSizer *setupSizer = new wxFlexGridSizer(2,2,0,0);
            // Sizer for the "Algorithm options" box
            wxStaticBoxSizer *optionsSizer = new wxStaticBoxSizer(options_box, wxVERTICAL);
                // Sizer to hold "Subgraph size" and "samples" in the same row
                wxBoxSizer *sub_grSizer = new wxBoxSizer(wxHORIZONTAL);
                // Sizer to keep Probability fields in the same row
                wxBoxSizer *probSizer = new wxBoxSizer(wxHORIZONTAL);
            // Sizer for the infile box
            wxStaticBoxSizer *infileSizer = new wxStaticBoxSizer(infile_frame, wxVERTICAL);
                // Sizer to hold input field and button for infile in the same row.
                wxBoxSizer *infileHSizer = new wxBoxSizer(wxHORIZONTAL);
            // Sizer that ensures enlargement of the "Random networks" static box
            wxStaticBoxSizer *rnetsSizer = new wxStaticBoxSizer(rnets_box, wxVERTICAL);
                // Grid bag sizer that holds all elements of the R-Net box
                wxGridBagSizer *rnetsGBSizer = new wxGridBagSizer(0,0);
            // Same like the infile sizer
            wxStaticBoxSizer *outfileSizer = new wxStaticBoxSizer(outfile_frame, wxVERTICAL);
                wxBoxSizer *outfileHSizer = new wxBoxSizer(wxHORIZONTAL);
        // Sizer that ensures enlargement of the Algorithm static box
        wxStaticBoxSizer *algorithmSizer = new wxStaticBoxSizer(algorithm_box, wxHORIZONTAL);
            // Grid bag sizer that holds all elements of the Algorithm box
            wxGridBagSizer *progressSizer = new wxGridBagSizer(0,0);
        // Sizer that holds the elements of the Results box
        wxStaticBoxSizer *resultsSizer = new wxStaticBoxSizer(results_box, wxHORIZONTAL);
            // Sizer that keeps the "detailed view" and "load file" button above each other.
            wxBoxSizer  *resButtonSizer = new wxBoxSizer( wxVERTICAL );
        // Sizer that holds the about and exit buttons    
        wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Configure sizers
    sub_grSizer->Add(subgr_statictext, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    sub_grSizer->Add(subgr_size_box, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 20); // keep some space to the Samples text
    sub_grSizer->Add(samples_statictext, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    sub_grSizer->Add(treesamples_ctrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    probSizer->Add(sampling_button, 0, wxALIGN_CENTER_VERTICAL | wxALL, 0); // No border, so the contents are inline with the "full enum" button
    for (int i=0; i<8; ++i)
        probSizer->Add(param_ctrls[i], 1, wxGROW | wxALL, 3); // Prob fields may become larger
    optionsSizer->Add(sub_grSizer, 0, wxGROW | wxALL, 3);
    optionsSizer->Add(full_enum_button, 0, wxALL, 3);
    optionsSizer->Add(probSizer, 0, wxGROW | wxALL, 3);
    infileHSizer->Add(infile_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 3); // the infile ctrl is getting all the remaining space
    infileHSizer->Add(openfile_button, 0, wxALL, 3);
    infileSizer->Add(infile_statictext, 0, wxALL, 3);
    infileSizer->Add(infileHSizer, 0, wxGROW, 0); // the horizontal sizer may grow to enlarge the infile ctrl
    infileSizer->Add(directed_button, 0, wxALL, 3);
    infileSizer->Add(colored_vertices_button, 0, wxALL, 3);
    infileSizer->Add(colored_edges_button, 0, wxALL, 3);
    
    rnetsGBSizer->Add(rtype_statictext, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    rnetsGBSizer->Add(rtype_box, wxGBPosition(0, 1), wxDefaultSpan, wxALL, 3);
    rnetsGBSizer->Add(regard_vc_box, wxGBPosition(1, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxALL, 3); 
    rnetsGBSizer->Add(regard_ec_box, wxGBPosition(2, 0), wxGBSpan(1, 3), wxALIGN_TOP | wxALL, 3); // align top to group the two regard buttons slightly together
    rnetsGBSizer->Add(reest_subgr_box, wxGBPosition(3, 0), wxGBSpan(1, 3), wxALIGN_CENTER_VERTICAL | wxALL, 3); // the reest-button spans three columns
    rnetsGBSizer->Add(rnets_statictext, wxGBPosition(0, 3), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    rnetsGBSizer->Add(rnets_ctrl, wxGBPosition(0, 4), wxDefaultSpan, wxALL, 3);
    rnetsGBSizer->Add(exchanges_statictext, wxGBPosition(1, 3), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    rnetsGBSizer->Add(exchanges_ctrl, wxGBPosition(1, 4), wxDefaultSpan, wxALL, 3);
    rnetsGBSizer->Add(attempts_statictext, wxGBPosition(2, 3), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    rnetsGBSizer->Add(attempts_ctrl, wxGBPosition(2, 4), wxDefaultSpan, wxALL, 3);
    // column 2 remains empty to give some space between the options
    rnetsSizer->Add(rnetsGBSizer, 1, wxGROW, 0); // the GB sizer may take all the space of its box        
    outfileHSizer->Add(outfile_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    outfileHSizer->Add(outfile_button, 0, wxALL, 3);
    outfileSizer->Add(outfile_statictext, 0, wxALL, 3);
    outfileSizer->Add(outfileHSizer, 0, wxGROW, 0);
    outfileSizer->Add(textout_button, 0, wxALL, 3);
    outfileSizer->Add(csv_button, 0, wxALL, 3);
    outfileSizer->Add(dump_checkbox, 0, wxALL, 3);
    setupSizer->AddGrowableCol(0, 3); // The first column grows 3 times as fast as the second.
    setupSizer->AddGrowableCol(1, 1);    
    setupSizer->Add(optionsSizer, 0, wxGROW | wxRIGHT | wxBOTTOM, 3); // all the sizers are allowed to grow
    setupSizer->Add(infileSizer, 0, wxGROW | wxLEFT | wxBOTTOM, 3);   // different borders set the desired gaps
    setupSizer->Add(rnetsSizer, 0, wxGROW | wxRIGHT, 3);
    setupSizer->Add(outfileSizer, 0, wxGROW | wxLEFT, 3);
    setupboxSizer->Add(setupSizer, 1, wxGROW, 0);
    // Here starts the progress-bar sizer. It's a bit complicated:
                                         // Col0 contains the button, it's static
    progressSizer->AddGrowableCol(1, 1); // Col1 is an spacer, growing slowly
                                         // Col2 contains the texts, it's static
    progressSizer->AddGrowableCol(3, 4); // Col3 contains the progressbars, growing fast
    progressSizer->AddGrowableCol(4, 1); // Col4 is a spacer, growing slowly
                                         // Col5 contains the percentages, it's static
    progressSizer->AddGrowableCol(6, 1); // Col6 is a spacer, growing slowly
    progressSizer->AddGrowableCol(7, 4); // Col7 contains the elapsed time, growing fast
    progressSizer->Add(start_button, wxGBPosition(0, 0), wxGBSpan(2,1), wxALIGN_CENTER_VERTICAL | wxALL, 3); // the button spans the two rows
    progressSizer->Add(totalprogress_statictext, wxGBPosition(0, 2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL, 0); // the texts are vertically centered with the gauges
    progressSizer->Add(total_gauge, wxGBPosition(0, 3), wxDefaultSpan, wxGROW | wxALL, 3);
    progressSizer->Add(total_perc_text, wxGBPosition(0, 5), wxDefaultSpan, wxALL, 3);
    progressSizer->Add(progresstext, wxGBPosition(1, 2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxRIGHT, 3);
    progressSizer->Add(actual_gauge, wxGBPosition(1, 3), wxDefaultSpan, wxGROW | wxALL, 3);
    progressSizer->Add(actual_perc_text, wxGBPosition(1, 5), wxDefaultSpan, wxALL, 3);
    progressSizer->Add(fixed_time_text, wxGBPosition(0, 7), wxDefaultSpan, wxGROW | wxALL, 3);
    progressSizer->Add(time_text, wxGBPosition(1, 7), wxDefaultSpan, wxGROW | wxALL, 3);
    algorithmSizer->Add(progressSizer, 1, wxALIGN_CENTER_VERTICAL, 0); // The buttons are vertically centered in the results-box
    resButtonSizer->Add(result_button, 0, wxGROW | wxALL, 3); // These Buttons may grow, so they have same size
    resButtonSizer->Add(load_button, 0, wxGROW | wxALL, 3);
    resultsSizer->Add(resButtonSizer, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    resultsSizer->Add(result_ctrl, 1, wxGROW | wxALL, 3);
    buttonSizer->AddStretchSpacer(1); //Spacer left of about button
    buttonSizer->Add(about_button, 0, wxALL, 3);
    buttonSizer->Add(exit_button, 0, wxALL, 3);    
    topSizer->Add(setupboxSizer, 0, wxGROW | wxALL, 3);
    topSizer->Add(algorithmSizer, 0, wxGROW | wxALL, 3);
    topSizer->Add(resultsSizer, 1, wxGROW | wxALL, 3);
    topSizer->Add(buttonSizer, 0, wxGROW | wxALL, 3);
    
    // Set sizers
    ThePanel->SetSizer( topSizer ); // use the sizer for layout
    topSizer->Fit( this );          // fit the dialog to the contents
    topSizer->SetSizeHints( this ); // set hints to honor min size

    // Make the window a little wider
    int width, height;
    this->GetSize(&width, &height);
	this->SetSize(width + 30, -1);

	this->SetTitle(program_name);
	this->Center();
	this->SetIcon(wxIcon(icon_xpm));

}

//randesuFrmClose
void randesuFrm::randesuFrmClose(wxCloseEvent& event)
{
    if (thread_exists) { // Delete the thread if it exists
         if (thread_paused){ // If the thread is paused, resume it to avoid "double paused" threads
             thread->Resume();
             watch->Resume();
             thread_paused = false;         
             start_button->SetLabel(wxT("Pause"));
         }
         thread->Pause(); // Thread and watch pause while the user is asked
         watch->Pause();
         wxMessageDialog dialog(this, wxT("Exiting will abort the current search.\nAre you sure?"), wxT("Really quit?"),
                                wxYES_NO|wxICON_QUESTION);
         if (dialog.ShowModal()==wxID_NO){ //Do nothing if user clicked no
             thread->Resume(); // Resume thread and watch
             watch->Resume();
             return;
         }    
         thread->Delete();
    }
    Destroy(); // Destroy the window
}

//exit_buttonClick
void randesuFrm::exit_buttonClick(wxCommandEvent& event)
{
    if (thread_exists){
         if (thread_paused){ // If the thread is paused, resume it to avoid "double paused" threads
             thread->Resume();
             watch->Resume();
             thread_paused = false;         
             start_button->SetLabel(wxT("Pause"));
         }
         thread->Pause(); // thread and watch pause while the user is asked
         watch->Pause();
         wxMessageDialog dialog(this, wxT("This will abort the current search.\nAre you sure?"), wxT("Really cancel?"),
                                wxYES_NO|wxICON_QUESTION);
         if (dialog.ShowModal()==wxID_NO){ //Do nothing if user clicked no
             thread->Resume(); // Resume thread and watch
             watch->Resume();
             return;
         }    

        // Destroy thread
        thread->Delete();
        thread_paused = false;        
        thread_exists = false;
                
    } else { // Exit the Program
        Destroy();
    }
}

//about_buttonClick
void randesuFrm::about_buttonClick(wxCommandEvent& event)
{
    aboutDlg *dialog = new aboutDlg(this);
    dialog->ShowModal();
}

//openfile_buttonClick
void randesuFrm::openfile_buttonClick(wxCommandEvent& event)
{
    wxFileDialog * openfile_dialog =  new wxFileDialog(this, wxT("Select graph file"), wxT(""), wxT(""), wxT("Text files|*.txt|All files|*.*"), wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);
    if (openfile_dialog->ShowModal() == wxID_OK)
        infile_ctrl->SetValue(openfile_dialog->GetPath());
        infile_ctrl->SetInsertionPointEnd();	
}

//infile_ctrlUpdated
void randesuFrm::infile_ctrlUpdated(wxCommandEvent& event)
{
    //if (outfile_ctrl->GetValue() == ""){ //If the user hasn't yet defined an outfile
    // Unfortunately this if does not distinguish between the user and my program...
        outfile_ctrl->SetValue(infile_ctrl->GetValue()+(csv_button->GetValue()?wxT(".csv"):wxT(".OUT")));
        outfile_ctrl->SetInsertionPointEnd();
    //}    
}

//outfile_buttonClick
void randesuFrm::outfile_buttonClick(wxCommandEvent& event)
{
	wxFileDialog * outfile_dialog =  new wxFileDialog(this, wxT("Choose an output file"), wxT(""), wxT(""), wxT("*.*"), wxSAVE | wxCHANGE_DIR);
    if (outfile_dialog->ShowModal() == wxID_OK){
        outfile_ctrl->SetValue(outfile_dialog->GetPath());
        outfile_ctrl->SetInsertionPointEnd();
    }    
}

//load_buttonClick
void randesuFrm::load_buttonClick(wxCommandEvent& event)
{
	wxFileDialog * resfile_dialog =  new wxFileDialog(this, wxT("Choose a result file"), wxT(""), wxT(""), wxT("*.*"), wxOPEN | wxFILE_MUST_EXIST | wxCHANGE_DIR);
    if (resfile_dialog->ShowModal() == wxID_OK){
        resultfile = resfile_dialog->GetPath(); // Set the result file as the selected file
        result_ctrl->SetValue(GetSummaryFromFile(resultfile)); //Display the summary
    }    
}

// result_buttonClick
void randesuFrm::result_buttonClick(wxCommandEvent& event)
{
    // Show error if the result file does not exist
    if (resultfile.empty()) {
         wxMessageDialog dialog(this, wxT("The resultfile is not set.\nUse the \"Load file\" button to choose a file."), wxT("File not set"),
                                wxOK|wxICON_ERROR);
         dialog.ShowModal();
         return;    
    }
    if (!wxFileName(resultfile).FileExists()) {
         wxMessageDialog dialog(this, wxT("The resultfile does not exist!"), wxT("File not found"),
                                wxOK|wxICON_ERROR);
         dialog.ShowModal();
         return;
    }
    ResultDialog *myResDialog = new ResultDialog(this, resultfile);
    myResDialog->ShowModal();
}

//full_enum_buttonClick
void randesuFrm::full_enum_buttonClick(wxCommandEvent& event)
{
    for (int i=0; i<8; i++){
        param_ctrls[i]->Disable();
    }
}

//sampling_buttonClick
void randesuFrm::sampling_buttonClick(wxCommandEvent& event)
{
    long subgr_size = 0;
    subgr_size_box->GetValue().ToLong(&subgr_size);
    for (long i=0; i< subgr_size; ++i){
        param_ctrls[i]->Enable();
    }    
}

//subgr_sizeUpdated
void randesuFrm::subgr_sizeUpdated(wxCommandEvent& event)
{
    if (sampling_button->GetValue()){ // If sampling button is activated
        long subgr_size = 0;
        subgr_size_box->GetValue().ToLong(&subgr_size);
        long i=0;
        for (; i< subgr_size; ++i){
            param_ctrls[i]->Enable();
        }
        for (; i<8; ++i){
            param_ctrls[i]->Disable();
        }    
    }    
}

// directed_buttonClick
void randesuFrm::directed_buttonClick(wxCommandEvent& event)
{
    if (directed_button->IsChecked()){
        rtype_box->Insert(wxT("No regard"), 0); // Add the items to the list.
        rtype_box->Insert(wxT("Global const"), 1);
        rtype_box->SetSelection(2);
    } else {
        rtype_box->Delete(0); // Delete No regard and Global const items from list.
        rtype_box->Delete(0);
        rtype_box->SetSelection(0);    
    }
           
}

// ec_boxClick
void randesuFrm::ec_boxClick(wxCommandEvent& event)
{
    if (colored_edges_button->IsChecked()){
        regard_ec_box->SetValue(true);   
        regard_ec_box->Enable();
    } else {
        regard_ec_box->SetValue(false);   
        regard_ec_box->Disable();
    }
           
}

// vc_boxClick
void randesuFrm::vc_boxClick(wxCommandEvent& event)
{
    if (colored_vertices_button->IsChecked()){
        regard_vc_box->SetValue(true); 
        regard_vc_box->Enable();
    } else {
        regard_vc_box->SetValue(false); 
        regard_vc_box->Disable();
    }
           
}

// vc_boxClick
void randesuFrm::dump_boxClick(wxCommandEvent& event)
{
    if (dump_checkbox->IsChecked()){
        dump_checkbox->SetValue(true); 
    } else {
        dump_checkbox->SetValue(false); 
    }
           
}

// csv_buttonClick
void randesuFrm::csv_buttonClick(wxCommandEvent& event)
{
    if ((outfile_ctrl->GetValue()).empty()) return;
    wxFileName outfile(outfile_ctrl->GetValue());
    // Case insensitive comparison of outfile-ext. with out
    if (outfile.GetExt().IsSameAs(wxT("out"),false)) {
        outfile.SetExt(wxT("csv"));
        outfile_ctrl->SetValue(outfile.GetFullPath());
        outfile_ctrl->SetInsertionPointEnd();
    }
}

// textout_buttonClick
void randesuFrm::textout_buttonClick(wxCommandEvent& event)
{
    if ((outfile_ctrl->GetValue()).empty()) return;
    wxFileName outfile(outfile_ctrl->GetValue());
    // Case insensitive comparison of outfile-ext. with csv
    if (outfile.GetExt().IsSameAs(wxT("csv"),false)) {
        outfile.SetExt(wxT("OUT"));
        outfile_ctrl->SetValue(outfile.GetFullPath());
        outfile_ctrl->SetInsertionPointEnd();
    }
}

/********************
 * start_buttonClick*
 ********************/
void randesuFrm::start_buttonClick(wxCommandEvent& event)
{
     if (!thread_exists){
         // Create the algorithm vars, set default values                
         wxString messageStr = wxT(""); // String for error-messages
         long G_N = 4;
         long treesmpls = 100000;
         bool fullenumeration = full_enum_button->GetValue();
         bool gen_dumpfile = dump_checkbox->GetValue();
         double prob[] = { 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5 };
         wxString infile = infile_ctrl->GetValue();
         bool directed = directed_button->GetValue();
         // Randomtype should be Local_const if graph is undir
         short random_type = directed?rtype_box->GetSelection():2;
         long num_r_nets = 0, num_exchanges = 3, num_tries = 3;
         // to ensure that vertex/edge color does not matter, if there is none, the two following vars are used
         bool vertex_color_matters = regard_vc_box->GetValue() && colored_vertices_button->GetValue();
         bool edge_color_matters = regard_ec_box->GetValue() && colored_edges_button->GetValue();
         bool reest_size = reest_subgr_box->GetValue();    
         bool text_output = textout_button->GetValue();

         // Get values, Check for correct values
         subgr_size_box->GetValue().ToLong(&G_N);
         if (!treesamples_ctrl->GetValue().ToLong(&treesmpls)
             || treesmpls < 0 || treesmpls > 10000000)
             messageStr += wxT("Number of samples should be between 0 and 1e7\n");
         if (!fullenumeration) {
             bool error_generated = false; // To avoid printing the error message to often
             for (int i=0; i < G_N; ++i)
                 if ((!param_ctrls[i]->GetValue().ToDouble(&prob[i]) ||
                     prob[i] < 0.0 || prob[i] > 1.0)
                     && !error_generated) {
                     messageStr += wxT("Sampling probabilities should be between 0 and 1\n");
                     error_generated = true;
                 }
         }
         if (!wxFileName::FileExists(infile))
             messageStr += wxT("Choosen graph file does not exist\n");
         //GetValue().ToLong tries to convert the String to long values
         // if this fails, the error is added
         if (!rnets_ctrl->GetValue().ToLong(&num_r_nets) || num_r_nets < 0)
             messageStr += wxT("Number of random networks should be an positive integer\n");
         if (!exchanges_ctrl->GetValue().ToLong(&num_exchanges) || num_exchanges < 0)
             messageStr += wxT("Exchanges should be an positive integer\n");
         if (!attempts_ctrl->GetValue().ToLong(&num_tries) || num_exchanges < 0)
             messageStr += wxT("Attempts should be an positive integer\n");
         if (!wxFileName(outfile_ctrl->GetValue()).IsOk())
             messageStr += wxT("Outfile name is not valid\n");
         
         // An error occured: abort
         if (!messageStr.empty()){
             wxMessageDialog dialog(this, messageStr, wxT("Please correct the following errors:"),
                                    wxOK|wxICON_ERROR);
             dialog.ShowModal();
             return;
         }

         // Reset gauge values
         nets_done = 0;
         total_nets = int(num_r_nets)+1;
         total_gauge->SetValue(0);
         actual_gauge->SetValue(0);
         progresstext->SetLabel(wxT("Progress original network:"));
         actual_perc_text->SetLabel(wxT("0%"));
         total_perc_text->SetLabel(wxT("0%"));
                  
         // Set resultfile var
         resultfile = outfile_ctrl->GetValue();
         // Ask if file exists
         if (wxFileName(resultfile).FileExists()){
             //result_ctrl->SetValue(GetSummaryFromFile(resultfile));                                                  
             wxMessageDialog dialog(this, wxT("Chosen output file exists!\nOverwrite?"), wxT("Overwrite prompt"),
                                    wxYES_NO|wxNO_DEFAULT|wxICON_QUESTION);
             if (dialog.ShowModal()==wxID_NO){ //Abort if user clicked no
                 return;
             }    
         }
         // Deactivate controls
         SwitchControls(false);
                  
         //Start the timer
         fixed_time_text->SetLabel(wxT("Time elapsed:"));
         time_text->SetLabel(wxT("00:00:00"));         
         timer->Start(1000); //tick every second
         watch->Start();

         // Start the thread
         thread = new esuThread(infile, directed, colored_vertices_button->GetValue(),
                                colored_edges_button->GetValue(),
                                G_N, fullenumeration, prob, treesmpls, num_r_nets, 
                                random_type, vertex_color_matters,
                                edge_color_matters,
                                num_exchanges, num_tries, reest_size,
                                resultfile, text_output, gen_dumpfile, this);
         // Create the thread                       
         if ( thread->Create() != wxTHREAD_NO_ERROR )
         {   // Show error dialog if sth went wrong
             wxMessageDialog dialog(this, wxT("Could not create thread"), wxT("Internal problem"),
                                    wxOK|wxICON_ERROR);
             dialog.ShowModal();
             thread->Delete();
             return;
         }
         // Run the thread
         if ( thread->Run() != wxTHREAD_NO_ERROR )
         {   // Show error dialog if sth went wrong
             wxMessageDialog dialog(this, wxT("Could not run thread"), wxT("Internal problem"),
                                    wxOK|wxICON_ERROR);
             dialog.ShowModal();
             thread->Delete();
             return;
         }
         thread_exists = true;
         
         // Change the buttons
         start_button->SetLabel(wxT("Pause"));
         exit_button->SetLabel(wxT("Cancel"));
         
     } else if (thread_paused){
         // Resume thread
         if ( thread->Resume() != wxTHREAD_NO_ERROR )
         {   // Show error dialog if sth went wrong
             wxMessageDialog dialog(this, wxT("Could not resume thread"), wxT("Internal problem"),
                                    wxOK|wxICON_ERROR);
             dialog.ShowModal();
             return;
         }
         watch->Resume();
         start_button->SetLabel(wxT("Pause"));
         thread_paused = false;
         
     } else {        // Thread running
         // Pause thread
         if ( thread->Pause() != wxTHREAD_NO_ERROR )
         {   // Show error dialog if sth went wrong
             wxMessageDialog dialog(this, wxT("Could not pause thread"), wxT("Internal problem"),
                                    wxOK|wxICON_ERROR);
             dialog.ShowModal();
             return;
         }
         watch->Pause();
         start_button->SetLabel(wxT("Resume"));
         thread_paused = true;
     }
}


//threadFinished
void randesuFrm::threadFinished(wxCommandEvent& event)
{
    // Destroy Thread
    timer->Stop();
    thread_exists = false;

    // Fill up progress bars
    wxString progress_str = wxT("Progress network ");
    progress_str << total_nets-1 << wxT(" of ") << total_nets-1 << wxT(":");
    progresstext->SetLabel(progress_str);
    actual_gauge->SetValue(100);
    actual_perc_text->SetLabel(wxT("100%"));
    total_gauge->SetValue(100);
    total_perc_text->SetLabel(wxT("100%"));
    
    // Change Buttons
    start_button->SetLabel(wxT("Start"));
    exit_button->SetLabel(wxT("Exit"));    
    this->SetTitle(program_name);
    
    // Reactivate controls
    SwitchControls(true);

    // Display result summary
    result_ctrl->SetValue(GetSummaryFromFile(resultfile));
    // Notify the user by blinking taskbar icon
    RequestUserAttention();
}

// threadNoSuccess is called, when the thread was cancelled
void randesuFrm::threadNoSuccess(wxCommandEvent& event)
{
    // Destroy Thread
    timer->Stop();
    thread_exists = false;
    
    // Change Buttons
    start_button->SetLabel(wxT("Start"));
    exit_button->SetLabel(wxT("Exit"));    
    this->SetTitle(program_name);
    
    // Reactivate controls
    SwitchControls(true);

    resultfile = wxT("");
    result_ctrl->SetValue(wxT("The motif search was aborted"));    
}

// percentReached 
void randesuFrm::percentReached(wxCommandEvent& event)
{
    // Increase the perc by value of the event
    int percentage = actual_gauge->GetValue() + event.GetInt();
    // Update the gauge
    actual_gauge->SetValue(percentage);
    actual_perc_text->SetLabel(wxString()<< percentage << wxT("%"));
    
    if (total_nets < 100){
        int total_perc = nets_done*100/total_nets + percentage/total_nets; 
        total_gauge->SetValue(total_perc);
        total_perc_text->SetLabel(wxString()<< total_perc << wxT("%"));
        this->SetTitle(wxString()<<total_perc<<wxT("% - ")<<program_name);
    }
}

// netFinished
void randesuFrm::netFinished(wxCommandEvent& event)
{
    // Increase the number of nets that are finished
    nets_done += event.GetInt();
    
    // Fill up the actual gauge
    actual_gauge->SetValue(100);
    actual_perc_text->SetLabel(wxT("100%"));
    
    // Update the total gauge
    int total_perc = nets_done*100/total_nets;
    if (total_perc > 100) total_perc = 100;
    total_gauge->SetValue(total_perc);
    total_perc_text->SetLabel(wxString()<< total_perc << wxT("%"));
    this->SetTitle(wxString()<<total_perc<<wxT("% - ")<<program_name);

    if (nets_done < total_nets){ //To prevent change when the last net is done
        //Reset the actual gauge
        actual_gauge->SetValue(0);
        actual_perc_text->SetLabel(wxT("0%"));
        //Put new string in front of gauge
        wxString progress_str = wxT("Progress network ");
        progress_str << nets_done << wxT(" of ") << total_nets-1 << wxT(":");
        progresstext->SetLabel(progress_str);
    }
}

// OnTimer
void randesuFrm::OnTimer(wxTimerEvent& event)
{
    // Get the time from the stopwatch, format it and pass it to time_text.
    time_text->SetLabel(wxTimeSpan(0,0,0,watch->Time()).Format(wxT("%H:%M:%S")));
}

// ThreadInfo
void randesuFrm::threadInfo(wxCommandEvent& event)
{
    result_ctrl->SetValue(event.GetString());
    result_ctrl->SetInsertionPointEnd();
}

// Disables or enables all controls the user may not press during calculation
void randesuFrm::SwitchControls(bool enable)
{
    subgr_statictext->Enable(enable);
    subgr_size_box->Enable(enable);
    samples_statictext->Enable(enable);
    treesamples_ctrl->Enable(enable);
    full_enum_button->Enable(enable);
    sampling_button->Enable(enable);
    if (sampling_button->GetValue()){
        long subgr_size = 0;
        subgr_size_box->GetValue().ToLong(&subgr_size);
        for (long i=0; i< subgr_size; ++i){
            param_ctrls[i]->Enable(enable);
        }
    }
    rtype_statictext->Enable(enable);
    rtype_box->Enable(enable);
    if (colored_edges_button->IsChecked())
        regard_ec_box->Enable(enable);
    if (colored_vertices_button->IsChecked())
        regard_vc_box->Enable(enable);
    reest_subgr_box->Enable(enable);
    rnets_statictext->Enable(enable);
    rnets_ctrl->Enable(enable);
    exchanges_statictext->Enable(enable);
    exchanges_ctrl->Enable(enable);
    attempts_statictext->Enable(enable);
    attempts_ctrl->Enable(enable);
    infile_statictext->Enable(enable);
    infile_ctrl->Enable(enable);
    openfile_button->Enable(enable);
    directed_button->Enable(enable);
    colored_vertices_button->Enable(enable);
    colored_edges_button->Enable(enable);
    outfile_statictext->Enable(enable);
    outfile_ctrl->Enable(enable);
    textout_button->Enable(enable);
    csv_button->Enable(enable);
    openfile_button->Enable(enable);
    outfile_button->Enable(enable);
    result_button->Enable(enable);
    load_button->Enable(enable);
    about_button->Enable(enable);
    
    
}
