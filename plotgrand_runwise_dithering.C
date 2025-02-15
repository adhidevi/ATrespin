#include "device_name.h"
void plotgrand_runwise_dithering(){
	gStyle->SetOptFit(0);
	gStyle->SetTitleYOffset(1.2);
	TChain* ch = new TChain("mini");
	ch->Add(Form("./rootfiles/dithering/slugs/grand_slug26_94.root"));
	int a_slug = 26;
	int b_slug = 54;
	ofstream outfile(Form("./TextFile/grand_dithering_runwise_slug%d_%d.csv",a_slug,b_slug));
	outfile<<"Channel"<<"\t"<<"Mean"<<"\t"<<"Error"<<"\t"<<"ChiSq/NDF"<<"\t"<<"RMS"<<endl;
	TCut cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<55","arm==0&&wein==2&&ihwp==2&&slug<55","arm==0&&wein==1&&ihwp==2&&slug<55","arm==0&&wein==2&&ihwp==1&&slug<55"};
//	TCut cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<=40&&slug>=30","arm==0&&wein==2&&ihwp==2&&slug<=40&&slug>=30","arm==0&&wein==1&&ihwp==2&&slug<=40&&slug>=30","arm==0&&wein==2&&ihwp==1&&slug<=40&&slug>=30"};
	TCut user_cut = {"asym_atl1_DEC==0&&asym_atl2_DEC==0&&asym_atr1_DEC==0&&asym_atr2_DEC==0"};
	const int ncuts = sizeof(cuts)/sizeof(*cuts);
	int sign[ncuts] = {1,1,-1,-1};
	int nDL = sizeof(device_name)/sizeof(*device_name);
	TGraphErrors* gr[ncuts];
	TF1* fit[ncuts];
	TGraph* grRMS[ncuts];
	TF1* fitRMS[ncuts];
	TString device;
	for(int iDL=0;iDL<nDL;iDL++){
	device = device_name[iDL];
	device.ReplaceAll("reg_","dit_");
	for(int icuts=0;icuts<ncuts;icuts++){
	int npt = ch->Draw(Form("%d*1e6*%s.mean:1e6*%s.err:1e6*%s.rms:run:Entry$",sign[icuts],device.Data(),device.Data(),device.Data()),user_cut+cuts[icuts],"goff");
	double* mean = new double[npt];
	double* mean_error = new double[npt];
	double* rms = new double[npt];
	double* ent = new double[npt];
	double* errX = new double[npt];
	for(int ipt=0;ipt<npt;ipt++){
	mean[ipt] = *(ch->GetV1()+ipt);
	mean_error[ipt] = *(ch->GetV2()+ipt);
	rms[ipt] = *(ch->GetV3()+ipt);
	ent[ipt] = *(ch->GetV4()+ipt);
	errX[ipt] = 0.0;
	}
	gr[icuts] = new TGraphErrors(npt,ent,mean,errX,mean_error);
	gr[icuts]->SetMarkerStyle(20);
	gr[icuts]->SetMarkerColor(icuts+6);
	gr[icuts]->Draw("AP");
	gr[icuts]->Fit("pol0");
	fit[icuts] = gr[icuts]->GetFunction("pol0");
	
	grRMS[icuts] = new TGraph(npt,ent,rms);
	grRMS[icuts]->SetMarkerStyle(20);
	grRMS[icuts]->SetMarkerColor(icuts+6);
	grRMS[icuts]->Draw("AP");
	grRMS[icuts]->Fit("pol0");
	fitRMS[icuts] = grRMS[icuts]->GetFunction("pol0");
	}
	TCanvas* c1 = new TCanvas("c1","c1",1000,600);
	c1->Divide(1,2);
	c1->cd(1);
	gPad->SetGrid();
	gPad->SetPad(0.0,0.2,1.0,1.0);
	TMultiGraph* mg = new TMultiGraph("mg","mg");
	mg->Add(gr[0]);
	mg->Add(gr[1]);
	mg->Add(gr[2]);
	mg->Add(gr[3]);
	mg->SetTitle(Form("%s_mean/ppm;run(minirun);%s_mean/ppm",device.Data(),device.Data()));
	mg->Draw("AP");
	mg->Fit("pol0");
	TF1* Gfit = mg->GetFunction("pol0");
	c1->cd(2);
	gPad->SetPad(0.0,0.0,1.0,0.2);
	TString text[ncuts];
	double mean[ncuts] = {0.0};
	double error[ncuts] = {0.0};
	double ChiSq[ncuts] = {0.0};
	int NDF[ncuts] = {0};
	TString wein_ihwp[ncuts] = {"Right IN","Left OUT","Right OUT", "Left IN"};
	for(int icuts=0;icuts<ncuts;icuts++){
	if(icuts<ncuts){
//	if(icuts==1||icuts==3){
	fit[icuts]->SetLineColor(icuts+6);
	mean[icuts] = fit[icuts]->GetParameter(0);
	error[icuts] = fit[icuts]->GetParError(0);
	ChiSq[icuts] = fit[icuts]->GetChisquare();
	NDF[icuts] = fit[icuts]->GetNDF();
	}
	text[icuts] = Form("%s: %4.3f#pm%4.3f #chi^{2}/NDF: %4.1f/%d",wein_ihwp[icuts].Data(),mean[icuts],error[icuts],ChiSq[icuts],NDF[icuts]);
	}
	TPaveLabel* pl[ncuts];
	pl[0] = new TPaveLabel(0.10,0.80,0.50,0.95,text[0].Data(),"NDC");
	pl[1] = new TPaveLabel(0.10,0.60,0.50,0.75,text[1].Data(),"NDC");
	pl[2] = new TPaveLabel(0.10,0.40,0.50,0.55,text[2].Data(),"NDC");
	pl[3] = new TPaveLabel(0.10,0.20,0.50,0.35,text[3].Data(),"NDC");
	for(int icuts=0;icuts<ncuts;icuts++){
	pl[icuts]->SetBorderSize(0);
	pl[icuts]->SetFillColor(0);
	pl[icuts]->SetTextColor(icuts+6);
	pl[icuts]->Draw();
	}
	TString average = Form("Grand: %4.3f#pm%4.3f #chi^{2}: %4.1f/%d",Gfit->GetParameter(0),Gfit->GetParError(0),Gfit->GetChisquare(),Gfit->GetNDF());
	TPaveLabel* plG = new TPaveLabel(0.45,0.50,0.80,0.65,average,"NDC");
	plG->SetBorderSize(0);
	plG->SetFillColor(0);
	plG->SetTextColor(kRed);
	plG->Draw();
	c1->SaveAs(Form("./plots/dithering/device_%d_mean.pdf",iDL));
	c1->cd(1);
	gPad->SetGrid();
	gPad->SetPad(0.0,0.2,1.0,1.0);
	TMultiGraph* mgRMS = new TMultiGraph("mgRMS","mgRMS");
	mgRMS->Add(grRMS[0]);
	mgRMS->Add(grRMS[1]);
	mgRMS->Add(grRMS[2]);
	mgRMS->Add(grRMS[3]);
	mgRMS->SetTitle(Form("%s_rms/ppm;run(minirun);%s_rms/ppm",device.Data(),device.Data()));
	mgRMS->Draw("AP");
	mgRMS->Fit("pol0");
	TF1* GfitRMS = mgRMS->GetFunction("pol0");
	c1->cd(2);
	gPad->SetPad(0.0,0.0,1.0,0.2);
	for(int icuts=0;icuts<ncuts;icuts++){
	if(icuts<ncuts){
//	if(icuts==1||icuts==3){
	fitRMS[icuts]->SetLineColor(icuts+6);
	mean[icuts] = fitRMS[icuts]->GetParameter(0);
	error[icuts] = fitRMS[icuts]->GetParError(0);
	ChiSq[icuts] = fitRMS[icuts]->GetChisquare();
	NDF[icuts] = fitRMS[icuts]->GetNDF();
	}
	text[icuts] = Form("%s: %4.3f#pm%4.3f #chi^{2}/NDF: %4.1f/%d",wein_ihwp[icuts].Data(),mean[icuts],error[icuts],ChiSq[icuts],NDF[icuts]);
	}
	pl[0] = new TPaveLabel(0.10,0.80,0.50,0.95,text[0].Data(),"NDC");
	pl[1] = new TPaveLabel(0.10,0.60,0.50,0.75,text[1].Data(),"NDC");
	pl[2] = new TPaveLabel(0.10,0.40,0.50,0.55,text[2].Data(),"NDC");
	pl[3] = new TPaveLabel(0.10,0.20,0.50,0.35,text[3].Data(),"NDC");
	for(int icuts=0;icuts<ncuts;icuts++){
	pl[icuts]->SetBorderSize(0);
	pl[icuts]->SetFillColor(0);
	pl[icuts]->SetTextColor(icuts+6);
	pl[icuts]->Draw();
	}
	average = Form("Grand: %4.3f#pm%4.3f #chi^{2}: %4.1f/%d",GfitRMS->GetParameter(0),GfitRMS->GetParError(0),GfitRMS->GetChisquare(),GfitRMS->GetNDF());
	plG = new TPaveLabel(0.45,0.50,0.80,0.65,average,"NDC");
	plG->SetBorderSize(0);
	plG->SetFillColor(0);
	plG->SetTextColor(kRed);
	plG->Draw();
	
	outfile<<device.Data()<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d \t %4.3f",Gfit->GetParameter(0),Gfit->GetParError(0),Gfit->GetChisquare(),Gfit->GetNDF(),GfitRMS->GetParameter(0))<<endl;
	c1->SaveAs(Form("./plots/dithering/device_%d_rms.pdf",iDL));
	
	int npull = ch->Draw(Form("run:minirun:ihwp:wein:Entry$"),Form("slug>=%d&&slug<=%d",a_slug,b_slug)+user_cut,"goff");
	double* run = new double[npull];
	double* minirun = new double[npull];
	double* ihwp = new double[npull];
	double* wein = new double[npull];
	for(int i=0;i<npull;i++){
	run[i] = *(ch->GetV1()+i);
	minirun[i] = *(ch->GetV2()+i);
	ihwp[i] = *(ch->GetV3()+i);
	wein[i] = *(ch->GetV4()+i);
	}
	npull = ch->Draw(Form("1e6*%s.mean:1e6*%s.err:slug:Entry$",device.Data(),device.Data()),Form("slug>=%d&&slug<=%d",a_slug,b_slug)+user_cut,"goff");
	double* point = new double[npull];
	double* sigma = new double[npull];
	double* slug = new double[npull];
	double* val = new double[npull];
	for(int i=0;i<npull;i++){
	point[i] = *(ch->GetV1()+i);
	sigma[i] = *(ch->GetV2()+i);
	slug[i] = *(ch->GetV3()+i);
	}
	TH1D* hPull = new TH1D(Form("hPull%d",iDL),"",npull,0,npull);
	TH1D* hPull1D = new TH1D(Form("hPull1D_%d",iDL),"1D pull",80,-8,8);
	for(int i=0;i<npull;i++){
	if((ihwp[i]==1&&wein[i]==1)||(ihwp[i]==2&&wein[i]==2))
	val[i] = (point[i]-Gfit->GetParameter(0))/sigma[i];
	else if((ihwp[i]==2&&wein[i]==1)||(ihwp[i]==1&&wein[i]==2))
	val[i] = (-1*point[i]-Gfit->GetParameter(0))/sigma[i];
	hPull->Fill(i,val[i]);
	hPull1D->Fill(val[i]);
	cout<<slug[i]<<"\t"<<run[i]<<"."<<minirun[i]<<"\t"<<point[i]<<"\t"<<sigma[i]<<"\t"<<Gfit->GetParameter(0)<<"\t"<<val[i]<<endl;
	}
	TCanvas* c3 = new TCanvas("c3","c3",1000,600);
	c3->Divide(2,1);
	c3->cd(1);
	gPad->SetPad(0.0,0.0,0.7,1.0);
	gPad->SetGrid();
	hPull->SetFillColor(kGreen-3);
	hPull->SetStats(0);
	hPull->SetTitle(Form("%s_pull/ppm;run(minirun);%s_pull/ppm",device.Data(),device.Data()));
	hPull->GetXaxis()->SetTitleOffset(1.55);
	hPull->Draw("B HISTO");
	for(int i=0;i<npull;i++){
//	if(i%10==9)
	if(i%20==19)
	hPull->GetXaxis()->SetBinLabel(i+1,Form("%4.0f",run[i]));
	}
	c3->cd(2);
	gPad->SetPad(0.7,0.0,1.0,1.0);
	gStyle->SetOptStat(1);
	hPull1D->Draw();
	c3->SaveAs(Form("./plots/dithering/device_%d_pull.pdf",iDL));
	}
    gSystem->Exec(Form("pdfunite ./plots/dithering/device_0_*.pdf ./plots/dithering/device_1_*.pdf ./plots/dithering/device_2_*.pdf ./plots/dithering/device_3_*.pdf ./plots/dithering/device_4_*.pdf ./plots/dithering/device_5_*.pdf ./plots/dithering/device_6_*.pdf ./plots/dithering/device_7_*.pdf ./plots/dithering/device_8_*.pdf ./plots/dithering/device_9_*.pdf ./plots/dithering/device_10_*.pdf ./plots/dithering/device_11_*.pdf ./plots/dithering/device_12_*.pdf ./plots/dithering/device_13_*.pdf ./plots/dithering/device_14_*.pdf ./plots/dithering/device_15_*.pdf ./plots/dithering/grand_AT_runwise_%d-%d.pdf",a_slug,b_slug));
    gSystem->Exec(Form("rm -rf plots/dithering/device_*.pdf"));
}
