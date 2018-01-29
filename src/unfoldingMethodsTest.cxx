#include <iostream>
#include <sstream>
#include <cstdlib>

#include "ttbarUnfoldAnalysis.h"
#include "FileList.h"

#include "AtlasStyle.h"

#include "tclap/CmdLine.h"

int main(int argc,char** argv){
   
   std::string mc_filename,data_filename,
               output_filename,histogram2unfold,
               sampleName,efficiency_filename,
               efficiency_histoname;
   unsigned int nToys=0,regValue=0,iterations=0;
   
   try{
      // Define the command line object, and insert a message
      // that describes the program. The "Command description message" 
      // is printed last in the help text. The second argument is the 
      // delimiter (usually space) and the last one is the version number. 
      // The CmdLine object parses the argv array based on the Arg objects
      // that it contains.
      TCLAP::CmdLine cmd("Performs unfolding using all four methods: SVD, Bayesian, BinByBin, and Simple Matrix Inversion", ' ', "0.1");
      
      
      // Define a value argument and add it to the command line.
      // A value arg defines a flag and a type of value that it expects,
      // such as "-m mc.root" or "--mc-file=mc.root".
      // true/false defines if the value is required
      // "mc.root" is a default to use if it is not defined on the command line
      // "string" is the human readable version of the data type expected
      // "cmd" add this argument to the command line parser
      TCLAP::ValueArg<std::string> mcfileArg("m","mc-file","MC filename",true,"mc.root","string",cmd);
      
      // data filename
      TCLAP::ValueArg<std::string> datafileArg("d","data-file","Data filename",true,"data.root","string",cmd);
      
      // sample name
      TCLAP::ValueArg<std::string> sampleArg("n","sample","defines the histogram paths to use in the MC files and can be set to values like: \n \"Tag1_Top1\" : one or more than 1 b-tagging;best KLFitter;leptonic\n \"Tag1_Top2\" : one or more than 1 b-tagging;best KLFitter;hadronic\n \"Tag1_TopLepHad\" : one or more than 1 b-tagging;best KLFitter;leptonic-hadronic\n \"Tag1_System\" : one or more than 1 b-tagging;best KLFitter;top system\n etc. See SampleDef.h for futher posibilities",true,"Tag1_Top1","string",cmd);
      
      // histogram to unfold
      TCLAP::ValueArg<std::string> dataHistoArg("u","unfold","Histogram path/name in the data file which should be unfolded. If this is omitted then it is assumed that the user is passing MC for both data and MC files so the unfolded histogram will be the MC Reco histogram in the \"data\" file.",false,"","string",cmd);
      
      // number of toys to run
      TCLAP::ValueArg<unsigned int> nToysArg("t","ntoys","Number of toys to run.",false,500,"unsigned integer",cmd);
      
      // regularization value for SVD unfolding
      TCLAP::ValueArg<unsigned int> regValueArg("r","svd-reg","Regularization value used in the SVD unfolding.",false,3,"unsigned integer",cmd);
      
      // iterations value for Bayesian unfolding
      TCLAP::ValueArg<unsigned int> iterationsArg("i","bays-iter","Number of iterations used in the Bayesian unfolding.",false,4,"unsigned integer",cmd);
      
      // output filename
      TCLAP::ValueArg<std::string> outputFileArg("o","output","Output Filename to store histograms",false,"analysis.root","string",cmd);
      
      // efficiency filename
      TCLAP::ValueArg<std::string> effFileArg("g","efficiency-file","Filename where the efficiency plot is stored",false,"","string",cmd);
      
      // efficiency histogram path+name
      TCLAP::ValueArg<std::string> effHistArg("j","efficiency-hist","Histogram name of the efficiency plot is in the efficiency file",false,"","string",cmd);
      
      // Parse the argv array.
      cmd.parse( argc, argv );
      
      // copy values
      mc_filename = mcfileArg.getValue();
      data_filename = datafileArg.getValue();
      output_filename = outputFileArg.getValue();
      histogram2unfold = dataHistoArg.getValue();
      sampleName = sampleArg.getValue();
      nToys = nToysArg.getValue();
      regValue = regValueArg.getValue();
      iterations = iterationsArg.getValue();
      efficiency_filename = effFileArg.getValue();
      efficiency_histoname = effHistArg.getValue();
      
      
   }catch (TCLAP::ArgException &e)  // catch any exceptions
   { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; return -1;}
   
   std::cout << " passed mc file list:   " << mc_filename << std::endl;
   std::cout << " passed data file list: " << data_filename << std::endl;
   
   SetAtlasStyle();
   
   ttbarUnfoldAnalysis analysis;
   if(output_filename.size() > 0)
      analysis.SetOutputFilename(output_filename);
   if(efficiency_filename.size() > 0)
      analysis.SetEfficiencyFilename(efficiency_filename);
   if(efficiency_histoname.size() > 0)
      analysis.SetEfficiencyHistoName(efficiency_histoname);
   
   if(!analysis.RunUnfoldingWithDifferentMethods(mc_filename,data_filename,sampleName,histogram2unfold,nToys,regValue,iterations)){
      std::cerr << "[main] ERROR in RunUnfoldingWithDifferentMethods\n";
      return -1;
   }
   
   return 0;
}
