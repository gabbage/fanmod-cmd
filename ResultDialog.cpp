//---------------------------------------------------------------------------
//
// Name:        ResultDialog.cpp
// Author:      Florian Rasche
// Created:     19.10.2005 11:37:57
//
//---------------------------------------------------------------------------

#include "ResultDialog.h"
#include "tooltips.cpp"
#include "openfile_button_XPM.xpm"
#include "icon.xpm"

// This is not nice but somehow the rest fails...
sort_constants sorted_by; // by which column is the array actually sorted
bool undef_bottom;

//----------------------------------------------------------------------------
// ResultDialog
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ResultDialog,wxDialog)
	EVT_BUTTON(ID_GENERATE_BUTTON, ResultDialog::generate_buttonClick)
	EVT_BUTTON(ID_FILE_BUTTON,ResultDialog::file_buttonClick)
    EVT_CHECKBOX(ID_Z_SCORE_TEXT,ResultDialog::z_score_textClick)
    EVT_CHECKBOX(ID_P_VALUE_TEXT,ResultDialog::p_value_textClick)
    EVT_CHECKBOX(ID_FREQUENCY_TEXT,ResultDialog::frequency_textClick)
    EVT_CHECKBOX(ID_NUMBER_TEXT,ResultDialog::number_textClick)
    EVT_CHECKBOX(ID_DANGLING_TEXT,ResultDialog::dangling_textClick)
	EVT_COMMAND_RANGE(ID_VERTEX_COLORS, ID_VERTEX_COLORS_END, wxEVT_COMMAND_BUTTON_CLICKED,
       ResultDialog::ChooseVertexColor)
	EVT_COMMAND_RANGE(ID_EDGE_COLORS, ID_EDGE_COLORS_END, wxEVT_COMMAND_BUTTON_CLICKED,
       ResultDialog::ChooseEdgeColor)
    EVT_CLOSE(ResultDialog::ResultDialogClose)
END_EVENT_TABLE()


ResultDialog::ResultDialog( wxWindow *parent, const wxString & resultfile_p)
    : wxDialog( parent, wxID_ANY, wxT("HTML-Export options"), wxDefaultPosition, wxDefaultSize, ResultDialog_STYLE),
      resultfile(resultfile_p)
{
    htmlfile = resultfile;
    htmlfile.ClearExt();
    htmlfile = wxFileName(htmlfile.GetFullPath()); // This is done to clear an second extension
    htmlfile.SetExt(wxT("html"));
    G_N = 0; //Size of subgraphs in resultfile
	number_v_colors = 0;
	number_e_colors = 0;
    sorted_by = freq; // default: sorted by frequency
    pics = true; // pictures are drawn by default
    ignore_undef = false; // undefined Z-Scores should not be ignored
    undef_bottom = false; // undefined Z-Scores should be on top of list
    summary1 = ""; 
    summary2 = "";	
	number_of_subgraphs = 0;
	vertex_pens = new wxPen[16];
	vertex_brushes = new wxBrush[16];
    edge_pens = new wxPen[7];
    edge_brushes = new wxBrush[7];

//wxLogMessage("Start reading file");
    if (!ReadResults()) // Only if reading was successful, the rest should be done.
        this->Destroy(); // if reading was not successful, destroy the window.
//wxLogMessage("Done reading file");
    InitColors();
    CreateGUIControls();
}

ResultDialog::~ResultDialog() {
    delete[] vertexColorButtons;
    delete[] edgeColorButtons;
    delete[] vertex_pens;
    delete[] vertex_brushes;
    delete[] edge_pens;
    delete[] edge_brushes;
} 

