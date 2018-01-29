#include <iostream>
#include <sstream>
#include <cstdlib>

#include "ttbarUnfold.h"
#include "FileList.h"

#include "AtlasStyle.h"
#include "TCanvas.h"

void print_usage(void);
bool parse_cmdline(int,char**,std::string&,std::string&,std::string&);

int main(int argc,char** argv){
   
   std::string mc_filename,data_filename,sample_name;
   if( !parse_cmdline(argc,argv,mc_filename,data_filename,sample_name) ) return 1;
   
   std::cout << " passed mc file:   " << mc_filename << std::endl;
   std::cout << " passed data file: " << data_filename << std::endl;
   std::cout << " passed sample name: " << sample_name << std::endl;   
   
   SetAtlasStyle();
   
   ttbarUnfold unfolding(mc_filename,data_filename,sample_name);
   // regularization parameter for the SVD method
   unsigned int svdRegValue = 3;
   unfolding.SetRegularization(svdRegValue);
   // Number of toy models to use to compute the error matrix (>=500 is best)
   unsigned int nToys = 500;
   unfolding.SetToysForErrors(nToys);
   if(!unfolding.doUnfolding()){
      std::cerr << "[main] ERROR performing Unfolding\n";
      return -1;
   }
   
   TCanvas can("can","can",0,0,800,600);
   can.Divide(2,2);
   
   can.cd(1);
   unfolding.DrawPtSpectra(sample_name);
   can.cd(2);
   unfolding.DrawMigrationMatrix(sample_name);
   can.cd(3);
   unfolding.DrawDeviation();
   can.cd(4);
   can.cd(4)->SetLogy(true);
   unfolding.DrawDistributionD();
   
   can.SaveAs("exampleUnfolding.ps");
   
   return 0;
}

void print_usage(void){
   std::cout << " usage: -d <data file> -m <mc file>\n";
   std::cout << "    <data file> is the file with the data distribution.\n";
   std::cout << "    <mc file>   is the file with the mc reconstruction and truth distributions,\n";
   std::cout << "                and the migration matrix.\n";
}

bool parse_cmdline(int argc,char** argv,std::string& mc_filename,std::string& data_filename,std::string& sample_name){
   
   // should be more than one argument
   if(argc <= 1){
      print_usage();
      return false;
   }
   
   // loop over arguments and set parameters
   std::vector<std::string> argstrings;
   for(unsigned int i=1;i<argc;++i){
      std::string arg = argv[i];
      
      if(arg.find("-d") != std::string::npos && arg.size() == 2){
         if(i+1 < argc)
            data_filename = argv[i+1];
         else{
            print_usage();
            return false;
         }
         i++;
         continue;
      }
      else if(arg.find("-m") != std::string::npos && arg.size() == 2){
         if(i+1 < argc)
            mc_filename = argv[i+1];
         else{
            print_usage();
            return false;
         }
         i++; // ignore next argument since we already used it.
         continue;
      }
      else if(arg.find("-s") != std::string::npos && arg.size() == 2){
         if(i+1 < argc)
            sample_name = argv[i+1];
         else{
            print_usage();
            return false;
         }
         i++; // ignore next argument since we already used it.
         continue;
      }
      else{
         print_usage();
         return false;
      }
      
   }
   
   return true;
   
}



