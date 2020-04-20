void reg_bpm_diff(int slug, int ihwp, int wein, int arm){
	gStyle->SetOptFit(111);
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
	TString device_name[] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm11X"};
	int nDL = sizeof(device_name)/sizeof(*device_name);

	TFile f(Form("./rootfiles/postpan/slugs/slugAvgBPM/slug%d.root",slug),"recreate");
	TTree mini("mini","mini");
	typedef struct {Double_t MEAN, ERROR, RMS;} REG_asym;
	TString name;
	TBranch* b1;
	int nlbl = ch->Draw("run:mini:Entry$","","goff");
        double* run = new double[nlbl];
        double* minirun = new double[nlbl];
	for(int ipt=0;ipt<nlbl;ipt++){
	run[ipt] = *(ch->GetV1()+ipt);
	minirun[ipt] = *(ch->GetV2()+ipt);
	}
	TString device;
	for(int iDL=0;iDL<nDL;iDL++){
	device = device_name[iDL];
	int npt=ch->Draw(Form("diff_%s.mean/um:diff_%s.err/um:diff_%s.rms/um:Entry$",device.Data(),device.Data(),device.Data()),Form("%s",user_cut.c_str()),"goff");
	double* diff_mean = new double[npt];
	double* diff_err = new double[npt];
	double* diff_rms = new double[npt];
	double* xerr = new double[npt];
	for(int ipt=0;ipt<npt;ipt++){
	diff_mean[ipt] = *(ch->GetV1()+ipt);
	diff_err[ipt] = *(ch->GetV2()+ipt);
	diff_rms[ipt] = *(ch->GetV3()+ipt);
	xerr[ipt] = 0.0;
	}
	TCanvas* c1 = new TCanvas("c1","c1",1000,600);
	gPad->SetGrid();
	TGraphErrors* meanG = new TGraphErrors(npt,ch->GetV4(),diff_mean,xerr,diff_err);
	meanG->SetMarkerStyle(20);
	meanG->SetTitle(Form("%s_mean/um;run.minirun;%s_mean/um",device.Data(),device.Data()));
	meanG->Draw("AP");
	meanG->Fit("pol0");
	meanG->GetXaxis()->SetTitleOffset(1.55);
        TF1* fit = meanG->GetFunction("pol0");
	meanG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	meanG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c1->SaveAs(Form("./plots/postpan/diffBPM/device_%d_mean.pdf",iDL));
	
	TCanvas* c2 = new TCanvas("c2","c2",1000,600);
	gPad->SetGrid();
	TGraph* rmsG = new TGraph(npt,ch->GetV4(),diff_rms);
	rmsG->SetMarkerStyle(20);
	rmsG->SetTitle(Form("%s_rms/um;run.minirun;%s_rms/um",device.Data(),device.Data()));
	rmsG->Draw("AP");
	rmsG->Fit("pol0");
	rmsG->GetXaxis()->SetTitleOffset(1.55);
        TF1* fit_rms = rmsG->GetFunction("pol0");
	rmsG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	rmsG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c2->SaveAs(Form("./plots/postpan/diffBPM/device_%d_rms.pdf",iDL));

	name = Form("%s",device.Data());
	REG_asym Val;
	b1 = mini.Branch(name,&Val,"mean/D:err/D:rms/D");
	Val.MEAN = fit->GetParameter(0);
	Val.ERROR = fit->GetParError(0);
	Val.RMS = fit_rms->GetParameter(0);
	b1->Fill();
	b1->ResetAddress();
	}

	TBranch* b2 = mini.Branch("slug",&slug,"slug/I");
	TBranch* b3 = mini.Branch("ihwp",&ihwp,"ihwp/I");
	TBranch* b4 = mini.Branch("wein",&wein,"wein/I");
	TBranch* b5 = mini.Branch("arm",&arm,"arm/I");
	b2->Fill();
	b3->Fill();
	b4->Fill();
	b5->Fill();
	mini.SetEntries();
	f.cd();
	mini.Write();
	f.Close();
        gSystem->Exec(Form("pdfunite ./plots/postpan/diffBPM/device_0_*.pdf ./plots/postpan/diffBPM/device_1_*.pdf ./plots/postpan/diffBPM/device_2_*.pdf ./plots/postpan/diffBPM/device_3_*.pdf ./plots/postpan/diffBPM/device_4_*.pdf ./plots/postpan/diffBPM/at_slug%d.pdf",slug));
	gSystem->Exec(Form("rm -rf ./plots/postpan/diffBPM/device_*.pdf"));
}
