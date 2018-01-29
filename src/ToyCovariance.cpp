// jiri kvita 7.4.2013, 11.6.2013

#include "ToyCovariance.h"
#include "SystematicBinList.h"


// ________________________________________________________________

ToyCovariance::ToyCovariance(int Nbins) : _Nbins(Nbins),
					  _nHistBins(1),
					  _Zmin(-100), _Zmax(100),
					  _Cov(0), _Corr(0), _debug(0), 
					  _mode(1),
					  _UseMeanClass(true)
{
  this -> Init();
}

// ________________________________________________________________

ToyCovariance::ToyCovariance(int Nbins, int debug, int Mode, bool UseMeanClass) : _Nbins(Nbins),
										  _nHistBins(1),
										  _Zmin(-100), _Zmax(100),
										  _Cov(0), _Corr(0), _debug(debug), 
										  _mode(Mode),
										  _UseMeanClass(true)
{
  this -> Init();
}


// ________________________________________________________________

ToyCovariance::ToyCovariance(int Nbins, int HistBins, double Min, double Max, int debug, int Mode, bool UseMeanClass) : _Nbins(Nbins),
															_nHistBins(HistBins),
															_Zmin(Min), _Zmax(Max),
															_Cov(0), _Corr(0), _debug(debug), 
															_mode(Mode),
															_UseMeanClass(UseMeanClass)
    // Mode:
    // 0: build the covariance from <x*y> - <x>*<y>
    // otherwise it is assumed that the covariance is to be filled directly in two passes, see the FillMode in the Fill method


{
  this -> Init();
}

// ________________________________________________________________

void ToyCovariance::Init()
{

  for (unsigned int i = 0; i < _Nbins; ++i) {
    string name = Form("hmean_%i", i);
    _hmeans.push_back(new TH1D(name.c_str(), name.c_str(), _nHistBins, _Zmin, _Zmax));

    vector<TH1D*> vecprod;
    vector<TH2D*> veccorr;
    for (unsigned int j = 0; j <= i; ++j) {
      name = Form("hprod_%i_%i", i, j);
      vecprod.push_back(new TH1D(name.c_str(), name.c_str(), _nHistBins, (_Zmin < 0 ? -1. : 1.)*_Zmin*_Zmin,  (_Zmax < 0 ? -1. : 1.)*_Zmax*_Zmax));
      name = Form("hcorr2d_%i_%i", i, j);
      veccorr.push_back(new TH2D(name.c_str(), name.c_str(), 200, _Zmin, _Zmax, 200, _Zmin, _Zmax));

    }
    _hproducts.push_back(vecprod);
    _hcorr2d.push_back(veccorr);
    
    
    if(_UseMeanClass){
       MeanValue empty;
       _mmeans.push_back(empty);
       _mproducts.push_back(std::vector<MeanValue>::vector<MeanValue>());
       for (unsigned int j = 0; j <= i; ++j)
          _mproducts[i].push_back(empty);
    }
  }
  
  

}
// ________________________________________________________________
void ToyCovariance::GetIndices(int &ii, int &jj) 
{
  if (ii < jj) {
    int kk = ii;
    ii = jj;
    jj = kk;
  }
}

// ________________________________________________________________

