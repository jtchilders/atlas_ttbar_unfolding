
#include "FileHandler.h"
#include "SystematicSample.h"
#include "globals.h"

#include <sstream>


FileHandler::FileHandler(void){
   Init();
}

FileHandler::FileHandler(const std::string outputFilename,const std::string baseDirectory){
   Init();
   SetOutputFilename(outputFilename);
   if(baseDirectory.size() > 0)
      SetBaseDirectory(baseDirectory);
}

FileHandler::~FileHandler(void){


}

void FileHandler::Init(void){
   m_outputFilename           = "analysis.root";
   m_baseDirectory            = "unfolding";
   
   m_ensembleDirectory        = "ensembles";
   m_toyDirectory             = "toys";
   m_unfoldingMethodDirectory = "unfoldingMethods";
   m_migrationToysDirectory   = "migrationToys";
   
   m_rootFile                 = 0;
   
   m_fileOpen                 = false;
   m_directoriesCreated       = false;
}


// open root file for UPDATE (open for write, if doesn't exist it's created)
// first make sure it is not already open
// then create directory structure for unfolding results if not already there.
void FileHandler::Open(void){
   
   //if file pointer doesn't exist create it
   if(!m_rootFile){
      
      // check to see if the file contains directories. if so
      // make sure the path exists
      if(m_outputFilename.find('/') != std::string::npos){
         if(!globals::CreatePath(globals::GetPath(m_outputFilename))){
            std::cerr << "[FileHandler::Open] ERROR Could not create path " << m_baseDirectory << "\n";
            return;
         }
      }
      
      std::cout << "[FileHandler::Open] Opening Root File: " << m_outputFilename << "\n";
      m_rootFile = new TFile(m_outputFilename.c_str(),"UPDATE");
      
      if(!m_rootFile->IsOpen()){
         std::cerr << "[FileHandler::Open] ERROR File was not opened, most likely doesn't exist.\n";
         return;
      }
      
   }
   else{
      std::cout << "[FileHandler::Open] Root File: " << m_outputFilename << " already open.\n";
   }
   
   // create root folder structure
   CreateFilePath(m_rootFile,m_baseDirectory);
   
   
   
   std::string path = m_baseDirectory + '/' + m_ensembleDirectory;
   CreateFilePath(m_rootFile,path);
   
   path = m_baseDirectory + '/' + m_toyDirectory;
   CreateFilePath(m_rootFile,path);
   
   path = m_baseDirectory + '/' + m_unfoldingMethodDirectory;
   CreateFilePath(m_rootFile,path);
   
   path = m_baseDirectory + '/' + m_migrationToysDirectory;
   CreateFilePath(m_rootFile,path);
   
   
   m_fileOpen = true;
   m_directoriesCreated = true;
}

bool FileHandler::cd(std::string path)
{
   if(m_rootFile->cd(path.c_str())) return true;
   else{
      CreateFilePath(m_rootFile,path);
      if(m_rootFile->cd(path.c_str()) ) return true;
      else return false;
   }
   return false;
}

void FileHandler::CreateFilePath(TFile * const file,const std::string path){
   // if path does not exist in the file, create it
   if(!file->cd(path.c_str())){
      std::vector<std::string> directories = SplitPath(path);
      std::string newPath;
      for(unsigned int i=0;i<directories.size();++i){
         std::string previousPath = newPath;
         if(i>0) newPath += '/';
         newPath += directories[i];
         
         if(!file->cd(newPath.c_str())){
            std::cout << "[FileHandler::CreateFilePath] creating " << m_rootFile->GetName() << ":" << newPath << "\n";
            file->cd(previousPath.c_str());
            gDirectory->mkdir(directories[i].c_str());
         }
         else continue;
      }
   }
   else 
      std::cout << "[FileHandler::CreateFilePath] " << m_rootFile->GetName() << ":" << path << " directory exists.\n";
   return;
}

std::vector<std::string> FileHandler::SplitPath(const std::string path){
   
   std::string leftover = path;
   std::vector<std::string> directoriesInPath;
   while(leftover.find('/') != std::string::npos){
      unsigned int pos = leftover.find('/');
      std::string dir = leftover.substr(0,pos);
      leftover = leftover.substr(pos+1,leftover.size());
      directoriesInPath.push_back(dir);
   }
   
   directoriesInPath.push_back(leftover);
   
   return directoriesInPath;
   
}

void FileHandler::Close(void){
   if(m_fileOpen){
      m_rootFile->Close();
      delete m_rootFile;
      m_rootFile = 0;
      m_fileOpen = false;
      globals::set_chmod(m_outputFilename);
   }
}

