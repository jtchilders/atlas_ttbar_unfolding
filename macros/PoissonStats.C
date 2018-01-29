

//void temp(void)
{

   const int maxEntries = 10000;
   
   TH1D hmean("hmean","mean",maxEntries,0,maxEntries);
   TH1D hrms ("hrms", "rms", maxEntries,0,maxEntries);
   
   const double mean = 10.;
   
   for(int i=1;i<=maxEntries;i++){
      
      TH1D* temp = new TH1D("temp","",100,0,100);
      
      TRandom r;
      
      for(int j=0;j<i;j++){
         temp->Fill(r.Poisson(mean));
      }
      
      hmean.SetBinContent(i,temp->GetMean());
      hrms.SetBinContent(i,temp->GetRMS());
      
      delete temp;
      temp = 0;
   }
   
   TCanvas can("can","can",0,0,800,600);
   
   can.Divide(1,2);
   
   can.cd(1);
   hmean.Draw();
   can.cd(2);
   hrms.Draw();
   
   
}