void ToyCovariance::Fill(int i, double z1, int j, double z2, int FillMode)
{

  // FillMode:
  // 0: old way of filling both means and products
  // 1: Fill only means
  // 2: Fill directly the Covariances using the means obtained by running the first pass of toys
  if ( _mode == 0 and (FillMode == 1 or FillMode == 2) )  {
    cerr << "ERROR, class NOT initialized for direct covariance filling and the two passes scheme!" << endl;
    return;
  }
  if ( _mode != 0 and (FillMode != 1 and FillMode != 2) )  {
    cerr << "ERROR, class initialized for direct covariance filling and the two passes scheme, but atempted to be filled in old way!" << endl;
    return;
  }

  if (FillMode != 2) {
    if (_debug > 1) 
      cout << "fill mean z1=" << z1 << " z2=" << z2 << endl;
    if (i < _hmeans.size()) {
       if(_UseMeanClass) _mmeans[i].AddEntry(z1);
      _hmeans[i]->Fill(z1);
    } else cerr << "ERROR accesing diagonal i=" << i << endl;
    if (i < _hmeans.size()) {
       if(_UseMeanClass) _mmeans[j].AddEntry(z2);
      _hmeans[j]->Fill(z2);
    } else cerr << "ERROR accesing diagonal j=" << j << endl;
  }
  if (FillMode == 1) {
    return;
  } else if (FillMode == 2) {
    // ensure the triangularity:
    int ii = i;
    int jj= j;
    this -> GetIndices(ii, jj);
    if (ii < _hproducts.size()) {
      if (jj < _hproducts[ii].size()) {
	if (_debug > 1) 
	  cout << "fill cov "
	       << " " << (z1 - _hmeans[ii] -> GetMean())
	       << " " << (z2 - _hmeans[jj] -> GetMean())
	       << " " << (z1 - _hmeans[ii] -> GetMean())*(z2 - _hmeans[jj] -> GetMean())
	       << endl;
   if(_UseMeanClass)  _mproducts[ii][jj].AddEntry((z1-_mmeans[ii].GetMean())*(z2-_mmeans[jj].GetMean()));
	_hproducts[ii][jj]->Fill((z1 - _hmeans[ii] -> GetMean())*(z2 - _hmeans[jj] -> GetMean()));
	_hcorr2d[ii][jj]->Fill(z1 - _hmeans[ii] -> GetMean(), z2 - _hmeans[jj] -> GetMean());
      } else cerr << "ERROR accesing column " << jj << endl;
    } else cerr << "ERROR accesing line " << ii << endl;
    return;
  }
  
  // fill all in case FillMode is not 1 nor 2:
  
  // ensure the triangularity:
  int ii = i;
  int jj= j;
  this -> GetIndices(ii, jj);
  
  if (ii < _hproducts.size()) {
    if (jj < _hproducts[ii].size()) {
      if (_debug > 1) 
         cout << "fill prod" << endl;
      if(_UseMeanClass) _mproducts[ii][jj].AddEntry(z1*z2);
      _hproducts[ii][jj]->Fill(z1*z2);
      _hcorr2d[ii][jj]->Fill(z1, z2);
      
    } else cerr << "ERROR accesing column " << jj << endl;
  } else cerr << "ERROR accesing line " << ii << endl;
  
  if (_debug > 1) 
    cout << "Done Fill" << endl;
  
} // Fill

// ________________________________________________________________

void ToyCovariance::Fill(TH1D *htoy, int FillMode)
{
  for (int i = 0; i < _Nbins; ++i) {
      for (int j = 0; j <= i; ++j) {
	if (_debug > 1) {
	  cout << htoy -> GetName() << endl;
	  cout << i << " " << htoy -> GetBinContent(i+1) << endl;
	  cout << j << " " << htoy -> GetBinContent(j+1) << endl;
	}
	this -> Fill(i, htoy -> GetBinContent(i+1), j, htoy -> GetBinContent(j+1), FillMode);
      }
    }
} // Fill

// ________________________________________________________________

void ToyCovariance::Fill(const SystematicBinList &htoy, int FillMode)
{
  for (int i = 0; i < htoy.size(); ++i) {
      for (int j = 0; j <= i; ++j) {
	if (_debug > 1) {
	  // cout << htoy -> GetName() << endl;
	  cout << i << " " << htoy[i].content() << endl;
	  cout << j << " " << htoy[j].content() << endl;
	}
	this -> Fill(i, htoy[i].content(), j, htoy[j].content(), FillMode);
      }
    }
} // Fill



// ________________________________________________________________

TCanvas* ToyCovariance::DrawProductHistos()
{

  TString name = "canproducts";
  TCanvas *can = new TCanvas(name, name, 0, 400, 500, 500);
  can -> Divide(_Nbins, _Nbins);
   for (int i = 0; i < _Nbins; ++i) {
      for (int j = 0; j <= i; ++j) {
	can -> cd(1+i*_Nbins + j);
	_hproducts[i][j] -> Draw("hist");
      }
   }
   return can;
}

// ________________________________________________________________

TCanvas* ToyCovariance::DrawCorr2d()
{

  TString name = "can2d";
  TCanvas *can = new TCanvas(name, name, 800, 500, 800, 600);
  can -> Divide(_Nbins, _Nbins);
   for (int i = 0; i < _Nbins; ++i) {
      for (int j = 0; j <= i; ++j) {
	can -> cd(1+i*_Nbins + j);
	_hcorr2d[i][j] -> Draw("col");
      }
   }
   return can;
}


