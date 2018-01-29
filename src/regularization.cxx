#include <iostream>
#include <sstream>
#include <cstdlib>

#include "ttbarUnfoldAnalysis.h"
#include "FileList.h"

#include "AtlasStyle.h"

void print_usage(void);
bool parse_cmdline(int,char**,std::string&,std::string&,std::string&,unsigned int&);

int main(int argc,char** argv){
   
   std::string mc_filename,data_filename,sample_name;
   unsigned int regValue = 3;
   if( !parse_cmdline(argc,argv,mc_filename,data_filename,sample_name,regValue) ) return 1;
   
   std::cout << " passed mc file list:   " << mc_filename << std::endl;
   std::cout << " passed data file list: " << data_filename << std::endl;
   std::cout << " passed name of the sample to be analyzed: " << sample_name << std::endl;
   std::cout << " maximum regularization value to be analyzed: " << regValue << std::endl;
   
   SetAtlasStyle();
   
   FileList* mc_files = new FileList(mc_filename);
   FileList* data_files = new FileList(data_filename);
  
   ttbarUnfoldAnalysis analysis;
   
   if(!analysis.OptimizeRegValueWithEnsembles(mc_files,data_files,sample_name,regValue)){
      //std::cerr << "[main] ERROR in OptimizeRegValueWithEnsembles\n";
      return -1;
   }
   
   delete mc_files;
   mc_files = 0;
   
   delete data_files;
   data_files = 0;
   
   return 0;
}

void print_usage(void){
   std::cout << " usage: -d <data file list> -m <mc file list> -n <sample name> -r <regularization>\n";
   std::cout << "    data file list: a plain text file containing a list of data files\n";
   std::cout << "                    one per line, each with the full path\n";
   std::cout << "    mc file list:   a plain text file containing a list of mc files\n";
   std::cout << "                    one per line, each with the full path\n";
   std::cout << "    Both file lists should have the same number of files, or the MC\n";
   std::cout << "                    list may contain only one file in the list\n";
   std::cout << "                    in which case, the same MC will be used to compare\n";
   std::cout << "                    to each data file.\n";
   std::cout << "    sample name: base name of the histograms that you would like to analyses\n";
}

bool parse_cmdline(int argc,char** argv,std::string& mc_filename,std::string& data_filename,std::string& sample_name,unsigned int& regValue){
   
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
      else if(arg.find("-n") != std::string::npos && arg.size() == 2){
         if(i+1 < argc)
            sample_name = argv[i+1];
         else{
            print_usage();
            return false;
         }
         i++; // ignore next argument since we already used it.
         continue;
      }
      else if(arg.find("-r") != std::string::npos){
         if(i+1 < argc)
            regValue = atoi(argv[i+1]);
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