#ifndef FILEHANDLER_H
#define FILEHANDLER_H


#include <iostream>
#include <string>

#include "TFile.h"
#include "ttbarUnfold.h"

class SystematicSample;

class FileHandler{
public:
   FileHandler();
   FileHandler(const std::string outputFilename,const std::string baseDirectory = "");
   ~FileHandler();
   
   void SetOutputFilename(const std::string outputFilename){m_outputFilename = outputFilename;};
   std::string GetOutputFilename(void){return m_outputFilename;};
   
   void SetBaseDirectory(const std::string baseDirectory){m_baseDirectory = baseDirectory;};
   std::string GetBaseDirectory(void){return m_baseDirectory;};
   
   void Open(void);
   void Close(void);
   
   bool cd(std::string path);
   
   bool toEnsembles    (const std::string subpath = "");
   bool toEnsembles    (const SystematicSample& syst,
                        const std::string elOrMu,
                        const std::string samplename = "",
                        const ttbarUnfold::UnfoldMethod unfoldMethod = ttbarUnfold::SVD,
                        const int regValue = -1,
                        const int ensembleNumber = -1);
   bool toToys         (const std::string subpath = "");
   bool toToys         (const SystematicSample& syst,
                        const std::string elOrMu,
                        const std::string samplename = "",
                        const ttbarUnfold::UnfoldMethod unfoldMethod = ttbarUnfold::SVD,
                        const int regValue = -1,
                        const int nToys = -1);
   bool toMethods      (const std::string subpath = "");
   bool toMethods      (const ttbarUnfold::UnfoldMethod unfoldMethod);
   bool toMigrationToys(const std::string subpath = "");
   
   static std::string GetPathEnsembles(const std::string base,
                                       const SystematicSample& syst,
                                       const std::string elOrMu,
                                       const std::string samplename,
                                       const ttbarUnfold::UnfoldMethod unfoldMethod,
                                       const int regValue,
                                       const int ensembleNumber);
   static std::string GetPathToys(const std::string base,
                                  const SystematicSample& syst,
                                  const std::string elOrMu,
                                  const std::string samplename,
                                  const ttbarUnfold::UnfoldMethod unfoldMethod,
                                  const int regValue,
                                  const int nToys);
   
private:
   TFile* m_rootFile;
   std::string m_outputFilename;
   
   // variables that hold directory structure
   
   // this is the base directory where everything is stored
   std::string m_baseDirectory;
   
   // This is the top directory for ensemble results
   std::string m_ensembleDirectory;
   // This is the top directory for toy results
   std::string m_toyDirectory;
   // This is the top directory for unfodling method results
   std::string m_unfoldingMethodDirectory;
   // This is the top directory for migration toy results
   std::string m_migrationToysDirectory;
   
   bool m_fileOpen;
   bool m_directoriesCreated;
   
   void Init(void);
   void CreateFilePath(TFile * const file,const std::string path);
   std::vector<std::string> SplitPath(const std::string path);
};



#endif