#include "device_list.h"
void plotslug_overload(int slug, int ihwp, int wein, int arm){
	gStyle->SetOptFit(111);
	TChain* ch = new TChain("mini");
	ch->Add(Form("./rootfiles/overload/slugs/slug%d.root",slug));
	int nDL = sizeof(device_list)/sizeof(*device_list);

	TFile f(Form("./rootfiles/overload/slugs/slugAvg/slug%d.root",slug),"recreate");
	TTree mini("mini","mini");
	typedef struct {Double_t MEAN, ERROR, RMS;} REG_asym;
	TString name;
	TBranch* b1;
	TCut user_cut = "asym_atl1_DEC==0&&asym_atl2_DEC==0&&asym_atr1_DEC==0&&asym_atr2_DEC==0";
//	TCut user_cut = "";
	int nlbl = ch->Draw("run:minirun:Entry$","","goff");
        double* run = new double[nlbl];
        double* minirun = new double[nlbl];
	for(int ipt=0;ipt<nlbl;ipt++){
	run[ipt] = *(ch->GetV1()+ipt);
	minirun[ipt] = *(ch->GetV2()+ipt);
	}
	TString device_name;
	for(int iDL=0;iDL<nDL;iDL++){
	device_name = device_list[iDL];
	device_name.ReplaceAll("-","_");
	int npt=ch->Draw(Form("1e6*%s.mean:1e6*%s.err:1e6*%s.rms:Entry$",device_name.Data(),device_name.Data(),device_name.Data()),user_cut,"goff");
	double* reg_asym_mean = new double[npt];
	double* reg_asym_err = new double[npt];
	double* reg_asym_rms = new double[npt];
	double* xerr = new double[npt];
	for(int ipt=0;ipt<npt;ipt++){
	reg_asym_mean[ipt] = *(ch->GetV1()+ipt);
	reg_asym_err[ipt] = *(ch->GetV2()+ipt);
	reg_asym_rms[ipt] = *(ch->GetV3()+ipt);
	xerr[ipt] = 0.0;
	}
	TCanvas* c1 = new TCanvas("c1","c1",1000,600);
	gPad->SetGrid();
	TGraphErrors* meanG = new TGraphErrors(npt,ch->GetV4(),reg_asym_mean,xerr,reg_asym_err);
	meanG->SetMarkerStyle(20);
	if(device_name.Contains("_reg"))
	meanG->SetTitle(Form("%s_dd_mean/ppm;run.minirun;%s_dd_mean/ppm",device_name.Data(),device_name.Data()));
	else
	meanG->SetTitle(Form("%s_mean/ppm;run.minirun;%s_mean/ppm",device_name.Data(),device_name.Data()));
	meanG->Draw("AP");
	meanG->Fit("pol0");
        TF1* fit = meanG->GetFunction("pol0");
	meanG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	meanG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c1->SaveAs(Form("./plots/overload/device_%d_mean.pdf",iDL));

	TCanvas* c2 = new TCanvas("c2","c2",1000,600);
	gPad->SetGrid();
	TGraph* rmsG = new TGraph(npt,ch->GetV4(),reg_asym_rms);
	rmsG->SetMarkerStyle(20);
	if(device_name.Contains("_reg"))
	rmsG->SetTitle(Form("%s_dd_rms/ppm;run.minirun;%s_dd_rms/ppm",device_name.Data(),device_name.Data()));
	else
	rmsG->SetTitle(Form("%s_rms/ppm;run.minirun;%s_rms/ppm",device_name.Data(),device_name.Data()));
	rmsG->Draw("AP");
	rmsG->Fit("pol0");
        TF1* fit_rms = rmsG->GetFunction("pol0");
	rmsG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	rmsG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c2->SaveAs(Form("./plots/overload/device_%d_rms.pdf",iDL));
	if(device_name.Contains("_reg"))
	name = Form("%s_dd",device_name.Data());
	else
	name = Form("%s",device_name.Data());
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
        gSystem->Exec(Form("pdfunite ./plots/overload/device_0_*.pdf ./plots/overload/device_1_*.pdf ./plots/overload/device_2_*.pdf ./plots/overload/device_3_*.pdf ./plots/overload/device_4_*.pdf ./plots/overload/device_5_*.pdf ./plots/overload/device_6_*.pdf ./plots/overload/device_7_*.pdf ./plots/overload/device_8_*.pdf ./plots/overload/device_9_*.pdf ./plots/overload/device_10_*.pdf ./plots/overload/device_11_*.pdf ./plots/overload/device_12_*.pdf ./plots/overload/device_13_*.pdf ./plots/overload/device_14_*.pdf ./plots/overload/device_15_*.pdf ./plots/overload/at_slug%d.pdf",slug));
	gSystem->Exec(Form("rm -rf ./plots/overload/device_*.pdf"));
}