bool FileHandler::toEnsembles(const std::string subpath){
   std::string path = m_baseDirectory + '/' + m_ensembleDirectory;
   if(subpath.size() > 0)
      path += '/' + subpath;
   return cd(path);
}

bool FileHandler::toEnsembles(const SystematicSample& syst,
                              const std::string elOrMu,
                              const std::string samplename,
                              const ttbarUnfold::UnfoldMethod unfoldMethod,
                              const int regValue,
                              const int ensembleNumber)
{
   std::string base = m_baseDirectory + '/' + m_ensembleDirectory;
   std::string path = GetPathEnsembles(base,syst,elOrMu,samplename,unfoldMethod,regValue,ensembleNumber);
   
   CreateFilePath(m_rootFile,path);
   
   return cd(path);
}

std::string FileHandler::GetPathEnsembles(const std::string base,
                                          const SystematicSample& syst,
                                          const std::string elOrMu,
                                          const std::string samplename,
                                          const ttbarUnfold::UnfoldMethod unfoldMethod,
                                          const int regValue,
                                          const int ensembleNumber)
{
   
   
   std::string path = base;
   path += '/' +  syst.str();
   
   if(elOrMu.size() > 0){
      path += '/' + elOrMu;
   }
   if(samplename.size() > 0){
      path += '/' + samplename;
   }
   
   path += '/' + ttbarUnfold::GetMethodString(unfoldMethod);
   
   if(unfoldMethod == ttbarUnfold::SVD && regValue  >= 0){
      std::stringstream temp;
      temp << regValue;
      path += "/reg" + temp.str();
   }
   if(ensembleNumber  >= 0){
      std::stringstream temp;
      temp << ensembleNumber;
      path += "/ens" + temp.str();
   }
   
   return path;
}

bool FileHandler::toToys(const std::string subpath){
   std::string path = m_baseDirectory + '/' + m_toyDirectory;
   if(subpath.size() > 0)
      path += '/' + subpath;
   
   return cd(path);
}


bool FileHandler::toToys(const SystematicSample& syst,
                         const std::string elOrMu,
                         const std::string samplename,
                         const ttbarUnfold::UnfoldMethod unfoldMethod,
                         const int regValue,
                         const int nToys)
{
   std::string base = m_baseDirectory + '/' + m_toyDirectory;
   std::string path = GetPathToys(base,syst,elOrMu,samplename,unfoldMethod,regValue,nToys);
   
   std::cout << "[FileHandler::toToys] Changing path to " << path << std::endl;
   
   CreateFilePath(m_rootFile,path);
   
   return cd(path);
}

std::string FileHandler::GetPathToys(const std::string base,
                                     const SystematicSample& syst,
                                     const std::string elOrMu,
                                     const std::string samplename,
                                     const ttbarUnfold::UnfoldMethod unfoldMethod,
                                     const int regValue,
                                     const int nToys)
{
   std::string path = base;
   path += '/' + syst.str();
   
   if(elOrMu.size() > 0){
      path += '/' + elOrMu;
   }
   if(samplename.size() > 0){
      path += '/' + samplename;
   }
   
   path += '/' + ttbarUnfold::GetMethodString(unfoldMethod);
   
   if(unfoldMethod == ttbarUnfold::SVD && regValue  >= 0){
      std::stringstream temp;
      temp << regValue;
      path += "/reg" + temp.str();
   }
   
   if(nToys  >= 0){
      std::stringstream temp;
      temp << nToys;
      path += "/toy" + temp.str();
   }
   
   
   return path;
}


bool FileHandler::toMethods(const std::string subpath){
   std::string path = m_baseDirectory + '/' + m_unfoldingMethodDirectory;
   if(subpath.size() > 0)
      path += '/' + subpath;
   
   
   return cd(path);
}

bool FileHandler::toMethods(const ttbarUnfold::UnfoldMethod unfoldMethod){
   std::string path = m_baseDirectory + '/' + m_unfoldingMethodDirectory;
   if(unfoldMethod != ttbarUnfold::NUM_OF_METHODS)
      path += '/' + ttbarUnfold::GetMethodString(unfoldMethod);
   
   
   return cd(path);
}

bool FileHandler::toMigrationToys(const std::string subpath){
   std::string path = m_baseDirectory + '/' + m_migrationToysDirectory;
   if(subpath.size() > 0)
      path += '/' + subpath;
   
   
   return cd(path);
}




