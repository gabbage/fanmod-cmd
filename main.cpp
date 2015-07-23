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
#include "randesuFrm.h"
#include "tooltips.cpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char** argv){
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("input,i", po::value<std::string>(), "input graph file")
      ("output,o", po::value<std::string>(), "output csv file")
  ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
      std::cout << desc << "\n";
      return 1;
  }
}
