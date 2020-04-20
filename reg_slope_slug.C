void reg_slope_slug(int slug, int ihwp, int wein, int arm){
	gStyle->SetOptFit(1);
	TChain* ch = new TChain("T");
	ch->Add(Form("/lustre19/expphy/volatile/halla/parity/treeMergeOutput/prexPrompt_slug%d.root",slug));
	ifstream infile(Form("./TextFile/slugs/slug%d.txt",slug));
	string cut;
	vector<string>cuts;
	while(infile>>cut)
	cuts.push_back(cut);
	infile.close();
	string user_cut = *cuts.data();
	cout<<Form("used cut:: ''%s''",user_cut.c_str())<<endl;
	TString dv_list[] = {"atl1","atl2","atr1","atr2","usl","usr"};
	TString iv_list[] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm11X"};
	int nDV = sizeof(dv_list)/sizeof(*dv_list);
	int nIV = sizeof(iv_list)/sizeof(*iv_list);
	TFile f(Form("./rootfiles/postpan/slugs/slugAvgslope/slug%d.root",slug),"recreate");
	TTree slope("slope","slope");
	TString name;
	TBranch* b1;
	TBranch* b1err;
	double p1, p1err;
	int nlbl = ch->Draw("run:mini:Entry$","","goff");
	double* run = new double[nlbl];
	double* minirun = new double[nlbl];
	for(int ipt=0;ipt<nlbl;ipt++){
	run[ipt] = *(ch->GetV1()+ipt);
	minirun[ipt] = *(ch->GetV2()+ipt);
	}
	for(int iDV=0;iDV<nDV;iDV++){
	for(int iIV=0;iIV<nIV;iIV++){
	int npt = ch->Draw(Form("%s_%s/(ppm/um):%s_%s_err/(ppm/um):Entry$",dv_list[iDV].Data(),iv_list[iIV].Data(),dv_list[iDV].Data(),iv_list[iIV].Data()),Form("%s",user_cut.c_str()),"goff");
	double* det_bpm = new double[npt];
	double* det_bpm_err = new double[npt];
	double* xerr = new double[npt];
	for(int ipt=0;ipt<npt;ipt++){
	det_bpm[ipt] = *(ch->GetV1()+ipt);
	det_bpm_err[ipt] = *(ch->GetV2()+ipt);
	xerr[ipt] = 0.0;
	}	
	TCanvas* c1 = new TCanvas("c1","c1",1000,600);
	gPad->SetGrid();
	TGraphErrors* gr = new TGraphErrors(npt,ch->GetV3(),det_bpm,xerr,det_bpm_err);
	gr->SetMarkerStyle(20);
	gr->SetTitle(Form("%s_%s;run.minirun;slope(ppm/um)",dv_list[iDV].Data(),iv_list[iIV].Data()));
	gr->Draw("AP");
	gr->Fit("pol0");
	TF1* fit = gr->GetFunction("pol0");
	gr->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	gr->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	name = Form("%s_%s",dv_list[iDV].Data(),iv_list[iIV].Data());
	b1 = slope.Branch(name,&p1,"mean/D");
	p1 = fit->GetParameter(0);
	b1->Fill();
	b1->ResetAddress();
	b1err = slope.Branch(name+"_err",&p1err,"mean/D");
	p1err = fit->GetParError(0);
	b1err->Fill();
	b1err->ResetAddress();
	c1->SaveAs(Form("./plots/postpan/slope/device_%d_%d.pdf",iDV,iIV));
	}
	}
        TBranch* b2 = slope.Branch("slug",&slug,"slug/I");
        TBranch* b3 = slope.Branch("ihwp",&ihwp,"ihwp/I");
        TBranch* b4 = slope.Branch("wein",&wein,"wein/I");
        TBranch* b5 = slope.Branch("arm",&arm,"arm/I");
        b2->Fill();
        b3->Fill();
        b4->Fill();
        b5->Fill();
        slope.SetEntries();
        f.cd();
        slope.Write();
        f.Close();
	gSystem->Exec(Form("pdfunite ./plots/postpan/slope/device_0*.pdf ./plots/postpan/slope/device_1*.pdf ./plots/postpan/slope/device_2*.pdf ./plots/postpan/slope/device_3*.pdf ./plots/postpan/slope/device_4*.pdf ./plots/postpan/slope/device_5*.pdf ./plots/postpan/slope/slug%d.pdf",slug));
	gSystem->Exec(Form("rm -rf ./plots/postpan/slope/device*.pdf")); 
}
