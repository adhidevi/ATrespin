void eventcounter(int slug, int run){
	TChain* mul = new TChain("mul");
	mul->Add(Form("/lustre19/expphy/volatile/halla/parity/japanOutput/prexPrompt_pass1_%d.000.root",run));
	ofstream outfile(Form("./TextFile/pattern_number_%d.txt",run));
	TCut user_cut = "asym_atl1.Device_Error_Code==0&&asym_atl2.Device_Error_Code==0&&asym_atr1.Device_Error_Code==0&&asym_atr2.Device_Error_Code==0";
	int patterns = mul->Draw("pattern_number>>htemp1","ErrorFlag==0","goff");
	int patterns_DEC = mul->Draw("pattern_number>>htemp2",user_cut+"ErrorFlag==0","goff");
	TH1F* htemp1 = (TH1F*)gDirectory->FindObject("htemp1");
	TH1F* htemp2 = (TH1F*)gDirectory->FindObject("htemp2");
	htemp1->SetDirectory(gROOT);
	htemp2->SetDirectory(gROOT);
	outfile<<slug<<"\t"<<run<<"\t"<<htemp1->GetEntries()<<"\t"<<htemp2->GetEntries()<<endl;
}
