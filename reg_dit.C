void reg_dit(){
    gStyle->SetOptFit(1);
    gStyle->SetTitleYOffset(1.2);
    TChain* reg = new TChain("mini");
    TChain* dit = new TChain("mini");
    TString key = "26_94";
    reg->Add(Form("./rootfiles/overload/slugs/slugAvg/grand_slug%s.root",key.Data()));
    dit->Add(Form("./rootfiles/dithering/slugs/slugAvg/grand_slug%s.root",key.Data()));
    reg->AddFriend(dit);

    TString mean[]={"reg_asym_atl1.mean-dit_asym_atl1.mean","reg_asym_atl2.mean-dit_asym_atl2.mean","reg_asym_atr1.mean-dit_asym_atr1.mean","reg_asym_atr2.mean-dit_asym_atr2.mean"};
    TString error[]={"sqrt(pow(dit_asym_atl1.err,2)-pow(reg_asym_atl1.err,2))","sqrt(pow(dit_asym_atl2.err,2)-pow(reg_asym_atl2.err,2))","sqrt(pow(dit_asym_atr1.err,2)-pow(reg_asym_atr1.err,2))","sqrt(pow(dit_asym_atr2.err,2)-pow(reg_asym_atr2.err,2))"};
    TString title[]={"#mu_{reg_atl1} - #mu_{dit_atl1}","#mu_{reg_atl2} - #mu_{dit_atl2}","#mu_{reg_atr1} - #mu_{dit_atr1}","#mu_{reg_atr2} - #mu_{dit_atr2}"};
    int nDL = sizeof(mean)/sizeof(*mean);
    for(int iDL=0;iDL<nDL;iDL++){
    Int_t npt = reg->Draw(Form("%s:%s:slug:Entry$",mean[iDL].Data(),error[iDL].Data()),"slug<55","goff");
    TCanvas* c1 = new TCanvas("c1","c1",1000,700);
    gPad->SetGrid();
    TGraphErrors* gr = new TGraphErrors(npt,reg->GetV4(),reg->GetV1(),0,reg->GetV2());
    gr->SetMarkerStyle(20);
    gr->SetTitle(Form("%s (err = #sqrt{#sigma_{dit}^{2}-#sigma_{reg}^{2}});slugs;",title[iDL].Data()));
    gStyle->SetTitleAlign(23);
    gr->GetXaxis()->Set(npt,-0.5,npt-0.5);
    gr->Draw("AP");
    gr->Fit("pol0");
    for(int ipt=0;ipt<npt;ipt++)
    gr->GetXaxis()->SetBinLabel(ipt+1,Form("%2.0f",reg->GetV3()[ipt]));
    c1->SaveAs(Form("./plots/combine/device%d.pdf",iDL));
  }
    gSystem->Exec(Form("pdfunite ./plots/combine/device0.pdf ./plots/combine/device1.pdf ./plots/combine/device2.pdf ./plots/combine/device3.pdf ./plots/combine/at_reg_dit.pdf"));
    gSystem->Exec(Form("rm -rf ./plots/combine/device*.pdf")); 
}
