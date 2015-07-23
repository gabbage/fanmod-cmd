#ifndef esuThread_HPP
#define esuThread_HPP

#include <wx/wxprec.h>
#ifdef __BORLANDC__
        #pragma hdrstop
#endif
#ifndef WX_PRECOMP
        #include <wx/wx.h>
#endif

#include <wx/thread.h>

#include <iostream>
using std::endl;
#include <fstream>
#include <sstream>
#include <vector>
using std::vector;
#include <stdexcept>
using std::domain_error;
#include <string>
using std::string;


#include <stdlib.h>

#include "hashmap.h"
#include "maingraph.hpp"
#include "output.hpp"
#include "random.hpp"

class esuThread : public wxThread
{
public:
    esuThread(const wxString & inputfile_p, bool directed_p, bool has_vertex_colors_p,
              bool has_edge_colors_p, long G_N_p, bool fullenumeration_p, 
              double prob_p[8], long treesmpls_p, long num_r_nets_p, short random_type_p,
              const bool vertex_color_matters_p, const bool edge_color_matters_p,
              long num_exchanges_p, long num_tries_p, bool reest_size_p,
              const wxString & outputfile_p, bool text_output_p, bool gen_dumpfile_p, 
              wxFrame *frame_p);
    virtual void *Entry();
private:
    //Parameters of the randesu function
    string inputfile;
    bool directed;
    bool has_edge_colors;
    bool has_vertex_colors;    
    short G_N;
    bool fullenumeration; 
    double prob[8];
    uint64 TREESMPLS;
    long num_r_nets;
    short random_type;
    const bool vertex_color_matters;
    const bool edge_color_matters;
    int num_exchanges;
    int num_tries;
    bool reest_size;
    string outputfile;
    bool text_output;
    bool gen_dumpfile;
    //The frame which called the thread
    wxFrame * frame;
};

// utility function
uint64 calc_expected_samples(double ntrees, const short & G_N, const double *prob);

// IDs for the events
#define ID_THREAD_INFO 104
#ifndef ID_PERCENT_REACHED
    #define ID_PERCENT_REACHED 103
#endif
#define ID_NET_FINISHED 102
#define ID_THREAD_NO_SUCCESS 101
#define ID_THREAD_FINISHED  100

#endif
