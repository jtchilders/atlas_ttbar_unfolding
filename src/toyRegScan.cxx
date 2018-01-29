#include <iostream>
#include <sstream>
#include <cstdlib>

#include "ttbarUnfoldAnalysis.h"
#include "FileList.h"
#include "SystematicSample.h"

#include "AtlasStyle.h"

#include "tclap/CmdLine.h"
#include "tclap/Arg.h"
#include "tclap/ArgException.h"

int main(int argc,char** argv){
   
   std::string mc_filename,data_filename,
   output_filename,histogram2unfold,sampleName,
   efficiency_filename,efficiency_histoname;
   SystematicSample systematicSample;
   unsigned int nToys=0,regValue=0,iterations=0;
   ttbarUnfold::UnfoldMethod method = ttbarUnfold::SVD;
   
   try{
      // Define the command line object, and insert a message
      // that describes the program. The "Command description message" 
      // is printed last in the help text. The second argument is the 
      // delimiter (usually space) and the last one is the version number. 
      // The CmdLine object parses the argv array based on the Arg objects
      // that it contains.
      TCLAP::CmdLine cmd("Performs unfolding and runs poisson toys of the measurement.", ' ', "0.1");
      
      
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
      
      // systematic samples
      std::string description = "Systematic sample to use, can be: ";
      for(SystematicSample::Sample sample=(SystematicSample::Sample)0;
          (int)sample<(int)SystematicSample::NUM_OF_SAMPLES;
          sample = (SystematicSample::Sample)((int)sample + 1))
      {
         description += SystematicSample::GetSampleString(sample) + ", ";
      }
      TCLAP::ValueArg<std::string> systArg("e","systematic",description,false,"nominal","string",cmd);
      
      // unfolding methods
      TCLAP::SwitchArg svdSwitch("s","svd","Use SVD unfolding method (default)", true);
      TCLAP::SwitchArg bayesianSwitch("b","bayesian","Use Bayesian unfolding method",false);
      TCLAP::SwitchArg binbybinSwitch("c","binbybin","Use BinByBin unfolding method",false);
      TCLAP::SwitchArg inversionSwitch("f","inversion","Use Simple Matrix Inversion unfolding method",false);
      std::vector<TCLAP::Arg*> xorList;
      xorList.push_back(&svdSwitch);
      xorList.push_back(&bayesianSwitch);
      xorList.push_back(&binbybinSwitch);
      xorList.push_back(&inversionSwitch);
   
      cmd.xorAdd(xorList);
   
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
      systematicSample = systArg.getValue();
      efficiency_filename = effFileArg.getValue();
      efficiency_histoname = effHistArg.getValue();
   
      if(svdSwitch.isSet())
         method = ttbarUnfold::SVD;
      else if(bayesianSwitch.isSet())
         method = ttbarUnfold::Bayes;
      else if(binbybinSwitch.isSet())
         method = ttbarUnfold::BinByBin;
      else if(inversionSwitch.isSet())
         method = ttbarUnfold::Inversion;
      else{
         throw( TCLAP::CmdLineParseException("Must only specify one unfolding method") );
      }
      
      
   }catch (TCLAP::ArgException &e)  // catch any exceptions
   { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; return -1;}
   
   
   std::cout << " passed mc file name:   " << mc_filename << std::endl;
   std::cout << " passed data file name: " << data_filename << std::endl;
   std::cout << " passed output file name: " << output_filename << std::endl;
   std::cout << " passed efficiency file name: " << efficiency_filename << std::endl;
   std::cout << " passed efficiency histo name: " << efficiency_histoname << std::endl;
   std::cout << " passed mc sample name: " << sampleName << std::endl;
   std::cout << " passed data histo path: " << histogram2unfold << std::endl;
   std::cout << " passed number of toys to use: " << nToys << std::endl;
   std::cout << " passed regularization value to use: " << regValue << std::endl;
   std::cout << " passed iterations value to use: " << iterations << std::endl;
   
   SetAtlasStyle();
   
   ttbarUnfoldAnalysis analysis;
   
   analysis.SetEfficiencyFilename(efficiency_filename);
   analysis.SetEfficiencyHistoName(efficiency_histoname);
   
   if(!analysis.RunToyUnfoldingForRegScan(mc_filename,data_filename,systematicSample,sampleName,histogram2unfold,method,regValue,nToys)){
      std::cerr << "[main] ERROR in RunToyUnfolding\n";
      return -1;
   }
   
   return 0;
}



