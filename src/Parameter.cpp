#include "Parameter.h"
#include <fstream>
#include "funct.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;

Parameter::Parameter() : 
  // Monte Carlo Simulated Annealing
  do_mc('n'), 
  mc_temp(0.0), 
  mc_max_tran_step(0.0),
  mc_max_tran_step_fine(0.0),
  mc_tran_freq(0.5),
  mc_tran_fine_freq(0.5),
  mc_max_rot_step(0.0),
  mc_max_rot_step_fine(0.0),
  mc_rot_freq(0.5),
  mc_rot_fine_freq(0.5),
  mc_niter_out(0),
  mc_niter_in(0),
  sa_alpha(1.0),
  mc_rand_seed(-1),
  // Rigid body minimization
  do_rbmin('n'),
  do_gradient_check('n'),
  max_iter(20),
  eps_grms(0.02),
  alpha_xyz(0.1),
  alpha_rot(0.01),
  learning_rate(0.1)
  { } // default constructor

Parameter::~Parameter() { } // default destructor

void Parameter::printKW(){
  // TODO: finish or consider switching to a dictionary
  cout << "do_mc " << do_mc << endl;
  cout << "mc_temp " << mc_temp << endl;
  cout << "mc_max_tran_step " << mc_max_tran_step << endl;
  cout << "mc_max_tran_step_fine " << mc_max_tran_step_fine << endl;
  cout << "mc_niter_out " << mc_niter_out << endl;
  cout << "mc_niter_in " << mc_niter_in << endl;
  cout << "sa_alpha " << sa_alpha << endl;
}

void Parameter::readKW(char *kwParFil) {
  /* This function reads the keyword-based parameter file */
  std::string line, key, keyval;
  std::string delims = "=";
  std::vector<std::string> split_line;


  std::ifstream inStream(kwParFil);
  if (inStream.fail()){
    cerr << "Cannot open keyword-based parameter file. Program exits!" << endl;
    exit(13);
  }

  while(std::getline(inStream, line)){
    boost::trim(line);
    if (line.length() == 0 || line.find("#") == 0){ // skip blank/comment lines
      continue;
    }
    boost::split(split_line, line, boost::is_any_of(delims));

    key = split_line[0];
    boost::trim(key);
    keyval = split_line[1];
    keyval = keyval.substr(0, keyval.find("#")); // remove possible comments
    boost::trim(keyval);

    /* Now parsing: */
    if (key == "do_mc"){
      do_mc = keyval[0];
      // cout << "do_mc = " << do_mc << endl;
    } 
    else if (key == "do_rbmin"){
      do_rbmin = keyval[0];
      // cout << "do_rbmin = " << do_rbmin << endl;
    } 
    else if (key == "mc_temp")
    {
      mc_temp = std::stod(keyval);
      // cout << "mc_temp = " << mc_temp << endl;
    } 
    else if (key == "mc_max_xyz_step")
    {
      std::istringstream(keyval) >> mc_max_tran_step >> mc_max_tran_step_fine;
      // cout << "xyx_step = " << mc_max_tran_step << " " << mc_max_tran_step_fine << endl;
    } 
    else if (key == "mc_max_rot_step")
    {
      std::istringstream(keyval) >> mc_max_rot_step >> mc_max_rot_step_fine;
      mc_max_rot_step = mc_max_rot_step * M_PI/180.0;
      mc_max_rot_step_fine = mc_max_rot_step_fine * M_PI/180.0;
    } 
    else if (key == "mc_rot_freq")
    {
      mc_rot_freq = std::stod(keyval);
      mc_tran_freq = 1.0 - mc_rot_freq;
    }
    else if (key == "mc_rot_fine_freq"){
      mc_rot_fine_freq = std::stod(keyval);
    }
    else if (key == "mc_xyz_fine_freq")
    {
      mc_tran_fine_freq = std::stod(keyval);
    }
    else if (key == "mc_niter")
    {
      std::istringstream(keyval) >> mc_niter_out >> mc_niter_in;
    }
    else if (key == "mc_sa_alpha")
    {
      sa_alpha = std::stod(keyval);
    } 
    else if (key == "mc_rseed")
    {
      mc_rand_seed = std::stoi(keyval);
    }
    else if (key == "do_gradient_check")
    {
      do_gradient_check = keyval[0];
    }
    else if (key == "min_max_iter")
    {
      max_iter = std::stoi(keyval);
    }
    else if (key == "min_eps_grms")
    {
      eps_grms = std::stod(keyval);
    }
    else if (key == "min_alpha_xyz")
    {
      alpha_xyz = std::stod(keyval);
    }
    else if (key == "min_alpha_rot")
    {
      alpha_rot = std::stod(keyval);
    }
    else if (key == "min_learning_rate")
    {
      learning_rate = std::stod(keyval);
    }
    else
    {
      std::cerr << "The keyword " << key << " was not recognized by SEED. Ignoring it." << std::endl;
    }
  }
  return;
}