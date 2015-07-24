/*
 * =====================================================================================
 *
 *    Description:
 *
 *         
 *
 *        Created:  07/23/2015 03:55:19 PM
 *   Organization:  Coini
 *
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <csignal>
#include <stdlib.h>

#include <stdio.h>
#include <unistd.h>
#include <boost/program_options.hpp>
#include "hashmap.h"
#include "maingraph.hpp"
#include "output.hpp"
#include "random.hpp"
#include "tooltips.cpp"

namespace po = boost::program_options;

char num_to_lett[] = {'0','1','2','3','4','5','6','7',
                           '8','9','A','B','C','D','E','F',
                           '#','#','#','#','#','#','#','#',}; //last line for safety only

uint64 calc_expected_samples(double ntrees, const short & G_N, const double *prob)
{
    for (int i = 0; i != G_N; ++i)
    {
   	    ntrees *= prob[i];
	}
    return (uint64)(ntrees+0.5);
}

int main(int argc, char** argv){
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("input,i", po::value<std::string>(), "input graph file")
      ("output,o", po::value<std::string>(), "output csv file")
      ("directed,d", "the graph input is directed")
      ("motif_size,s", po::value<int>(), "the size of the searched motifs. default = 4")
      ("rnd_nets,r", po::value<int>(), "the amount of random nets to compare the motif frequency. default = 1000")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);



  if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
  }

  if (vm.count("input") && vm.count("output")){
    std::string inputfile = vm["input"].as<std::string>();
    std::string outputfile = vm["output"].as<std::string>();

    bool directed = false;
    bool has_edge_colors = false;
    bool has_vertex_colors = false;
    short G_N = 4;
    bool fullenumeration = true;
    double prob[8] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
    uint64 TREESMPLS = 100000;
    long num_r_nets = 1000;
    short random_type = 2;
    bool vertex_color_matters = false;
    bool edge_color_matters = false;
    int num_exchanges = 3;
    int num_tries = 3;
    bool reest_size = false;
    bool text_output = false;
    bool gen_dumpfile = false;

    if(vm.count("directed"))
      directed = true;
    if(vm.count("motif_size"))
      G_N = vm["motif_size"].as<int>();
    if(vm.count("rnd_nets"))
      num_r_nets = vm["rnd_nets"].as<int>();

    // Some vars which are necessary, but not params
    uint64 SMPLS = 1000000;
    long total_tries = 0, total_success = 0;
    int perc_number = 1, netevent_number = 1; // How often are percentage events send to the main program

	// Initialize Graph structure from file
    maingraph maing;
    if (!read_graph(maing, inputfile, directed, has_vertex_colors,
         has_edge_colors, G_N, gen_dumpfile)) {           // If reading of the graph failed
      std::cerr<<"reading graph failed"<<std::endl;
      return -1;
    }

    // Fill the arrays which store neighbour information
    build_graph(maing);

    // v_extension is used in the sampling and est_tree_size-functions
   	long* v_extension = new long[maing.maxnumneighbours*G_N];

    std::signal(SIGTERM, [](int s) {
      //TODO: cleanup like in esuThread.cpp:354
      exit(-1);
    });

    // Write general info into the outputfile
    std::ofstream outfile (outputfile.c_str());
    outfile << "FANMOD 1.1 subgraph "
            << (fullenumeration ? "enumeration" : "sampling") << endl;
    outfile << "-------------------------------\n" << endl;
    outfile << "Network name: " << inputfile << endl;
    outfile << "Network type: " << (maing.directed? "Directed" : "Undirected") << endl;
    outfile << "Number of nodes: " << maing.n << endl
            << "Number of edges: " << maing.m << endl;
    outfile << "Number of single edges: " << maing.num_dir_edges << endl
            << "Number of mutual edges: " << maing.num_undir_edges << endl;
    if (has_vertex_colors) outfile << "Number of vertex colors: " << maing.num_vertex_colors << endl;
    if (has_edge_colors) outfile << "Number of edge colors: " << maing.num_edge_colors << endl;
    outfile << endl << "Algorithm: " << (fullenumeration ? "enumeration" : "sampling") << endl;
    if (!fullenumeration) {
        outfile << "Sampling parameters = {";
		for (int i = 0; i!= G_N; ++i)
			outfile << " " << prob[i];
		outfile << " }" << endl;
    }
    outfile << "Subgraph size: " << G_N << endl << endl;
    string type_str = "";
    if (num_r_nets == 0)
    {
        outfile << "No random graphs were generated " << endl;
    } else {
        switch (random_type) {
            case 0: type_str = "no respect to bidirectional edges,"; break;
            case 1: type_str = "globally constant number of bidirectional edges,";
                    break;
            case 2: type_str = "locally constant number of bidirectional edges,";
                    break;
        }
        if (has_edge_colors && edge_color_matters)
            type_str += "\n   exchanging only edges with the same color,";
        if (has_vertex_colors && vertex_color_matters)
            type_str += "\n   exchanging only edges with the same vertex colors,";
        outfile << "Generated " << num_r_nets << " random networks" << endl
                << "   with " << type_str << endl
                << "   " << num_exchanges << " exchanges per edge and "
                <<  num_tries << " tries per edge." << endl << endl;
    }
    outfile.close(); // Close the file so it is not open during main algorithm.

    // Initializers for main algorithm
    randlib::rand rand(time(NULL));
    // Estimate number of subgraphs by random tree sampling
    uint64 numtrees = 0x28F5C28F5C28F5CULL;

    if (TREESMPLS > 0) {
        numtrees = est_tree_size(maing, v_extension, TREESMPLS, G_N, rand) / TREESMPLS * maing.n;
        std::cout << "Approximate number of subgraphs: " << numtrees
             << " (based on " << TREESMPLS << " samples)\n";
	    if (!fullenumeration){
            SMPLS = calc_expected_samples(double(numtrees), G_N, prob);
            std::cout << "Expected number of sampled subgraphs: " << SMPLS << endl;
        }
    }

    // Main enumeration / sampling loop
    int total_num_nets = num_r_nets + 1; // We sample num_r_nets and the original graph
    // In this hashmap, the intermediate result is stored before entered into the result - hashmap.
    hash_map < graphcode64, uint64 > inter_result;
    // In this hashmap, the results of the sampling and the randomization are stored.
    hash_map < graphcode64, uint64* > result_graphs;
    uint64 *count_subgr = new uint64[total_num_nets];
    uint64 total_subgr = 0;
    double sampling_time = 0.0, random_time = 0.0;
    uint64 *current_array; // Abbrevation for the array currently updated

    // Sample the original graph and random graphs, if necessary.
    EdgeContainer EC;
    uint64 equiv100p = fullenumeration ? numtrees : SMPLS;

    for (int nets_ctr = 0; nets_ctr < total_num_nets; ++nets_ctr){

       // Reestimates tree-size if the user wishes.
       // and it is not the original network being sampled
       if (reest_size && nets_ctr != 0 && TREESMPLS > 0) {
           numtrees = est_tree_size(maing, v_extension, TREESMPLS, G_N, rand) / TREESMPLS * maing.n;
	       if (!fullenumeration) SMPLS = calc_expected_samples(double(numtrees), G_N, prob);
       }

      //Create vector where subgraphs can be dumped to
      vector<subgraph> subgraphdump;
      const bool subgraph_dumpfile = (gen_dumpfile && (nets_ctr == 0));

      // Sample the graph
      // sampling is called with its options,
      // true to show the status-bar and inter_result and count_subgr as result-parameters
      // The frame and this thread are passed to send events and "TestDestroy()"
      sampling_time += sampling(maing, v_extension, G_N, fullenumeration, prob,
                                equiv100p, perc_number, inter_result,
                                count_subgr[nets_ctr], NULL, NULL, rand,
                                subgraph_dumpfile, subgraphdump);
      total_subgr += count_subgr[nets_ctr];

      //Write the subgraph dump
      if (subgraph_dumpfile) {
           //TO DO
           //string outputfile
           std::ofstream dumpfile ((outputfile+".dump").c_str());
           dumpfile << "Number of subgraphs: " << subgraphdump.size() << endl;
           dumpfile << "Format: adjacency matrix, <participating vertices>" << endl;
           graph64 g;
           init_graph(g,G_N,maing.num_vertex_colors,maing.num_edge_colors,maing.directed);
           for (vector<subgraph>::const_iterator iter = subgraphdump.begin();
	                             iter !=subgraphdump.end(); ++iter) {
               readHashCode(g, iter->gc);
               for (short i = 0; i != G_N; ++i) {
                   for (int j = 0; j != G_N; ++j) {
                       dumpfile << num_to_lett[get_element(g,i,j)];
                   }
               }
               for (short i = 0; i != G_N; ++i) {
                   dumpfile << "," << getvertex(*iter,i);
               }
               dumpfile << endl;
           }
           dumpfile.close();
           subgraphdump.clear();
      }

      if (nets_ctr == 0){
        std::cout << count_subgr[0] << " subgraphs were "
               << (fullenumeration ? "enumerated" : "sampled")
               << " in the original network.";
      }

      // Set the perc_number according to how long the maingraph took.
      if (nets_ctr == 0) {
          if (sampling_time < 20)
              perc_number = 5;
          if (sampling_time < 5)
              perc_number = 10;
          if (sampling_time < 1)
              perc_number = 50;
          if (sampling_time < 0.5)
              perc_number = 0; // Send no percentage events
          if (sampling_time < 0.1) {
              netevent_number = int( 1/sampling_time ); // Do not send events every network!
              if (netevent_number <= 0) netevent_number = 1000;
          }
      }

       // Write results into result_graphs-hashmap
       for (hash_map < graphcode64, uint64 >::const_iterator iter = inter_result.begin();
	        iter !=inter_result.end(); ++iter){
           if (result_graphs.find(iter->first) == result_graphs.end()){
               // create the array at the graph's hashmap position.
               current_array = (result_graphs[iter->first] = new uint64[total_num_nets]);
               // Initialize the array
               for (int i=0; i < total_num_nets; ++i)
                    current_array[i] = 0;
           } else {
               // set current_array as the array at the graphs map position
               current_array = result_graphs[iter->first];
           }
           // Write the number of subgraphs in the array
           current_array[nets_ctr] = iter->second;
       } // end for
       inter_result.clear();       // Empty the intermediate result hashmap.

       // Randomize the graph
       if (num_r_nets != 0) {

         if (nets_ctr == 0) {
            //build EC, needs only be done once because sampling
            //does not change the edges and hence the content remains
            //correct
            short color_u = 1, color_v = 1, color_uv, color_vu;
            for (hash_map < edge, edgetype >::const_iterator iter = maing.edges.begin();
                   iter != maing.edges.end(); ++iter) {
              const edge e = iter->first;
              if (vertex_color_matters) {
                 color_u = maing.vertex_colors[edge_get_u(e)];
                 color_v = maing.vertex_colors[edge_get_v(e)];
              }
              const edgetype et = iter->second;
              if (edge_color_matters) {
                   color_uv = get_color_u_v(et);
                   color_vu = get_color_v_u(et);
              } else {
                   color_uv = et & DIR_U_T_V;
                   color_vu = (et & DIR_V_T_U) >> 1;
              }
              //Put edge into EC. For NO_REGARD, the bidirectional
              //edge is split up, additionally, the edge code shows
              //an edge's direction
              switch (random_type) {
                     case NO_REGARD    : if ((et & DIR_U_T_V) == DIR_U_T_V)
                                            EC.put(new_edge(edge_get_u(e),edge_get_v(e)), getBagID(color_uv, 0 ,color_u,color_v) );
                                         if ((et & DIR_V_T_U) == DIR_V_T_U)
                                            EC.put(new_edge(edge_get_v(e),edge_get_u(e)), getBagID( 0 , color_vu ,color_u,color_v) );
                                         break;
                     case GLOBAL_CONST : EC.loggedPut(e, getBagID(color_uv,color_vu,color_u,color_v) );
                                         break;
                     case LOCAL_CONST  : EC.put(e, getBagID(color_uv,color_vu,color_u,color_v) );
                                         break;
              }
         }

         }
         random_time += randomize_graph(maing, random_type, num_exchanges, num_tries,
                                        vertex_color_matters, edge_color_matters,
                                        EC, total_tries, total_success, rand);
       }
    } // end for


    // Free memory
    maing.edges.clear();
    for (vertex i = 0; i != maing.n; ++i) {
      delete[] maing.neighbours[i] ;
    }
    delete[] maing.num_neighbours;
    delete[] maing.neighbours;
    //delete[] maing.num_larger_neighbours;
    delete[] maing.v_util;
    delete[] v_extension;

    outfile.open(outputfile.c_str(), std::ofstream::out | std::ofstream::app);
    outfile << count_subgr[0] << " subgraphs were "
            << (fullenumeration ? "enumerated" : "sampled")
            << " in the original network." << endl;
    if (num_r_nets != 0) {
        outfile << (total_subgr-count_subgr[0]) << " subgraphs were "
                << (fullenumeration ? "enumerated" : "sampled")
                << " in the random networks." << endl;
        outfile << total_subgr << " subgraphs were "
                << (fullenumeration ? "enumerated" : "sampled")
                << " in all networks." << endl << endl;

        outfile << "For the random networks: " << total_tries << " tries were made, "
                << total_success << " were successful." << endl;
        outfile << "Randomization took " << random_time << " seconds." << endl;
    } else {
        outfile << endl;
    }
    outfile << (fullenumeration ? "Enumeration took " : "Sampling took ")
            << sampling_time << " seconds.\n\n" << endl;
    // Output the results table
    // if text_output is false, CSV output is given.
    pretty_output(text_output, result_graphs, G_N, maing.num_vertex_colors,
                  maing.num_edge_colors, maing.directed, count_subgr,
                  num_r_nets, outfile);
    outfile.close();

    // Free Memory of result storage
    for (hash_map < graphcode64, uint64* >::const_iterator iter = result_graphs.begin();
         iter != result_graphs.end(); ++iter){
        delete[] iter->second;
    }
    result_graphs.clear();
    delete[] count_subgr;

  } else {
    std::cerr<<"input and output files need to be specified"<<std::endl;
    return -1;
  }
}
