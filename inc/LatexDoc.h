#ifndef LATEXDOC_H
#define LATEXDOC_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

class LatexDoc{
public:
   LatexDoc();
   
   void SetDocArgs(const std::string text){m_doc_args=text;};
   void SetDocClass(const std::string text){m_doc_class=text;};
   
   void SetHeaderText(const std::string text){m_doc_header=text;};
   void AddHeaderText(const std::string text){m_doc_header+=text;};
   
   void SetBodyText(const std::string text){m_doc_body=text;};
   void AddBodyText(const std::string text){m_doc_body+=text;};
   
   void WriteToFile(const std::string output_filename = "latex.tex");
   
   void MakePdf() const;
   
   static LatexDoc* GetTopDiffInstance();
   
private:
   std::string m_doc_class;
   std::string m_doc_args;
   std::string m_doc_header;
   std::string m_doc_body;
   
   std::string m_output_filename;
};

#endif