#include "device_list.h"
void atrun_dit(int run){
	TChain* mulc_dit = new TChain("mulc_dit");
	TChain* mulc_dit_combo = new TChain("mulc_dit_combo");
	TChain* mul = new TChain("mul");
	TChain* mini = new TChain("mini");
	TChain* reg = new TChain("reg");
	TString key = "overload";
	mulc_dit->Add(Form("/lustre19/expphy/volatile/halla/parity/japanOutput/prexPrompt_pass1_%d.000.root",run));
	mulc_dit_combo->Add(Form("/lustre19/expphy/volatile/halla/parity/japanOutput/prexPrompt_pass1_%d.000.root",run));
	mul->Add(Form("/lustre19/expphy/volatile/halla/parity/japanOutput/prexPrompt_pass1_%d.000.root",run));
	mini->Add(Form("/lustre19/expphy/volatile/halla/parity/postpan_respin/prexPrompt_%d_000_regress_%s.root",run,key.Data()));
	reg->Add(Form("/lustre19/expphy/volatile/halla/parity/postpan_respin/prexPrompt_%d_000_regress_%s.root",run,key.Data()));
	mul->AddFriend(mulc_dit);
	mul->AddFriend(mulc_dit_combo);
	mul->AddFriend(reg);
	int nmini = mini->Draw("minirun:Entry$","","goff");
        int nDL = sizeof(device_list)/sizeof(*device_list);
	
	TFile f(Form("./rootfiles/dithering/runs/run_%d.root",run),"recreate");
	TTree regg("mini","mini");
	typedef struct {Double_t MEAN, ERROR, RMS, DEC;} REG_asym;
	TString name;
	TBranch* b1;
	Int_t minirun;
	TString device_name;
	for(int iDL=0;iDL<nDL;iDL++){
	device_name = device_list[iDL];
	device_name.ReplaceAll("reg_","dit_");
	if(device_name.Contains("-dit"))
	name = Form("%s_dd",device_name.Data());
	else
	name = Form("%s",device_name.Data());
	name.ReplaceAll("-","_");
	REG_asym Val;
	b1 = regg.Branch(name,&Val,"mean/D:err/D:rms/D");
	for(int imini=0;imini<nmini;imini++){
	mul->Draw(Form("%s>>htemp_%d_%d",device_name.Data(),iDL,imini),Form("ErrorFlag==0&&minirun==%d",imini),"goff");
	TH1D* htemp=(TH1D*)gDirectory->FindObject(Form("htemp_%d_%d",iDL,imini));
	htemp->SetDirectory(gROOT);
	Double_t mean = htemp->GetMean();
	Double_t err = htemp->GetMeanError();
	Double_t rms = htemp->GetRMS();
	Val.MEAN = mean;
	Val.ERROR = err;
	Val.RMS = rms;
	b1->Fill();
}
	b1->ResetAddress();
}
	TBranch* b2 = regg.Branch("minirun",&minirun,"minirun/I");
	TBranch* b3 = regg.Branch("run",&run,"run/I");
for(int imini=0;imini<nmini;imini++){
	minirun = imini;
	b2->Fill();
	b3->Fill();
}
	Double_t dec;
	TBranch* DEC;
	TString device_err;
	for(int iDL=0;iDL<4;iDL++){
	device_err = device_list[iDL];
	device_err.ReplaceAll("reg_asym_","asym_");
	DEC = regg.Branch(Form("%s_DEC",device_err.Data()),&dec,Form("%s_DEC/D",device_err.Data()));
	for(int imini=0;imini<nmini;imini++){
	mul->Draw(Form("%s.Device_Error_Code>>htemp1_%d_%d",device_err.Data(),iDL,imini),Form("ErrorFlag==0&&%s.Device_Error_Code!=0&&minirun==%d",device_err.Data(),imini),"goff");
	TH1D* htemp1=(TH1D*)gDirectory->FindObject(Form("htemp1_%d_%d",iDL,imini));
	htemp1->SetDirectory(gROOT);
	dec = htemp1->GetMean();
	DEC->Fill();
}
	DEC->ResetAddress();
}
regg.SetEntries();
f.Write();
}