// ________________________________________________________________

TCanvas* ToyCovariance::DrawMeanHistos()
{
  TString name = "canmean";
  TCanvas *can = new TCanvas(name, name, 0, 0, 1000, 400);
  can -> Divide(_Nbins, 1);
   for (int i = 0; i < _Nbins; ++i) {
     can -> cd(1+i);
     _hmeans[i] -> Draw("hist");
   }
   return can;
}

// ________________________________________________________________

void ToyCovariance::PrintMeans()
{
  cout << "Means: ";
   for (int i = 0; i < _Nbins; ++i) {
     
      cout << "bin" << i << " ";
      if(_UseMeanClass) 
         cout << _mmeans[i] . GetMean() << " ";
      else
         cout << _hmeans[i] -> GetMean() << " ";
   }
   cout << endl;
}

// ________________________________________________________________

void ToyCovariance::PrintSigmas()
{
  cout << "Sigmas: ";
   for (int i = 0; i < _Nbins; ++i) {
     cout << "bin" << i << " " << sqrt((*_Cov)[i][i]) << " ";
   }
   cout << endl;
}

// ________________________________________________________________

void ToyCovariance::PrintProducts() {
  cout << "Products mean matrix: " << endl;
  for (int i = 0; i < _Nbins; ++i) {
    for (int j = 0; j < _Nbins; ++j) {
      cout << _hproducts[i][j] -> GetMean() << " ";
    }
    cout << endl;
  }
}
// ________________________________________________________________

TMatrixD *ToyCovariance::GetCovariance()
{
  this -> EnsureCov();
  return _Cov;
}

// ________________________________________________________________

TMatrixD *ToyCovariance::GetCorrelation()
{
  this -> EnsureCorr();
  return _Corr;
}



// ________________________________________________________________
 TMatrixD * ToyCovariance::MakeEmptyCov()
  {
    return  new TMatrixD(_Nbins, _Nbins);
  }

// ________________________________________________________________
  void ToyCovariance::EnsureCov()
  {
    if (_Cov)
      return;

    _Cov = this -> MakeEmptyCov();

    if (_mode == 0) {
	for (int i = 0; i < _Nbins; ++i) {
	  for (int j = 0; j < _Nbins; ++j) {
	    int ii = i;
	    int jj = j;
	    this -> GetIndices(ii, jj);
	    if (_UseMeanClass)
	      (*_Cov)[i][j] = _mproducts[ii][jj].GetMean() - _mmeans[ii].GetMean() * _mmeans[jj].GetMean();
	    else
	      (*_Cov)[i][j] = _hproducts[ii][jj] -> GetMean() - _hmeans[ii] -> GetMean() * _hmeans[jj] -> GetMean();
	  }
	}
    } else {
      	for (int i = 0; i < _Nbins; ++i) {
	  for (int j = 0; j < _Nbins; ++j) {
	    int ii = i;
	    int jj = j;
	    this -> GetIndices(ii, jj);
	    if (_UseMeanClass)
	      (*_Cov)[i][j] = _mproducts[ii][jj].GetMean();
	    else
	      (*_Cov)[i][j] = _hproducts[ii][jj] -> GetMean();
	  }
	}
    }

  }
// ________________________________________________________________
  void ToyCovariance::EnsureCorr()
  {
    if (_Corr)
      return;
    
    this -> EnsureCov();

   _Corr = new TMatrixD(_Nbins, _Nbins);
    for (int i = 0; i < _Nbins; ++i) {
      for (int j = 0; j < _Nbins; ++j) {
	(*_Corr)[i][j] = (*_Cov)[i][j] / sqrt((*_Cov)[i][i]*(*_Cov)[j][j]);
      }
    }
    

  }

// ________________________________________________________________
// ________________________________________________________________
// ________________________________________________________________
void ToyCovariance::Cleanup()
{

  for (unsigned int i = 0; i < _hmeans.size(); ++i) {
    delete _hmeans[i];
    for (unsigned int j = 0; j <= i; ++j) {
      delete _hproducts[i][j];
      delete _hcorr2d[i][j];
    }
  } 
}
