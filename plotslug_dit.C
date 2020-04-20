#include "device_name.h"
void plotslug_dit(int slug, int ihwp, int wein, int arm){
	gStyle->SetOptFit(111);
	TChain* ch = new TChain("mini");
	ch->Add(Form("./rootfiles/dithering/slugs/slug%d.root",slug));
	int nDL = sizeof(device_name)/sizeof(*device_name);

	TFile f(Form("./rootfiles/dithering/slugs/slugAvg/slug%d.root",slug),"recreate");
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
	TString device;
	for(int iDL=0;iDL<nDL;iDL++){
	device = device_name[iDL];
	device.ReplaceAll("reg_","dit_");
	int npt=ch->Draw(Form("1e6*%s.mean:1e6*%s.err:1e6*%s.rms:Entry$",device.Data(),device.Data(),device.Data()),user_cut,"goff");
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
	meanG->SetTitle(Form("%s_mean/ppm;run.minirun;%s_mean/ppm",device.Data(),device.Data()));
	meanG->Draw("AP");
	meanG->Fit("pol0");
	meanG->GetXaxis()->SetTitleOffset(1.55);
        TF1* fit = meanG->GetFunction("pol0");
	meanG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	meanG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c1->SaveAs(Form("./plots/dithering/device_%d_mean.pdf",iDL));

	TCanvas* c2 = new TCanvas("c2","c2",1000,600);
	gPad->SetGrid();
	TGraph* rmsG = new TGraph(npt,ch->GetV4(),reg_asym_rms);
	rmsG->SetMarkerStyle(20);
	rmsG->SetTitle(Form("%s_rms/ppm;run.minirun;%s_rms/ppm",device.Data(),device.Data()));
	rmsG->Draw("AP");
	rmsG->Fit("pol0");
	rmsG->GetXaxis()->SetTitleOffset(1.55);
        TF1* fit_rms = rmsG->GetFunction("pol0");
	rmsG->GetXaxis()->Set(nlbl,-0.5,nlbl-0.5);
	for(int ipt=0;ipt<nlbl;ipt++)
	rmsG->GetXaxis()->SetBinLabel(ipt+1,Form("%4.0f.%2.0f",run[ipt],minirun[ipt]));
	c2->SaveAs(Form("./plots/dithering/device_%d_rms.pdf",iDL));
	name = Form("%s",device.Data());
	REG_asym Val;
	b1 = mini.Branch(name,&Val,"mean/D:err/D:rms/D");
	Val.MEAN = fit->GetParameter(0);
	Val.ERROR = fit->GetParError(0);
	Val.RMS = fit_rms->GetParameter(0);
	b1->Fill();
	b1->ResetAddress();

        int npull = ch->Draw("run:minirun:Entry$",user_cut,"goff");
        double* runlbl = new double[npull];
        double* minilbl = new double[npull];
        for(int i=0;i<npull;i++){
        runlbl[i] = *(ch->GetV1()+i);
        minilbl[i] = *(ch->GetV2()+i);
        }
        TH1D* hPull = new TH1D(Form("hPull%d",iDL),"",npull,0,npull);
        TH1D* hPull1D = new TH1D(Form("hPull1D_%d",iDL),"1D pull",40,-8,8);
        double* val = new double[npt];
        for(int i=0;i<npull;i++){
        val[i] = (reg_asym_mean[i]-fit->GetParameter(0))/reg_asym_err[i];
        hPull->Fill(i,val[i]);
        hPull1D->Fill(val[i]);
        }
        TCanvas* c3 = new TCanvas("c3","c3",1000,600);
        c3->Divide(2,1);
        c3->cd(1);
        gPad->SetPad(0.0,0.0,0.7,1.0);
        hPull->SetFillColor(kGreen-3);
        hPull->SetStats(0);
        gPad->SetGrid();
        hPull->SetTitle(Form("%s_pull/ppm;run.minirun;%s_pull/ppm",device.Data(),device.Data()));
        hPull->GetXaxis()->SetTitleOffset(1.55);
        hPull->Draw("B HISTO");
        for(int i=0;i<npull;i++)
        hPull->GetXaxis()->SetBinLabel(i+1,Form("%4.0f.%2.0f",runlbl[i],minilbl[i]));
        c3->cd(2);
        gPad->SetPad(0.7,0.0,1.0,1.0);
        gStyle->SetOptStat(1);
        hPull1D->Draw();
        c3->SaveAs(Form("./plots/dithering/device_%d_pull.pdf",iDL));
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
        gSystem->Exec(Form("pdfunite ./plots/dithering/device_0_*.pdf ./plots/dithering/device_1_*.pdf ./plots/dithering/device_2_*.pdf ./plots/dithering/device_3_*.pdf ./plots/dithering/device_4_*.pdf ./plots/dithering/device_5_*.pdf ./plots/dithering/device_6_*.pdf ./plots/dithering/device_7_*.pdf ./plots/dithering/device_8_*.pdf ./plots/dithering/device_9_*.pdf ./plots/dithering/device_10_*.pdf ./plots/dithering/device_11_*.pdf ./plots/dithering/device_12_*.pdf ./plots/dithering/device_13_*.pdf ./plots/dithering/device_14_*.pdf ./plots/dithering/device_15_*.pdf ./plots/dithering/at_slug%d.pdf",slug));
	gSystem->Exec(Form("rm -rf ./plots/dithering/device_*.pdf"));
}
