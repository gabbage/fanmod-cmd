// Unfortunately, tooltips do not work with static texts, e.g. the "elapsed time" text
const wxString subgr_size_tooltip = wxT("Size of the subgraphs that are to be enumerated / sampled.");
const wxString samples_tooltip = wxT("Number of samples that are used to determine the approximate number of subgraphs.\nSetting this low will make the initialization of the algorithm faster but cause the progress bar to be less accurate.");
const wxString full_enum_tooltip = wxT("All subgraphs of the selected size are enumerated in the original network and in the random networks.");
const wxString sampling_tooltip = wxT("Enables subgraph sampling instead of full enumeration. For details regarding the sampling procedure, please refer to the paper \n   S. Wernicke \n   A Faster algorithm for detecting\n          network motifs. \n   Proceedings of WABI '05 \n   Springer LNBI 3692, 165--177 \n   Springer-Verlag, October 2005");
const wxString sampling_param_tooltip = wxT("Sampling parameters for traversing the enumeration tree. The expected number of samples that is pbtained is\n   (number of subgraphs) x (parameters).\nFor details regarding this parameter, please refer to the paper \n   S. Wernicke \n   A Faster algorithm for detecting\n          network motifs. \n   Proceedings of WABI '05 \n   Springer LNBI 3692, 165--177 \n   Springer-Verlag, October 2005");;
const wxString random_type_tooltip = wxT("For directed graphs, three different random graph models can be selected that differ in how the handle bidirectional edges:\n  - \'No regard\' does not preserve the\n    number of bidirectional edges\n  - \'Global const\' keeps the number\n    of bidirectional edges globally\n    constant in the network\n  - \'Local const\' preserves the number\n    of adjoining bidirectional edges\n    for every vertex separately.");
const wxString regard_ec_tooltip = wxT("Specifies that when randomizing graphs, adjacent edge colors are kept constant.");
const wxString regard_vc_tooltip = wxT("Specifies that when randomizing graphs, edges can only be exchanged between endpoints of equal color.");
const wxString number_rnets_tooltip = wxT("Chooses the number of random networks that are generated for determining subgraph significance.");
const wxString reest_subgr_tooltip = wxT("Causes the number of subgraphs for each random network to be reestimated. Setting this will cause the progrees bar to be more accurate but the randomization is slowed down.");
const wxString exchanges_tooltip = wxT("Sets the number of times the endpoints of each edge are \'shuffled\' during the generation of random networks.");
const wxString attempts_tooltip = wxT("If an edge cannot be \'shuffled\' at once during the edge exchange randomization (because no exchange partner is found), this parameter determines the number of additional tries that are made to achieve the exchange.");
const wxString inputfile_tooltip = wxT("Selects the input file containing the network to be analyzed. As input format, a plain ASCII file is expected that contains a line for each edge of the network in the format\n	 A  B  X\nwhere \'A\' and \'B\' are integers.");//  and the third number is optional (and redundant) due to compatibility issues with some network formats.\n");
const wxString directed_tooltip = wxT("Specifies that the input graph is directed (direction of an edge then is from first vertex to second vertex in each line of the input file).");
const wxString colored_vertices_tooltip = wxT("Specifies that the input graph has colored vertices.");
const wxString colored_edges_tooltip = wxT("Specifies that the input graph has colored edges.");
const wxString outfile_tooltip = wxT("Select output file where results are written to.");
const wxString textout_tooltip = wxT("Output the results to a textfile.\nThis is nicer to read than CSV but may be harder to process automatically with some applications.");
const wxString csv_tooltip = wxT("Output the results in a CSV (Comma Separated Values) format.\nThis is not readable for humans but easy to process automatically (e.g., with a spreadsheet applications).");
const wxString start_button_tooltip = wxT("Start the enumeration / sampling (once all parameters have been set).");
const wxString progress_bar_tooltip = wxT("Note that - depending on the number of samples that is used to estimate the overall number of subgraphs - these progress bars may not be too accurate (especially the bottom one).");
const wxString detailed_view_tooltip = wxT("Switch to a nice and detailed view of the calculated results.\nAllows for HTML export of results.");
const wxString loadfile_tooltip = wxT("Loads previously calculated results from a file into the viewer.");
const wxString dump_tooltip = wxT("Outputs a file named <outputfile>.dump that contains a list of all subgraphs found in the input graph as a list (including their adjacency matrix and the participating vertices).");


// Tooltips in the HTML-Export window
const wxString htmlfile_tooltip = wxT("Name for the output file (if more than one file is generated, these will be numbered subsequently).");
const wxString motifs_per_file_tooltip = wxT("Specifiex number of motifs per HTML page.");
const wxString create_pictures_tooltip = wxT("Use pictures to denote motifs instead of adjacency matrices.");
const wxString z_score_filter_tooltip = wxT("Only show motifs with a Z-score significance above the specified threshold.\nThe Z-score is the number of standard deviations that the concentration in the input network differs from the mean concentration in the random networks.");
const wxString p_value_filter_tooltip = wxT("Only show motifs with a P-value significance above the specified threshold.\nThe P-value is the percentage of random networks in which a higher concentration of a motif was found than present in the inout network.");
const wxString frequency_filter_tooltip = wxT("Only show motifs that are displayed above a specified minimum frequency.");
const wxString number_filter_tooltip = wxT("Only show motifs that are displayed at least a specified number of times.");
const wxString dangling_filter_tooltip = wxT("A dangling edge is one that starts or ends at a degree-1 vertex. Sometimes, motifs with a certain number of dangling edges are not considered as interesting motifs. These can be ignored here.");
const wxString sort_box_tooltip = wxT("Specifies which criterion is used to sort the subgraphs that pass all filters.");
const wxString undef_box_tooltip = wxT("Specifies how motifs which are never found in random networks (and which hence do not have a defined Z-Score) are to be handled.");
const wxString generate_button_tooltip = wxT("Generate the HTML file(s) and open the index page in your default browser.");