void ResultDialog::CreateGUIControls()
{
    //GUI Items Creation
    file_box = new wxStaticBox(this, wxID_ANY, wxT("File Options "));
    file_text = new wxStaticText(this, wxID_ANY, wxT("HTML-Output-File(s):"));
    file_ctrl = new wxTextCtrl(this, wxID_ANY, htmlfile.GetFullPath(), wxDefaultPosition, wxSize(160,-1));
    file_ctrl->SetToolTip(htmlfile_tooltip);
    
    wxBitmap file_button_BITMAP (openfile_button_XPM);
    file_button = new wxBitmapButton(this, ID_FILE_BUTTON, file_button_BITMAP, wxDefaultPosition, wxSize(25,25), wxBU_AUTODRAW);
    file_button->SetToolTip(htmlfile_tooltip);
    
    motifs_text = new wxStaticText(this, wxID_ANY, wxT("Motifs per file:"));
    motifs_ctrl = new wxTextCtrl(this, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize(40,-1));
    motifs_ctrl->SetToolTip(motifs_per_file_tooltip);
    
    pictures = new wxCheckBox(this, wxID_ANY, wxT("Create pictures"));
    pictures->SetValue(true);
    pictures->SetToolTip(create_pictures_tooltip);

    filter_box = new wxStaticBox(this, wxID_ANY, wxT("Filters "));
    z_score_text = new wxCheckBox(this, ID_Z_SCORE_TEXT, wxT("|Z-Score| bigger than"));
    z_score_ctrl  = new wxTextCtrl(this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize(40,-1));
    z_score_text->SetValue(true);
    z_score_text->SetToolTip(z_score_filter_tooltip);
    z_score_ctrl->SetToolTip(z_score_filter_tooltip);

    p_value_text = new wxCheckBox(this, ID_P_VALUE_TEXT, wxT("p-Value less than"));
    p_value_ctrl = new wxTextCtrl(this, wxID_ANY, wxT("0.05"), wxDefaultPosition, wxSize(40,-1));
    p_value_text->SetValue(true);
    p_value_text->SetToolTip(p_value_filter_tooltip);
    p_value_ctrl->SetToolTip(p_value_filter_tooltip);

    frequency_text = new wxCheckBox(this, ID_FREQUENCY_TEXT, wxT("Frequency bigger than"));
    frequency_ctrl  = new wxTextCtrl(this, wxID_ANY, wxT("0.01"), wxDefaultPosition, wxSize(40,-1));
    frequency_ctrl->Disable();
    frequency_text->SetToolTip(frequency_filter_tooltip);
    frequency_ctrl->SetToolTip(frequency_filter_tooltip);
    percentage_text = new wxStaticText(this, wxID_ANY, wxT("%"));

    number_text = new wxCheckBox(this, ID_NUMBER_TEXT, wxT("Motif found at least"));
    number_ctrl  = new wxTextCtrl(this, wxID_ANY, wxT("5"), wxDefaultPosition, wxSize(40,-1));
    number_text->SetValue(true);
    number_text->SetToolTip(number_filter_tooltip);
    number_ctrl->SetToolTip(number_filter_tooltip);
    times_text = new wxStaticText(this, wxID_ANY, wxT("times"));

    dangling_text = new wxCheckBox(this, ID_DANGLING_TEXT, wxT("Ignore motifs that have between"));
    dangling_min_ctrl  = new wxTextCtrl(this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(40,-1));
    dangling_min_ctrl->Disable();
    dangling_middle_text = new wxStaticText(this, wxID_ANY, wxT("and"));
    dangling_max_ctrl = new wxTextCtrl(this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(40,-1));
    dangling_max_ctrl->Disable();
    dangling_end_text = new wxStaticText(this, wxID_ANY, wxT("dangling edges."));
    dangling_text->SetToolTip(dangling_filter_tooltip);
    dangling_min_ctrl->SetToolTip(dangling_filter_tooltip);
    dangling_max_ctrl->SetToolTip(dangling_filter_tooltip);
    
    wxArrayString sort_strings;
    sort_strings.Add(wxT("|Z-Score|"));
    sort_strings.Add(wxT("p-Value"));
    sort_strings.Add(wxT("Frequency     "));
    sort_box = new wxRadioBox(this, ID_SORT_BOX, wxT("Sort by: "), wxDefaultPosition, wxDefaultSize, sort_strings, 1, wxRA_SPECIFY_COLS);
    sort_box->SetToolTip(sort_box_tooltip);

    wxArrayString undef_strings;
    undef_strings.Add(wxT("put at top of list"));
    undef_strings.Add(wxT("put at bottom of list"));
    undef_strings.Add(wxT("ignore"));
    undef_box = new wxRadioBox(this, ID_SORT_BOX, wxT("Motifs not found in random networks: "), wxDefaultPosition, wxDefaultSize, undef_strings, 1, wxRA_SPECIFY_COLS);
    undef_box->SetSelection(1);
    undef_box->SetToolTip(undef_box_tooltip);

    if (number_v_colors != 0) vertexColorBox = new wxStaticBox(this, wxID_ANY, wxT("Vertex Colors "));
    vertexNumbers = new wxStaticText*[16];
    vertexColorButtons = new wxBitmapButton*[16];    
    wxMemoryDC *dc = new wxMemoryDC();
    for (int i=0; i<number_v_colors; ++i){
        vertexNumbers[i] = new wxStaticText(this, wxID_ANY, wxString()<<(i)<< wxT(")"));
        vertexColorButtons[i] = new wxBitmapButton(this, ID_VERTEX_COLORS+i, wxBitmap(20,15), wxDefaultPosition, wxSize(30,25));
        dc->SelectObject(vertexColorButtons[i]->GetBitmapLabel());
        dc->SetBrush(vertex_brushes[i]);
        dc->SetPen(vertex_pens[i]);
        dc->DrawRectangle(0,0,20,15);     
        //vertexColorButtons[i]->Refresh();
    }

    if (number_e_colors != 0) edgeColorBox = new wxStaticBox(this, wxID_ANY, wxT("Edge Colors "));
    edgeNumbers = new wxStaticText*[7];
    edgeColorButtons = new wxBitmapButton*[7];    
    for (int i=0; i<number_e_colors; ++i){
        edgeNumbers[i] = new wxStaticText(this, wxID_ANY, wxString()<<(i+1)<< wxT(")"));
        edgeColorButtons[i] = new wxBitmapButton(this, ID_EDGE_COLORS+i, wxBitmap(20,15), wxDefaultPosition, wxSize(30,25));
        dc->SelectObject(edgeColorButtons[i]->GetBitmapLabel());
        dc->SetBrush(edge_brushes[i]);
        dc->SetPen(edge_pens[i]);
        dc->DrawRectangle(0,0,20,15);     
        //edgeColorButtons[i]->Refresh();
    }
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);        
    dc->SelectObject(wxNullBitmap);


    generate_button = new wxButton(this, ID_GENERATE_BUTTON, wxT("Generate HTML"));
    generate_button->SetToolTip(generate_button_tooltip);
    close_button = new wxButton(this, wxID_CANCEL, wxT("Close"));  

    
	// Sizer Declaration
    wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);
      wxStaticBoxSizer *fileBoxSizer = new wxStaticBoxSizer(file_box, wxVERTICAL);
        wxBoxSizer *fileSizer = new wxBoxSizer(wxHORIZONTAL);
        wxBoxSizer *motifSizer = new wxBoxSizer(wxHORIZONTAL);
      wxGridBagSizer *notFileSizer = new wxGridBagSizer();
        wxStaticBoxSizer *filterBoxSizer = new wxStaticBoxSizer(filter_box, wxVERTICAL);
          wxGridBagSizer *filterSizer = new wxGridBagSizer();
      wxStaticBoxSizer *vertexColorBoxSizer = new wxStaticBoxSizer(vertexColorBox, wxHORIZONTAL);
        wxGridSizer *vertexColorSizer = new wxFlexGridSizer(14);
      wxStaticBoxSizer *edgeColorBoxSizer = new wxStaticBoxSizer(edgeColorBox, wxHORIZONTAL);	  
        wxGridSizer *edgeColorSizer = new wxFlexGridSizer(14);
      wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	
    // Sizer Configuration
    fileSizer->Add(file_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    fileSizer->Add(file_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    fileSizer->Add(file_button, 0, wxALL, 3);
    motifSizer->Add(motifs_text, 0, wxALIGN_CENTER_VERTICAL | wxALL, 3);
    motifSizer->Add(motifs_ctrl, 0, wxALL, 3);
	fileBoxSizer->Add(fileSizer, 0, wxGROW | wxALL, 0);
	fileBoxSizer->Add(motifSizer, 0, wxALL, 0);
    fileBoxSizer->Add(pictures, 0, wxALL, 3);
    
    filterSizer->Add(z_score_text, wxGBPosition(0,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(z_score_ctrl, wxGBPosition(0,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(p_value_text, wxGBPosition(1,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(p_value_ctrl, wxGBPosition(1,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(frequency_text, wxGBPosition(2,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(frequency_ctrl, wxGBPosition(2,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(percentage_text, wxGBPosition(2,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(number_text, wxGBPosition(3,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(number_ctrl, wxGBPosition(3,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(times_text, wxGBPosition(3,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(dangling_text, wxGBPosition(4,0), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(dangling_min_ctrl, wxGBPosition(4,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(dangling_middle_text, wxGBPosition(4,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterSizer->Add(dangling_max_ctrl, wxGBPosition(5,1), wxDefaultSpan, wxALL, 3);	
    filterSizer->Add(dangling_end_text, wxGBPosition(5,2), wxDefaultSpan, wxALIGN_CENTER_VERTICAL | wxALL, 3);	
    filterBoxSizer->Add(filterSizer, 0, wxALL, 0);    
    buttonSizer->AddStretchSpacer(1); //Spacer left of buttons
	buttonSizer->Add(generate_button, 0, wxALL, 3);
	buttonSizer->Add(close_button, 0, wxALL, 3);	
	notFileSizer->Add(filterBoxSizer, wxGBPosition(0,0), wxGBSpan(2,1), wxGROW | wxALL, 3);
	notFileSizer->Add(sort_box, wxGBPosition(0,1), wxDefaultSpan, wxGROW | wxALL, 3);    
    notFileSizer->Add(undef_box, wxGBPosition(1,1), wxDefaultSpan, wxGROW | wxALL, 3);
    for (int i=0; i<number_v_colors; ++i){
        vertexColorSizer->Add(vertexNumbers[i], 0, wxALL | wxALIGN_CENTER, 3);
        vertexColorSizer->Add(vertexColorButtons[i], 0, wxALL, 3);
    }
    for (int i=0; i<number_e_colors; ++i){
        edgeColorSizer->Add(edgeNumbers[i], 0, wxALL | wxALIGN_CENTER, 3);
        edgeColorSizer->Add(edgeColorButtons[i], 0, wxALL, 3);
    }
    if (number_v_colors != 0) vertexColorBoxSizer->Add(vertexColorSizer, 0, wxGROW | wxALL, 0);
    if (number_e_colors != 0) edgeColorBoxSizer->Add(edgeColorSizer, 0, wxGROW | wxALL, 0);
    topSizer->Add(fileBoxSizer, 0, wxGROW | wxALL, 3);
	topSizer->Add(notFileSizer, 0, wxGROW | wxALL, 0);
	if (number_v_colors != 0) topSizer->Add(vertexColorBoxSizer, 0, wxGROW | wxALL, 3);	
	if (number_e_colors != 0) topSizer->Add(edgeColorBoxSizer, 0, wxGROW | wxALL, 3);	
	topSizer->Add(buttonSizer, 0, wxGROW | wxALL, 3);
	
    this->SetSizer(topSizer);
	topSizer->Fit(this);
	topSizer->SetSizeHints(this);
	
    this->SetTitle(wxT("HTML-Export options"));
	this->Center();
	this->SetIcon(wxIcon(icon_xpm));
}

//values that will never filter any motif
static const double z_score_filterDEFAULT = -1.0, 
                    p_value_filterDEFAULT = 2.0,
                    frequency_filterDEFAULT = -1.0;
static const long motifs_per_fileDEFAULT = 40,
                  number_filterDEFAULT = 0, 
                  d_min_filterDEFAULT = 1, 
                  d_max_filterDEFAULT = 0;

void ResultDialog::InitColors()
{
    vertex_pens[0] = wxPen(*wxBLACK);
    vertex_brushes[0] = wxBrush(*wxBLACK);    
    for (int i=1; i<16; ++i){
        switch (i) {
               case 1: vertex_pens[i] = wxPen(*wxGREEN);
                       vertex_brushes[i] = wxBrush(*wxGREEN);
                       break;
               case 2: vertex_pens[i] = wxPen(*wxRED);
                       vertex_brushes[i] = wxBrush(*wxRED);
                       break;
               case 3: vertex_pens[i] = wxPen(*wxBLUE);
                       vertex_brushes[i] = wxBrush(*wxBLUE);
                       break;
               case 4: vertex_pens[i] = wxPen(wxColour("YELLOW"));
                       vertex_brushes[i] = wxBrush(wxColour("YELLOW"));
                       break;
               case 5: vertex_pens[i] = wxPen(wxColour("VIOLET"));
                       vertex_brushes[i] = wxBrush(wxColour("VIOLET"));
                       break;
               case 6: vertex_pens[i] = wxPen(wxColour("ORCHID"));
                       vertex_brushes[i] = wxBrush(wxColour("ORCHID"));
                       break;
               case 7: vertex_pens[i] = wxPen(wxColour("ORANGE"));
                       vertex_brushes[i] = wxBrush(wxColour("ORANGE"));
                       break;
               case 8: vertex_pens[i] = wxPen(wxColour("PALE GREEN"));
                       vertex_brushes[i] = wxBrush(wxColour("PALE GREEN"));
                       break;
               case 9: vertex_pens[i] = wxPen(wxColour("PURPLE"));
                       vertex_brushes[i] = wxBrush(wxColour("PURPLE"));
                       break;
               case 10: vertex_pens[i] = wxPen(wxColour("CYAN"));
                       vertex_brushes[i] = wxBrush(wxColour("CYAN"));
                       break;
               case 11: vertex_pens[i] = wxPen(wxColour("YELLOW GREEN"));
                       vertex_brushes[i] = wxBrush(wxColour("YELLOW GREEN"));
                       break;
               case 12: vertex_pens[i] = wxPen(wxColour("GOLD"));
                       vertex_brushes[i] = wxBrush(wxColour("GOLD"));
                       break;
               case 13: vertex_pens[i] = wxPen(wxColour("BROWN"));
                       vertex_brushes[i] = wxBrush(wxColour("BROWN"));
                       break;
               case 14: vertex_pens[i] = wxPen(wxColour("SALMON"));
                       vertex_brushes[i] = wxBrush(wxColour("SALMON"));
                       break;
               case 15: vertex_pens[i] = wxPen(wxColour("FIREBRICK"));
                       vertex_brushes[i] = wxBrush(wxColour("FIREBRICK"));
                       break;
        }
    }
    edge_pens[0] = wxPen(*wxBLACK);
    edge_brushes[0] = wxBrush(*wxBLACK);    
    for (int i=1; i<7; ++i){
         switch (i) {      
               case 1: edge_pens[i] = wxPen(*wxGREEN);
                       edge_brushes[i] = wxBrush(*wxGREEN);
                       break;
               case 2: edge_pens[i] = wxPen(*wxRED);
                       edge_brushes[i] = wxBrush(*wxRED);
                       break;
               case 3: edge_pens[i] = wxPen(*wxBLUE);
                       edge_brushes[i] = wxBrush(*wxBLUE);
                       break;
               case 4: edge_pens[i] = wxPen(wxColour("YELLOW"));
                       edge_brushes[i] = wxBrush(wxColour("YELLOW"));
                       break;
               case 5: edge_pens[i] = wxPen(wxColour("ORANGE"));
                       edge_brushes[i] = wxBrush(wxColour("ORANGE"));
                       break;
               case 6: edge_pens[i] = wxPen(wxColour("ORCHID"));
                       edge_brushes[i] = wxBrush(wxColour("ORCHID"));
                       break;
         }
    }    
}     

void ResultDialog::generate_buttonClick(wxCommandEvent& event)
{
    wxString messageStr = wxT(""); // String for error-messages
    // init filters with values that will never filter any motif
    double z_score_filter = z_score_filterDEFAULT, 
           p_value_filter = p_value_filterDEFAULT, 
           frequency_filter = frequency_filterDEFAULT;
    long   motifs_per_file = motifs_per_fileDEFAULT, 
           number_filter = number_filterDEFAULT, 
           d_min_filter = d_min_filterDEFAULT, 
           d_max_filter = d_max_filterDEFAULT;    

    htmlfile = wxFileName(file_ctrl->GetValue());
    if (!htmlfile.GetExt().IsSameAs(wxT("html"),false))
        htmlfile.SetExt(wxT("html")); // set proper extension for the html file.
    if (!htmlfile.IsOk())
        messageStr += wxT("HTML-Filename is not valid");

    if (!motifs_ctrl->GetValue().ToLong(&motifs_per_file))
        messageStr+= wxT("Motif-Number should be an integer\n");
    pics = pictures->IsChecked();
    
    if (z_score_text->IsChecked())
       if (!z_score_ctrl->GetValue().ToDouble(&z_score_filter))
           messageStr+= wxT("Z-Score-Filter should be a number\n");
    if (p_value_text->IsChecked())
       if (!p_value_ctrl->GetValue().ToDouble(&p_value_filter))
           messageStr+= wxT("p-Value-Filter should be a number\n");
    if (frequency_text->IsChecked())
       if (!frequency_ctrl->GetValue().ToDouble(&frequency_filter))
           messageStr+= wxT("Frequency-Filter should be a number\n");
    if (number_text->IsChecked())
       if (!number_ctrl->GetValue().ToLong(&number_filter))
           messageStr+= wxT("Number-Filter should be an integer\n");
    if (dangling_text->IsChecked()){
       if (!dangling_min_ctrl->GetValue().ToLong(&d_min_filter) || !dangling_max_ctrl->GetValue().ToLong(&d_max_filter))
           messageStr+= wxT("Dangling-Filter should be an integer\n");
    }
    
    // What shall be done with undef Z-Scores
    ignore_undef = (undef_box->GetSelection() == 2);
    undef_bottom = (undef_box->GetSelection() == 1);

    // An error occured: abort
    if (!messageStr.empty()){
        wxMessageBox(messageStr, wxT("Please correct the following errors:"), wxOK | wxICON_ERROR);
        return;
    }
    
    SortData(sort_constants(sort_box->GetSelection()));    
//wxLogMessage("Start drawing");
    if (pics) // Draw pictures, abort the process if the user clicked "Cancel"
        if(!DrawGraphs(z_score_filter, p_value_filter, frequency_filter,
                         number_filter, d_min_filter, d_max_filter)) return;
//wxLogMessage("Start writing file");
    if (!GenerateHTML(motifs_per_file, z_score_filter, p_value_filter, frequency_filter,
                 number_filter, d_min_filter, d_max_filter)) return;
//wxLogMessage("Done writing file");
    if (!wxLaunchDefaultBrowser(wxT("file://")+htmlfile.GetFullPath()))
        wxMessageBox(wxT("Sorry, could not launch your default browser./nPlease open the HTML-File manually."), wxT("No default browser"), wxOK | wxICON_INFORMATION);
}

void ResultDialog::z_score_textClick(wxCommandEvent& event)
{
    z_score_ctrl->Enable(z_score_text->GetValue());
}

void ResultDialog::p_value_textClick(wxCommandEvent& event)    
{
    p_value_ctrl->Enable(p_value_text->GetValue());
}

void ResultDialog::frequency_textClick(wxCommandEvent& event)    
{
    frequency_ctrl->Enable(frequency_text->GetValue());
}

void ResultDialog::number_textClick(wxCommandEvent& event)  
{
    number_ctrl->Enable(number_text->GetValue());
}

void ResultDialog::dangling_textClick(wxCommandEvent& event)
{
    dangling_min_ctrl->Enable(dangling_text->GetValue());
    dangling_max_ctrl->Enable(dangling_text->GetValue());
}


void ResultDialog::file_buttonClick(wxCommandEvent& event)
{
	wxFileDialog * file_dialog =  new wxFileDialog(this, wxT("Choose an HTML output file"), wxT(""), wxT(""), wxT("*.html"), wxSAVE | wxCHANGE_DIR);
    if (file_dialog->ShowModal() == wxID_OK){
        file_ctrl->SetValue(file_dialog->GetPath());
        file_ctrl->SetInsertionPointEnd();
    }         
}

void ResultDialog::ChooseVertexColor(wxCommandEvent& event)
{
    int number = event.GetId() - ID_VERTEX_COLORS;
    wxColourData* data = new wxColourData();
    data->SetColour(vertex_pens[number].GetColour());
    wxColourDialog* color_dialog = new wxColourDialog(this, data);
    if (color_dialog->ShowModal() == wxID_OK){
        wxColour color = color_dialog->GetColourData().GetColour();
        vertex_pens[number] = wxPen(color);
        vertex_brushes[number] = wxBrush(color);
        wxMemoryDC *dc = new wxMemoryDC(); // dc to redraw the Bitmap of the Button
        dc->SelectObject(vertexColorButtons[number]->GetBitmapLabel());
        dc->SetBrush(vertex_brushes[number]);
        dc->SetPen(vertex_pens[number]);
        dc->DrawRectangle(0,0,20,15);     
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);        
        dc->SelectObject(wxNullBitmap);
        vertexColorButtons[number]->Refresh();
    }
}

void ResultDialog::ChooseEdgeColor(wxCommandEvent& event)
{
    int number = event.GetId() - ID_EDGE_COLORS;
    wxColourData* data = new wxColourData();
    data->SetColour(edge_pens[number].GetColour());
    wxColourDialog* color_dialog = new wxColourDialog(this, data);
    if (color_dialog->ShowModal() == wxID_OK){
        wxColour color = color_dialog->GetColourData().GetColour();
        edge_pens[number] = wxPen(color);
        edge_brushes[number] = wxBrush(color);
        wxMemoryDC *dc = new wxMemoryDC();
        dc->SelectObject(edgeColorButtons[number]->GetBitmapLabel());
        dc->SetBrush(edge_brushes[number]);
        dc->SetPen(edge_pens[number]);
        dc->DrawRectangle(0,0,20,15);     
        dc->SetBrush(wxNullBrush);
        dc->SetPen(wxNullPen);        
        dc->SelectObject(wxNullBitmap);
        edgeColorButtons[number]->Refresh();
    }
}

void ResultDialog::ResultDialogClose(wxCloseEvent& event)
{
    Destroy();
}

bool ResultDialog::ReadResults()
{
    wxProgressDialog *progress_dlg;    
    wxFile file(resultfile.GetFullPath());
	if (!file.IsOpened()) {
        wxMessageBox(wxT("Could not open file for reading"),wxT("Error reading file"), wxOK | wxICON_ERROR);
        return false;
    }
    wxFileInputStream filestream(file);
    wxTextInputStream in(filestream);
    //ifstream in(resultfile.GetFullPath().c_str());
    SetCursor(*wxHOURGLASS_CURSOR); //Set the cursor to wait, as this may take time.

    
	// Initialize variables
    char separator_char = ' '; // The column delimiter char
    int numcols = 0;
    string actual_text = "", line = "";
	istringstream isstream;
	motif actual_motif;
	short degree[8] = { 0 }; //all degrees to 0 at the beginning

	// Skip the header lines, till the network line comes
	line = in.ReadLine().mb_str();
    while ( !file.Eof() && line.find("Network name") == string::npos) {
        line = in.ReadLine().mb_str();
    }
	if (line.length() > 60) { // If the name is very long
        summary1 = "Network name: "; // shorten it to the filename only
        summary1 += wxFileName(wxString(line.substr(14, line.length()).c_str(),wxConvUTF8)).GetFullName().mb_str();
        summary1 += "<br>\n";
    } else {
        summary1 = line + "<br>\n"; //No changes
    }
    
    // Read the next lines
    line = in.ReadLine().mb_str();
	while ( !file.Eof() && line.find("vertex colors") == string::npos &&
            line.find("edge colors") == string::npos && line.find("random") == string::npos) {
        summary1 += line + "<br>\n";
        line = in.ReadLine().mb_str();
    }
    if (line.find("vertex colors") != string::npos){
        isstream.str(line.substr(24));
        isstream.clear();
        isstream >> number_v_colors;
    }
    
	while ( !file.Eof() && line.find("edge colors") == string::npos && line.find("random") == string::npos) {
        summary1 += line + "<br>\n";
        line = in.ReadLine().mb_str();
    }
    if (line.find("edge colors") != string::npos){
        isstream.str(line.substr(22));
        isstream.clear();
        isstream >> number_e_colors;
    }
    
	while ( !file.Eof() && line.find("random") == string::npos) {
        summary1 += line + "<br>\n";
        line = in.ReadLine().mb_str();
    }
    
    summary2 = line + "<br>\n"; // Read line with "Generated" belongs to summary2
	line = in.ReadLine().mb_str();
    while ( !file.Eof() && line.find("subgraphs") == string::npos) {
        summary2 += line + "<br>\n";
        line = in.ReadLine().mb_str();
    }
    summary2 += line + "<br>\n";
    isstream.str(line);
    isstream.clear();
    isstream >> number_of_subgraphs;
    line = in.ReadLine().mb_str();
	while ( !file.Eof() && line != "Result overview:") {
        summary2 += line + "<br>\n";
        line = in.ReadLine().mb_str();
    }
    
    // Read to the table header
	line = in.ReadLine().mb_str();
    while ( !file.Eof() && line.find("ID") == string::npos) {
        line = in.ReadLine().mb_str();
    }

    // if there is no space in the whole header line, csv-format is assumed:
    if (line.find(" ") == string::npos) {
        separator_char = ',';
        if (line.find(",") == string::npos) {
            wxMessageBox(wxT("Could not recognize file type"), wxT("File error"), wxICON_ERROR | wxOK);
            SetCursor(wxNullCursor); //Reset cursor.    
            return false;
        } 
    }
    
    //Read the first header line into the headers array
    isstream.str(line);
    isstream.clear();
    // getline only reads till the next sep_char, not a whole line.
    while (getline(isstream, actual_text, separator_char))
    if (!actual_text.empty()) {
        headers.push_back(actual_text);
        numcols++;
    }
    
    line = in.ReadLine().mb_str();
    if (!!file.Eof()) return false; // Read second line safely (it might not exist)
    isstream.str(line); // get the new value of line to the stream
    isstream.clear(); // Reset the state of the stream

    // Add the Additions to the headers, the first two cols have no addition
    for (int col=2; col<numcols && getline(isstream, actual_text, separator_char);)
    if (!actual_text.empty()) {
        headers[col] += "<br>" + actual_text;
        ++col; // increase loop var only if a value was read. 
    }

    // Show the progress-Dialog
    progress_dlg = new wxProgressDialog(wxT("HTML-Export options"), wxT("Reading Resultfile"), file.Length(), this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
    // Read the table from the resultfile
    line = in.ReadLine().mb_str();
    while (!file.Eof()){
    if (line != "") {
        isstream.str(line); // get the new value of line to the stream
        isstream.clear(); // Reset the state of the stream
        // Getline reads only part of the line here, till the next separator_char is found!
        for (int col=0; col<numcols && getline(isstream, actual_text, separator_char);)
        if (actual_text != "") {
            switch (col) {
            case 0: actual_motif.id = actual_text; break;
            case 1: actual_motif.adj = actual_text; break;
            case 2: actual_motif.freq = atof(actual_text.c_str()); break;
            case 3: actual_motif.rand_mean = atof(actual_text.c_str()); break;
            case 4: actual_motif.rand_sd = (actual_text=="undefined"?Inf:atof(actual_text.c_str())); break;
            case 5: actual_motif.z_score = (actual_text=="undefined"?Inf:atof(actual_text.c_str())); break;
            case 6: actual_motif.p_value = atof(actual_text.c_str()); break;
            }
            // If G_N has not yet been set, and we are in the second col
            if (G_N==0 && col == 1){
                G_N = actual_text.length(); // set G_N, start showing the progressDialog
            }
            ++col; // increase loop var only if a value was read.
        }
        for (int lines_between = 0; lines_between < G_N-1; ++lines_between){
            line = in.ReadLine().mb_str();
            if (!file.Eof()) { // get the new line
                isstream.str(line); // get the new value of line to the stream
                isstream.clear(); // Reset the state of the stream                
                // Jump the empty col before the second line of the adj-matrix
                while(getline(isstream, actual_text, separator_char) && actual_text == "") {}
                    actual_motif.adj += actual_text; // Append the new text to the data-value
            }
        }
        // Determine dangling edges by reading the adj-matrix
        actual_motif.dangling_edges = 0;
        for (int i=0; i<G_N; ++i)
            for (int j=0; j<G_N; ++j)
                if (actual_motif.adj[i*G_N+j] == '1'){
                    ++degree[i];
                    ++degree[j];
                }
        for (int i=0; i<G_N; ++i){
            if (degree[i] == 1) ++actual_motif.dangling_edges;
            degree[i] = 0;
        }                
        data.push_back(actual_motif); // append motif to the vector
            if (!progress_dlg->Update(file.Tell())){ // If user canceled return false
                SetCursor(wxNullCursor); // Done, reset cursor.
                progress_dlg->Update(file.Length()); // To close the dialog
                return false;
            }
        
    }
    line = in.ReadLine().mb_str();
    }
    if (data.size() > 10000) pictures->SetValue(false); // if data is too big, no pictures is the default
    SetCursor(wxNullCursor); //Done, reset cursor.    
    progress_dlg->Update(file.Length()); // To close the dialog    
    return true;    
}

void ResultDialog::SortData(sort_constants sort)
{
    if(sorted_by != sort || sort == z_score){ // Always sort if "Z-Score", cause the handling of "undefined" might have changed.
        sorted_by = sort;
        std::sort(data.begin(), data.end(), compare); 
    }
}

bool ResultDialog::GenerateHTML(const long & motifs_per_file,
                    const double & z_score_filter, const double & p_value_filter,
                    const double &  frequency_filter, const long & number_filter,
                    const long & d_min_filter, const long & d_max_filter)
{
    SetCursor(*wxHOURGLASS_CURSOR); // Set Cursor to wait.
    long motifs_written = 0, motifs_done = 0; // written is the number of motif in the current file, done counts the overall motifs!
    int file_number = 0;
    wxProgressDialog *progress_dlg = new wxProgressDialog(wxT("HTML-Files"), wxT("Generating HTML-Files"), data.size(), this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
    wxFileName actual_file = htmlfile;
    actual_file.SetName(htmlfile.GetName()<<file_number);
    ofstream out(actual_file.GetFullPath().mb_str()); // define out-Stream for actual_file

    // Write headers in the actual file.    
    StartHtmlFile(out, file_number);
    // Write data into the html-file
    for (vector<motif>::const_iterator iter = data.begin(); iter != data.end(); ++iter){
    // Filter it according to all the filters and (if ignore_undef is set) throw out the motifs with undef Z-Scores
    if (fabs(iter->z_score)>z_score_filter && !(ignore_undef && std::isinf(iter->z_score))
        && iter->p_value < p_value_filter && iter->freq > frequency_filter
        && (iter->freq * number_of_subgraphs) / 100 >= number_filter
        && (iter->dangling_edges > d_max_filter || iter->dangling_edges < d_min_filter))
    {
        if (motifs_written%motifs_per_file == 0 && motifs_written != 0){ // if the file has been fully written.
            FinishHtmlFile(out, file_number, false); //Finish it, false, cause this is not the last file
            out.close();
            ++file_number;                           // Inc file_number
            actual_file.SetName(htmlfile.GetName()<<file_number); //and start a new one
            out.open(actual_file.GetFullPath().mb_str()); // open new actual_file
            StartHtmlFile(out, file_number);
        }
        out << " <tr>\n";
        for (unsigned int i=0; i<headers.size(); ++i){ 
            out << "  <td>";
            switch (i) {
            case 0: out << iter->id; break;
            case 1: if (pics) { 
                        out << "<img src=\"" << htmlfile.GetName() << "-images/" << iter->adj << ".png\""
                            << " alt=\""; 
                        for (int i=0; i<G_N; ++i)
                            out << iter->adj.substr(i*G_N,G_N) << '\n';
                        out << "\">";
                    } else {
                        for (int i=0; i<G_N; ++i)
                            out << iter->adj.substr(i*G_N,G_N) << "<br>";
                    }
                    break;
            case 2: out << iter->freq << '%'; break;
            case 3: out << iter->rand_mean << '%'; break;
            case 4: if (std::isinf(iter->rand_sd) || std::isnan(iter->rand_sd))
                        out << "undefined";
                    else out << iter->rand_sd;
                    break;
            case 5: if (std::isinf(iter->z_score) || std::isnan(iter->z_score))
                        out << "undefined";
                    else out << iter->z_score;
                    break;
            case 6: out << iter->p_value; break;
            }
            out << "</td>\n";
        }
        out << " </tr>\n";
        ++motifs_written;
        }
        ++motifs_done;
        if (motifs_done%10 == 0)  //Update every 10 motifs
            if (!progress_dlg->Update(motifs_done)){ // If user canceled return false
                SetCursor(wxNullCursor); // Done, reset cursor.
                progress_dlg->Update(data.size()); // To close the dialog
                return false;
            }
        
    }
    FinishHtmlFile(out, file_number, true); //this is the last file!
    out.close();
    
    // Exchange the last Next-link with the Text "Next" without link
    wxTextFile textfile(actual_file.GetFullPath());
    textfile.Open();
    // Seek to the line where "Next" is found.
    for (wxString str = textfile.GetFirstLine(); str.Find(wxT("Next")) == -1; str = textfile.GetNextLine() ){}
    textfile.RemoveLine(textfile.GetCurrentLine());
    textfile.InsertLine(wxT(" Next"), textfile.GetCurrentLine());
    textfile.Write();
    textfile.Close();
    
    WriteIndexFile(file_number+1, z_score_filter, p_value_filter,
                   frequency_filter, motifs_per_file, number_filter, 
                   d_min_filter, d_max_filter ); //We have file_number+1 files written
    progress_dlg->Update(data.size()); // To close the dialog
    SetCursor(wxNullCursor); // Done, reset cursor.
    return true;
}

void ResultDialog::StartHtmlFile(ofstream & out, int file_number)
{
    wxFileName prev_file = htmlfile;
    prev_file.SetName(htmlfile.GetName()<<(file_number-1));    
    wxFileName next_file = htmlfile;
    next_file.SetName(htmlfile.GetName()<<(file_number+1));    
    
    // HTML-header
    
    string::size_type HTML_index = 0, HTML_indexEnd = 0;
    HTML_index = summary1.find("name: ")+6;
    HTML_indexEnd = summary1.find("<br>",HTML_index);
    const string HTML_NetName = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("graph size: ",HTML_indexEnd)+12;
    const string HTML_SubSize = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    
    out << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
    out << "<html lang=en>\n";
    out << "<head>\n";
    out << "<title>Size-"<< HTML_SubSize <<" Network Motifs in Network \'"
        << HTML_NetName<<"\' - Page "<< (file_number+1)
        << " - Generated With FANMOD</title>\n";
    out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n";
    out << "<meta name=\"generator\" content=\"FANMOD by Sebastian Wernicke and Florian Rasche\">\n";
    out << "<style type=\"text/css\">\n";
    out << "<!--\n";
    out << "p {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << " text-align: justify;\n";
    out << "}\n";
    out << "th {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << "}\n";
    out << "td {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << " text-align: center;\n";
    out << "}\n";
    out << "h1 {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 18pt;\n";
    out << " text-align: left;\n";
    out << "}\n";
    out << "h3 {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 12pt;\n";
    out << " text-align: left;\n";
    out << "}\n";
    out << "-->\n";
    out << "</style>\n";
    out << "</head>\n";
    out << "<body text=\"#000000\" bgcolor=\"#FFFFFF\" link=\"#FF0000\" alink=\"#FF0000\" vlink=\"#FF0000\">\n";
    out << "<div align=\"center\"><table border=0><tr><td>\n";
 
    out << "<div align=\"right\">\n";

    // Write first navigation
    if (file_number > 0)
        out << " <a href=\"" << prev_file.GetFullName().c_str() << "\" ><font size=\"-1\">Previous</font></a>\n";
    else
        out << "<font size=\"-1\">Previous</font>\n";
    out << " <a href=\"" << htmlfile.GetFullName().c_str() << "\" ><font size=\"-1\">Index</font></a>\n";
    out << " <a href=\"" << next_file.GetFullName().c_str() << "\" ><font size=\"-1\">Next</font></a>\n";

    out << "</div>\n";
    out << "<hr noshade size=\"1\">\n";
    out << "<table width=\"100%\">\n";
    out << "<td align=\"left\"><font size=\"+1\"><div align=\"left\">Size-"
        << HTML_SubSize<<" Network Motifs in '"<< HTML_NetName<<"'</div></font></td>\n";
    out << "<td align=\"right\"><font size=\"+1\"><div align=\"right\">Page "<< (file_number+1) <<"</div></font></td>\n";
    out << "</tr>\n";
    out << "</table><br>\n";
    out << "<font size=\"-2\">\n";
    out << "Generated with <a href=\"http://www.minet.uni-jena.de/~wernicke/motifs/index.html\">\n";
    out << "FANMOD - FAst Network MOtif Detection</a>. ";
    out << "Written by Sebastian Wernicke and Florian Rasche\n";
    out << "</font>\n";
    out << "<hr noshade size=\"1\">\n";
    out << "<br>\n";
    
    // Write table headers into the html-file
    out << "<table border=1>\n <tr>";
    
    for (vector<string>::const_iterator iter = headers.begin(); iter != headers.end(); ++iter)
    {
        out << "  <th> " << *iter << " </th>\n";
    }
    out << " </tr>\n";
}

void ResultDialog::FinishHtmlFile(ofstream & out, int file_number, bool is_last)
{
    wxFileName prev_file = htmlfile;
    prev_file.SetName(htmlfile.GetName()<<(file_number-1));    
    wxFileName next_file = htmlfile;
    next_file.SetName(htmlfile.GetName()<<(file_number+1));    
    // Finish the table, write the navigation.
    out << "</table>\n<br>\n";
    if (file_number > 0)
        out << " <a href=\"" << prev_file.GetFullName().c_str() << "\" >Previous</a>\n";
    else
        out << "Previous\n";
    out << " <a href=\"" << htmlfile.GetFullName().c_str() << "\" >Index</a>\n";
    if (!is_last)
        out << " <a href=\"" << next_file.GetFullName().c_str() << "\" >Next</a>\n";
    else
       out << "Next\n";
    out << "<br>\n<hr noshade size=\"1\">\n";
    out << "<font size=\"-2\">\n";
    out << "Generated with <a href=\"http://www.minet.uni-jena.de/~wernicke/motifs/index.html\">\n";
    out << "FANMOD - FAst Network MOtif Detection</a>.\n";
    out << "Written by Sebastian Wernicke and Florian Rasche\n";
    out << "</font>\n";
    out << "<hr noshade size=\"1\">\n";
    out << "<br>\n";
    out << "</td></tr></table></div>\n</body>\n</html>"; //end the file  
}

void ResultDialog::WriteIndexFile(int data_files, double z_score_filter, 
            double p_value_filter, double frequency_filter, long motifs_per_file, 
            long number_filter, long d_min_filter, long d_max_filter)
{
    const int cols = 20; //number of columns for index
    ofstream out(htmlfile.GetFullPath().mb_str()); //Write into htmlfile
    stringstream ss1, ss2, ss3, ss4;
    
    //Parse Summary1 Data
    bool HTML_SamplingParams = false;
    if (summary1.find("Sampling parameters = {") != string::npos)
       HTML_SamplingParams = true;

    string::size_type HTML_index = 0, HTML_indexEnd = 0;
    HTML_index = summary1.find("name: ")+6;
    HTML_indexEnd = summary1.find("<br>",HTML_index);
    const string HTML_NetName = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("type: ",HTML_indexEnd)+6;
    const string HTML_NetType = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("of nodes: ",HTML_indexEnd)+10;
    const string HTML_NumNodes = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("of edges: ",HTML_indexEnd)+10;
    const string HTML_NumEdges = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("single edges: ",HTML_indexEnd)+14;
    const string HTML_SingleEdges = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("mutual edges: ",HTML_indexEnd)+14;
    const string HTML_MutualEdges = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);

    HTML_index = summary1.find("Number of vertex colors: ",HTML_indexEnd)+25;    
    const string HTML_vcolors = (HTML_index == string::npos) ? "" : summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("Number of edge colors: ",HTML_indexEnd)+23;
    const string HTML_ecolors = (HTML_index == string::npos) ? "" : summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    
    HTML_index = summary1.find("rithm: ",HTML_indexEnd)+7;
    const string HTML_Algorithm = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary1.find("parameters = {",HTML_indexEnd)+14;
    const string HTML_Parameters = summary1.substr(HTML_index,summary1.find("}",HTML_index)-HTML_index);
    HTML_index = summary1.find("graph size: ",HTML_indexEnd)+12;
    const string HTML_SubSize = summary1.substr(HTML_index,summary1.find("<br>",HTML_index)-HTML_index);

    //Parse Summary2 Data
    
    bool HTML_RandomGraphs = true;
    if (summary2.find("No random graphs were generated") != string::npos)
       HTML_RandomGraphs = false;
    const bool HTML_ecolors_matter = (summary2.find("exchanging only edges with the same color")!=string::npos);
    const bool HTML_vcolors_matter = (summary2.find("only edges with the same vertex colors")!=string::npos);
    HTML_index = summary2.find("Generated ")+10;
    const string HTML_NumRand = summary2.substr(HTML_index,summary2.find(" random networks",HTML_index)-HTML_index);
    HTML_index = summary2.find("random networks<br>")+19;
    const string HTML_RandType = summary2.substr(HTML_index,summary2.find("<br>",HTML_index)-HTML_index);
    HTML_index = summary2.find("<br>",HTML_index)+4;
    if (HTML_ecolors_matter || HTML_vcolors_matter)
       HTML_index = summary2.find("<br>",HTML_index)+4;
    const string HTML_RandExchanges = summary2.substr(HTML_index,summary2.find("exchanges per",HTML_index)-HTML_index);
    HTML_index = summary2.find("per edge and")+12;
    const string HTML_RandTries = summary2.substr(HTML_index,summary2.find("tries per",HTML_index)-HTML_index);
    if (HTML_RandomGraphs)
        HTML_index = summary2.find("tries per edge")+14+10; // 10 = Two <br> and new lines        
    else
        HTML_index = summary2.find("were generated")+14+10;    
    const string HTML_NumSubOri = summary2.substr(HTML_index,summary2.find(" subgraphs",HTML_index)-HTML_index);
    HTML_index = summary2.find("original network.<br>")+21;
    const string HTML_NumSubRan = summary2.substr(HTML_index,summary2.find(" subgraphs",HTML_index)-HTML_index);
    if (HTML_RandomGraphs)
        HTML_index = summary2.find("random networks.<br>")+20;
    else
       HTML_index = summary2.find("No random graphs were generated")+31+6;    
    const string HTML_NumSubTtl = summary2.substr(HTML_index,summary2.find(" subgraphs",HTML_index)-HTML_index);
    HTML_index = summary2.find("random networks: ")+17;
    const string HTML_NumTries = summary2.substr(HTML_index,summary2.find(" tries were made",HTML_index)-HTML_index);
    ss3.str(HTML_NumTries);
    double tries = 0;
    ss3 >> tries;
    HTML_index = summary2.find(" tries were made, ")+18;
    const string HTML_NumSucc = summary2.substr(HTML_index,summary2.find(" were successful.",HTML_index)-HTML_index);
    ss4.str(HTML_NumSucc);
    double success = 0;
    ss4 >> success;
    HTML_index = summary2.find("Randomization took ")+19;
    const string HTML_RandTime = summary2.substr(HTML_index,summary2.find(" seconds",HTML_index)-HTML_index);
    ss1.str(HTML_RandTime);
    double t1 = 0;
    ss1 >> t1;
    HTML_index = summary2.find("took ",HTML_index)+5;
    const string HTML_AlgoTime = summary2.substr(HTML_index,summary2.find(" seconds",HTML_index)-HTML_index);
    ss2.str(HTML_AlgoTime);
    double t2 = 0;
    ss2 >> t2;
    const double HTML_TotalTime = t1+t2;

    // HTML-header
    out << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
    out << "<html lang=en>\n";
    out << "<head>\n";
    out << "<title>Size-"<<HTML_SubSize<<" Network Motifs in Network \'"<<HTML_NetName<<"\' - Generated With FANMOD</title>\n";
    out << "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=ISO-8859-1\">\n";
    out << "<meta name=\"generator\" content=\"FANMOD by Sebastian Wernicke and Florian Rasche\">\n";
    out << "<style type=\"text/css\">\n";
    out << "<!--\n";
    out << "p {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << " text-align: justify;\n";
    out << "}\n";
    out << "th {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << "}\n";
    out << "td {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 10pt;\n";
    out << " text-align: center;\n";
    out << "}\n";
    out << "h1 {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 18pt;\n";
    out << " text-align: left;\n";
    out << "}\n";
    out << "h3 {\n";
    out << " font-family: sans-serif;\n";
    out << " font-size: 12pt;\n";
    out << " text-align: left;\n";
    out << "}\n";
    out << "-->\n";
    out << "</style>\n";
    out << "</head>\n";
    out << "<body text=\"#000000\" bgcolor=\"#FFFFFF\" link=\"#FF0000\" alink=\"#FF0000\" vlink=\"#FF0000\">\n";
    out << "<div align=\"center\"><table border=0><tr><td>\n";

//    out << "----" << summary1 << "----";
//    out << "----" << summary2 << "----";
    
    out << "<hr noshade size=\"1\">\n";
    out << "<h1>Size-" << HTML_SubSize << " Network Motifs in \'" << HTML_NetName << "\'</h1>\n";
    out << "<p>\n";
    out << "<font size=\"-2\">\n";
    out << "Generated with <a href=\"http://www.minet.uni-jena.de/~wernicke/motifs/index.html\">\n";
    out << "FANMOD - FAst Network MOtif Detection</a>.\n";
    out << "Written by Sebastian Wernicke and Florian Rasche\n";
    out << "</font>\n";
    out << "</p>\n";
    out << "<hr noshade size=\"1\">\n";
    out << "<h3>Network</h3>\n";
    out << "<p>\n";
    out << "<b>Filename:</b> \'" << HTML_NetName << "\' (" << HTML_NetType <<")<br/>\n";
    out << "<b>Number of nodes:</b> " << HTML_NumNodes << "<br/>\n";
    out << "<b>Number of edges:</b> " << HTML_NumEdges << " ("<< HTML_SingleEdges <<" single, "<< HTML_MutualEdges << " bidirectional)<br/>\n";
    if (number_v_colors != 0) 
       out << "<b>Number of vertex colors:</b> " << number_v_colors << "<br/>\n";
    if (number_e_colors != 0) 
       out << "<b>Number of edge colors:</b> " << number_e_colors << "<br/>\n";
    out << "</p>\n";
    out << "<h3>Algorithm</h3>\n";
    out << "<p>\n";
    out << "<b>Size of subgraphs:</b> " << HTML_SubSize << "<br/>\n";
    out << "<b>Algorithm:</b> "<< HTML_Algorithm << "<br/>\n";
    if (HTML_SamplingParams)
       out << "<b>Sampling parameters:</b> " << HTML_Parameters << "<br>\n"; 
    out << "</p>\n";
    out << "<h3>Random Networks</h3>\n";
    out << "<p>\n";
    
    if (HTML_RandomGraphs) {
        out << "<b>Number of random networks:</b> " << HTML_NumRand << "<br/>\n";
        out << "<b>Random model:</b> " << HTML_RandType  
            << (HTML_ecolors_matter && (!HTML_vcolors_matter) ? " (regarding edge colors)" : "")
            << ((!HTML_ecolors_matter) && HTML_vcolors_matter ? " (regarding vertex colors)" : "")
            << (HTML_ecolors_matter && HTML_vcolors_matter    ? " (regarding edge and vertex colors)" : "")
            << "<br/>\n";
        out << "<b>Edge exchange parameters:</b> " << HTML_RandExchanges  
            << " exchanges per edge, " << HTML_RandTries 
            << " tries per exchange<br/>\n";
    } else {
        out << "<b>No random networks were generated.</b><br/>";               
    }

    out << "</p>\n";
    out << "<h3>Algorithm Statistics</h3>\n";
    out << "<p>\n";
    out << "<b>Running time:</b> "<<HTML_TotalTime<<" s ";
    if (HTML_RandomGraphs) {
        out <<" ("<<HTML_RandTime
            <<" s for randomization, "<<HTML_AlgoTime
            <<" s for subgraph detection)<br/>\n";
    } else {
        out <<"<br/>\n";
    }
    out << "<b>Number of detected subgraphs:</b> " << HTML_NumSubTtl;
    if (HTML_RandomGraphs) {
        out <<" in total ("
            <<HTML_NumSubOri<<" in original network, "
            <<HTML_NumSubRan<<" in random networks)<br/>\n";
        out << "<b>Edge exchanges:</b> "<<HTML_NumTries<<" attempts of which "
            <<HTML_NumSucc<<" (="<< int(success/tries*100)
            <<"%) were successful<br/><br/>\n";
    }
    out << "</p>\n";
    out << "<hr noshade size=\"1\">\n";
    out << "<h3>Output Parameters</h3>\n";
    out << "<p>\n";
    out << "<b>Subgraphs per file:</b> "<<motifs_per_file<<"<br>\n";
    
    out << "<b>Subgraph sorting:</b> ";
    switch(sort_box->GetSelection()) {
        case 0 : out << "By |Z-Score|"; break;
        case 1 : out << "By P-Value"; break;
        case 2 : out << "By ocurrence in original network"; break;
    };
    out <<"<br/>\n";
    
    out << "<b>Handling of subgraphs not detected in random networks:</b> ";
    switch(undef_box->GetSelection()) {
        case 0 : out << "Put at top of list"; break;
        case 1 : out << "Put at end of list"; break;
        case 2 : out << "Ignore"; break;
    };    
    out << "<br/>\n";
    
    if (z_score_filter != z_score_filterDEFAULT ||
        p_value_filter != p_value_filterDEFAULT ||
        frequency_filter != frequency_filterDEFAULT ||
        number_filter != number_filterDEFAULT ||
        (d_min_filter != d_min_filterDEFAULT) || (d_max_filter != d_max_filterDEFAULT)) {
        out << "<b>Output Filters:</b><br/>";
        if (z_score_filter != z_score_filterDEFAULT)
           out << "Ignored subgraphs for which |Z-Score| &lt;= " << z_score_filter << "<br>\n";
        if (p_value_filter != p_value_filterDEFAULT)
           out << "Ignored subgraphs for which the P-Value &gt;= " << p_value_filter << "<br>\n"; 
        if (frequency_filter != frequency_filterDEFAULT)
           out << "Ignored subgraphs with a relative frequency of less than " << frequency_filter << "%<br>\n"; 
        if (number_filter != number_filterDEFAULT)
           out << "Ignored subgraphs that were found less than " << number_filter << " times<br>\n"; 
        if ((d_min_filter != d_min_filterDEFAULT) || (d_max_filter != d_max_filterDEFAULT))
           out << "Ignored subgraphs that have between " << d_min_filter << " and "
               << d_max_filter << " dangling edges (=degree-1 vertices)<br/>\n";
    } else {
      out << "<b>Output filters:</b> None<br/>";
    }
    out << "</p>\n";
    // ouput the colors used by the pictures
    if (number_v_colors > 0 || number_e_colors > 0){
        if (number_v_colors > 0) {
            out << "<p><b>Vertex Colors:</b></p>\n";
            for (int i=0; i<number_v_colors; ++i){
                out << "<table border=0 align=\"left\">\n <tr>\n";
                out << "<td>" << (i) << ")</td>\n";
                out << "<td width=\"20\" bgcolor=\"" 
                    << HTML_Color(vertex_pens[i].GetColour()) 
                    << "\">&nbsp;</td>\n";                                
                out << "<td width=\"10\">&nbsp;</td></tr></table><br>\n";
            }
            if (!number_e_colors > 0)
               out << "<p>&nbsp;</p>\n";
        }
        if (number_e_colors > 0) {
            out << "<p><b>Edge Colors:</b></p>\n";
            for (int i=0; i<number_e_colors; ++i){
                out << "<table border=0 align=\"left\">\n <tr>\n";
                out << "<td>" << (i+1) << ")</td>\n";
                out << "<td width=\"20\" bgcolor=\"" 
                    << HTML_Color(edge_pens[i].GetColour()) 
                    << "\">&nbsp;</td>\n";                                
                out << "<td width=\"10\">&nbsp;</td></tr></table>\n";
            }
        }
        out << "<p>&nbsp;</p>\n";
    }
    
    out << "<hr noshade size=\"1\">\n";
    out << "<h3>Result Pages</h3>\n";
    // give the list of indices as table
    out << "<table cellpadding=\"5\" align=\"center\">\n";
    for (int i=0; i<(data_files/cols)+1; ++i){
        out << " <tr>\n";
        for (int j=0; j<cols && i*cols+j < data_files; ++j){
            out << "   <td> <a href=\"" << htmlfile.GetName().c_str() << (i*cols+j)
                << "." << htmlfile.GetExt().c_str() << "\">" << (i*cols+j)+1 << "</a></td>\n";
        }
        out << " </tr>\n";
    }
    out << "</table><br>\n<p>&nbsp;</p>\n";
    out << "<hr noshade size=\"1\">\n";   
    out << "<p>\n";
    out << "<font size=\"-2\">\n";
    out << "Generated with <a href=\"http://www.minet.uni-jena.de/~wernicke/motifs/index.html\">\n";
    out << "FANMOD - FAst Network MOtif Detection</a>.\n";
    out << "Written by Sebastian Wernicke and Florian Rasche\n";
    out << "</font>\n";
    out << "</p>\n";
    
    //end the file 
    out << "</td></tr></table></div>\n</body>\n</html>";  
    out.close();
}

wxString ResultDialog::HTML_Color(const wxColour & color)
{
    unsigned long number = color.Red() << 16 | color.Green() << 8 | color.Blue();
    wxString str;
    str.Printf(wxT("%lX"), number); 
    while (str.length() < 6)
        str.Prepend(wxT("0"));
    str.Prepend(wxT("#"));
    return str;
}
         

bool ResultDialog::DrawGraphs(const double & z_score_filter, const double & p_value_filter,
                    const double &  frequency_filter, const long & number_filter,
                    const long & d_min_filter, const long & d_max_filter)
{
    SetCursor(*wxHOURGLASS_CURSOR); //Set the cursor to wait, as this may take time.
    wxProgressDialog *progress_dlg = new wxProgressDialog(wxT("Pictures"), wxT("Creating pictures"), data.size(), this, wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT);
    long pict_done = 0; // Count of the drawn pictures.
    int *x = new int[G_N], *y = new int[G_N];
    // initalize the array according to G_N
    switch (G_N) {
        case 3: x[0] = 50;
                x[1] = 15;
                x[2] = 85;
                y[0] = 15;
                y[1] = 85;
                y[2] = 85;
                break;
        case 4: x[0] = 15;
                x[1] = 85;
                x[2] = 15;
                x[3] = 85;
                y[0] = 15;
                y[1] = 15;
                y[2] = 85;
                y[3] = 85;
                break;
        case 5: x[0] = 30;
                x[1] = 70;
                x[4] = 50;
                x[3] = 15;
                x[2] = 85;
                y[0] = 15;
                y[1] = 15;
                y[4] = 65;
                y[3] = 85;
                y[2] = 85;
                break;
        case 6: x[0] = 15;
                x[1] = 85;
                x[5] = 50;
                x[3] = 15;
                x[4] = 85;
                x[2] = 50;
                y[0] = 15;
                y[1] = 15;
                y[5] = 30;
                y[3] = 70;
                y[4] = 70;
                y[2] = 85;
                break;
        case 7: x[0] = 10;
                x[1] = 90;
                x[2] = 50;
                x[3] = 30;
                x[6] = 70;
                x[5] = 10;
                x[4] = 90;
                y[0] = 10;
                y[1] = 10;
                y[2] = 30;
                y[3] = 70;
                y[6] = 70;
                y[5] = 90;
                y[4] = 90;
                break;
        case 8: x[0] = 10;
                x[1] = 90;
                x[2] = 50;
                x[3] = 30;
                x[7] = 70;
                x[5] = 50;
                x[6] = 10;
                x[4] = 90;
                y[0] = 10;
                y[1] = 10;
                y[2] = 30;
                y[3] = 50;
                y[7] = 50;
                y[5] = 70;
                y[6] = 90;
                y[4] = 90;
                break;
    }
    
    // Create the dir, where the images are going to be saved
    const wxString imagedir = htmlfile.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) 
                              + htmlfile.GetName() + wxT("-images") + wxFileName::GetPathSeparator();
    if (!wxFileName::DirExists(imagedir)) wxFileName::Mkdir(imagedir);
    
    // Init the drawing system
    wxImage::AddHandler(new wxPNGHandler());
    wxBitmap *bitmap = new wxBitmap(100, 100);
    wxMemoryDC *dc = new wxMemoryDC();
    dc->SelectObject(*bitmap);
    
    // Generate all images
    for (vector<motif>::const_iterator iter = data.begin(); iter != data.end(); ++iter){
    // Filter it according to all the filters and (if ignore_undef is set) throw out the motifs with undef Z-Scores
    if (fabs(iter->z_score)>z_score_filter && !(ignore_undef && std::isinf(iter->z_score))
        && iter->p_value < p_value_filter && iter->freq > frequency_filter
        && (iter->freq * number_of_subgraphs) / 100 >= number_filter
        && (iter->dangling_edges > d_max_filter || iter->dangling_edges < d_min_filter))
        { 
            DrawGraph(dc, iter->adj, x, y); // Pass the adj-matrix to drawgraph
            bitmap->SaveFile(imagedir + wxString(iter->adj.c_str(),wxConvUTF8) + wxT(".png"), wxBITMAP_TYPE_PNG);
        }
        ++pict_done; // If the picture was sorted out, its done as well.
        if (pict_done%10 == 0)  //Update every 10 pictures
            if (!progress_dlg->Update(pict_done)){ // If user canceled return false
                // Reset things for smooth destruction
                dc->SetPen(wxNullPen);
                dc->SetBrush(wxNullBrush);
                dc->SelectObject(wxNullBitmap);
                SetCursor(wxNullCursor); // Done, reset cursor.
                progress_dlg->Update(data.size()); // To close the dialog
                return false;
            }
    }
    // Reset things for smooth destruction
    dc->SetPen(wxNullPen);
    dc->SetBrush(wxNullBrush);
    dc->SelectObject(wxNullBitmap);
    SetCursor(wxNullCursor); // Done, reset cursor.
    progress_dlg->Update(data.size()); // Send the full value to destroy the progress dialog
    return true;
}

void ResultDialog::DrawGraph(wxDC *dc, string adj, int *x_coords, int *y_coords)
{
    dc->BeginDrawing();
    dc->Clear();
    int *edge_drawn = new int[64]; // we need only one half of this matrix, but its easier this way.
    for (int i=0; i<64; ++i) edge_drawn[i] = 0;
    int ed_index = 0;
    bool shift = false; //should the actual edge be shifted?
    // find out whether there is an edge between i and j
    for (int i = 0; i != G_N; ++i) {
		for (int j = 0; j != G_N; ++j) {
    	    if (i == j) // a node will be drawn
            switch (adj[G_N*i+j]){ // a line in adj has length G_N
            case '0': DrawNode(dc, x_coords[i], y_coords[i], 0); break;
            case '1': DrawNode(dc, x_coords[i], y_coords[i], 1); break;
            case '2': DrawNode(dc, x_coords[i], y_coords[i], 2); break;
            case '3': DrawNode(dc, x_coords[i], y_coords[i], 3); break;
            case '4': DrawNode(dc, x_coords[i], y_coords[i], 4); break;
            case '5': DrawNode(dc, x_coords[i], y_coords[i], 5); break;
            case '6': DrawNode(dc, x_coords[i], y_coords[i], 6); break;
            case '7': DrawNode(dc, x_coords[i], y_coords[i], 7); break;
            case '8': DrawNode(dc, x_coords[i], y_coords[i], 8); break;
            case '9': DrawNode(dc, x_coords[i], y_coords[i], 9); break;
            case 'A': DrawNode(dc, x_coords[i], y_coords[i], 10); break;
            case 'B': DrawNode(dc, x_coords[i], y_coords[i], 11); break;
            case 'C': DrawNode(dc, x_coords[i], y_coords[i], 12); break;
            case 'D': DrawNode(dc, x_coords[i], y_coords[i], 13); break;
            case 'E': DrawNode(dc, x_coords[i], y_coords[i], 14); break;
            case 'F': DrawNode(dc, x_coords[i], y_coords[i], 15); break;
            default: wxLogMessage(wxT("Strange sign in adjacency matrix")); break;
            }
            else { // an edge will be drawn
            ed_index = (i<j?i*G_N+j:j*G_N+i); // index for the "edge drawn"-matrix
            switch (adj[G_N*i+j]){ // a line in adj has length G_N
            case '0': break; // No edge
            case '1': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=1);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      0, shift);
                      edge_drawn[ed_index]= 1; // Edge with respctive color is drawn;
                      break; 
            case '2': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=2);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      1, shift);
                      edge_drawn[ed_index]= 2; // Edge with respctive color is drawn;
                      break; 
            case '3': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=3);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      2, shift);
                      edge_drawn[ed_index]= 3; // Edge with respctive color is drawn;
                      break; 
            case '4': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=4);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      3, shift);
                      edge_drawn[ed_index]= 4; // Edge with respctive color is drawn;
                      break; 
            case '5': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=5);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      4, shift);
                      edge_drawn[ed_index]= 5; // Edge with respctive color is drawn;
                      break; 
            case '6': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=6);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      5, shift);
                      edge_drawn[ed_index]= 6; // Edge with respctive color is drawn;
                      break; 
            case '7': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=7);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      6, shift);
                      edge_drawn[ed_index]= 7; // Edge with respctive color is drawn;
                      break; 
            case '8': shift = (edge_drawn[ed_index]!=0) && (edge_drawn[ed_index]!=8);
                      DrawEdge(dc, x_coords[i], y_coords[i], x_coords[j], y_coords[j],
                      7, shift);
                      edge_drawn[ed_index]= 8; // Edge with respctive color is drawn;
                      break; 
            default: wxLogMessage(wxT("Strange sign in adjacency matrix")); break;
            }
            }
		}
    }
    dc->EndDrawing();
}

void ResultDialog::DrawNode(wxDC *dc, int x, int y, const int & color)
{
    dc->SetPen(vertex_pens[color]);
    dc->SetBrush(vertex_brushes[color]);
    dc->DrawCircle(x, y, radius);    
}

void ResultDialog::DrawEdge(wxDC *dc, int x1, int y1, int x2, int y2, 
                            const int & color, bool shift)
{
    dc->SetPen(edge_pens[color]);
    dc->SetBrush(edge_brushes[color]);
    if (shift){
        int dist = 5;
        double mu = atan2(double(y2-y1),double(x2-x1)); // calc angle of line x1,y1->x2, y2
        double beta = mu - 1.5707; // mu - pi/2
        int x_shift = int(dist * cos(beta));
        int y_shift = int(dist * sin(beta));
        x1 += x_shift;
        y1 += y_shift;
        x2 += x_shift;
        y2 += y_shift;
    }
    DrawArrow(dc, x1, y1, x2, y2);
}

void ResultDialog::DrawArrow(wxDC *dc, int x2, int y2, int x1, int y1) // Tip is drawn at x1,y1
{
    int l=12; // length of tip
    double alpha = 0.2618; // = 15° angle of the tip 0.349 = 20° 0.5236 = 30°
    double mu = atan2(double(y2-y1),double(x2-x1)); // calc angle of line x1,y1->x2, y2
    // Move x1,y1 on the circle around the original x1, y1
    x1 += int(radius * cos(mu));
    y1 += int(radius * sin(mu));
    x2 -= int(radius * cos(mu));
    y2 -= int(radius * sin(mu));
    
    // Offsets for the endpionts of the tip
    int x_off1 = int(l * cos(mu-alpha));
    int y_off1 = int(l * sin(mu-alpha));
    int x_off2 = int(l * cos(mu+alpha));
    int y_off2 = int(l * sin(mu+alpha));
    // Draw the basic line
    dc->DrawLine(x1, y1, x2, y2);
    // Draw the tip
    wxPoint list[3];
    list[0] = wxPoint(x1,y1);
    list[1] = wxPoint(x1+x_off1, y1+y_off1);
    list[2] = wxPoint(x1+x_off2, y1+y_off2);
    dc->DrawPolygon(3, list); // draw a triangle
}

bool compare(const motif & a, const motif & b)
{
    switch (sorted_by) {
    case freq: return a.freq > b.freq;
    case rand_mean: return a.rand_mean > b.rand_mean;
    case rand_sd: return a.rand_sd > b.rand_sd;
    case z_score: if (undef_bottom){
                      if (std::isinf(a.z_score)) return false; // Ininity is the smallest of all numbers... *lol*
                      if (std::isinf(b.z_score)) return true;
                  }
                  return fabs(a.z_score) > fabs(b.z_score);
    case p_value: return a.p_value < b.p_value;
    }
    return a.freq > b.freq; // just to return sth behind the switch
}

// Reads an output file and returns the summary
wxString GetSummaryFromFile(const wxString & filename)
{
    ifstream in(filename.mb_str());
    
    if (!in.good())
        return wxT("File ")+filename+wxT(" does not exist");
    string ret = "";
    string line = "";
    for (int i=0; getline(in, line) && i<2; ++i) {} // Skip the header lines
    // Read as long as Result overview is not found
    while (getline(in, line) && line != "Result overview:") {
        ret += line + "\n";
    }
    // Remove the last empty lines and convert to wxString
    return wxString(ret.substr(0,ret.length()-3).c_str(),wxConvUTF8);
}
