#include "LatexDoc.h"


LatexDoc::LatexDoc(){
   m_doc_class       = "article";
   m_doc_args        = "a4paper,12pt";
}

void LatexDoc::WriteToFile(const std::string output_filename){
   
   m_output_filename = output_filename;
   
   std::stringstream s;
   
   s << "\\documentclass[" << m_doc_args << "]{" << m_doc_class << "}\n\n";
   s << m_doc_header << "\n";
   s << "\\begin{document}\n\n";
   s << m_doc_body << "\n";
   s << "\\end{document}\n\n";
   
   std::ofstream outfile(m_output_filename.c_str());
   if(!outfile){
      std::cerr << "[LatexDoc::WriteToFile] ERROR opening file for output: " << m_output_filename << "\n";
      return;
   }
   
   outfile << s.str();
   
   outfile.close();
   
}

void LatexDoc::MakePdf() const{
   
   std::string filename = m_output_filename;
   std::string extension = ".tex";
   int index = filename.find(extension);
   if(index != std::string::npos){
      filename.replace(index,extension.size(),"");
   }
   
   std::string latex = "latex ";
   latex += filename;
   system(latex.c_str());
   system(latex.c_str());
   
   std::string bibtex = "bibtex ";
   bibtex += filename;
   system(bibtex.c_str());
   
   system(latex.c_str());
   
   std::string dvi2pdf = "dvi2pdf ";
   bibtex += filename;
   system(bibtex.c_str());
   
}

LatexDoc* LatexDoc::GetTopDiffInstance(){
   LatexDoc* output = new LatexDoc();
   
   std::stringstream header;
   header << "\\usepackage{xspace}\n";
   header << "\\usepackage{graphicx}\n";
   header << "\\graphicspath{{eps/}}\n";
   header << "\\newcommand{\\Alpgen}{{\\sc Alpgen}\\xspace}\n";
   header << "\\newcommand{\\McAtNlo}{{\\sc MC{@}NLO}\\xspace}\n";
   header << "\\newcommand{\\PowHeg}{{\\sc PowHeg}\\xspace}\n";
   header << "\\newcommand{\\MCFM}{{\\sc MCFM}\\xspace}\n";
   header << "\\newcommand{\\ejets}{{$e+$jets}\\xspace}\n";
   header << "\\newcommand{\\mujets}{{$\\mu+$jets}\\xspace}\n";
   header << "\\newcommand{\\ljets}{{$\\ell+$jets}\\xspace}\n";
   
   // from atlasphysics.sty
   header << "\\def\\antibar#1{\\ensuremath{#1\\bar{#1}}}\n";
   header << "\\def\\tbar{\\ensuremath{\\bar{t}}}\n";
   header << "\\def\\ttbar{\\antibar{t}}\n";
   
   header << "\\newcommand{\\pt}{\\ensuremath{p_T}}\n";
   
   header << "\\newcommand{\\ptt}{\\ensuremath{\\pt^t}}\n";
   header << "\\newcommand{\\dptt}{\\ensuremath{{\\rm d}\\ptt}}\n";
   
   header << "\\newcommand{\\ptttbar}{\\ensuremath{\\pt^{\\ttbar}}}\n";
   header << "\\newcommand{\\dptttbar}{\\ensuremath{{\\rm d}\\ptttbar}}\n";
   
   header << "\\newcommand{\\mttbar}{\\ensuremath{M_{\\ttbar}}}\n";
   header << "\\newcommand{\\dmttbar}{\\ensuremath{{\\rm d}\\mttbar}}\n";
   
   header << "\\newcommand{\\yttbar}{\\ensuremath{y_{\\ttbar}}}\n";
   header << "\\newcommand{\\dyttbar}{\\ensuremath{{\\rm d}\\yttbar}}\n";
   
   header << "\\newcommand{\\dsigma}{\\ensuremath{{\\rm d}\\sigma}}\n";
   
   
   output->SetHeaderText(header.str());
   
   return output;
}

