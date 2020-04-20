#include "device_name.h"
void plotgrand_overload(){
    gStyle->SetOptFit(0);
    gStyle->SetTitleYOffset(1.2);
    TChain* ch = new TChain("mini");
    TString key = "26_94";
    ch->Add(Form("./rootfiles/overload/slugs/slugAvg/grand_slug%s.root",key.Data()));
    ofstream outfile("./TextFile/grand_overload_slug26_75.csv");
//    outfile<<"\t"<<"Right IN"<<"\t\t"<<"Left OUT"<<"\t\t"<<"Right OUT"<<"\t\t"<<"Left IN"<<"\t\t"<<"Average"<<endl;
//    outfile<<"Channel"<<"\t"<<"Mean"<<"\t"<<"Err"<<"\t"<<"ChiSq/NDF"<<"\t"<<"Mean"<<"\t"<<"Err"<<"\t"<<"ChiSq/NDF"<<"\t"<<"Mean"<<"\t"<<"Err"<<"\t"<<"ChiSq/NDF"<<"\t"<<"Mean"<<"\t"<<"Err"<<"\t"<<"ChiSq/NDF"<<"\t"<<"Mean"<<"\t"<<"Err"<<"\t"<<"ChiSq/NDF"<<endl;
      outfile<<"Channel"<<"\t"<<"Mean"<<"\t"<<"Error"<<"\t"<<"ChiSq/NDF"<<"\t"<<"RMS"<<endl;
    TString cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<76&&slug!=43&&slug!=74","arm==0&&wein==2&&ihwp==2&&slug<76&&slug!=43&&slug!=74","arm==0&&wein==1&&ihwp==2&&slug<76&&slug!=43&&slug!=74","arm==0&&wein==2&&ihwp==1&&slug<76&&slug!=43&&slug!=74"};
//    TString cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<55&&slug!=43&&slug!=74","arm==0&&wein==2&&ihwp==2&&slug<55&&slug!=43&&slug!=74","arm==0&&wein==1&&ihwp==2&&slug<55&&slug!=43&&slug!=74","arm==0&&wein==2&&ihwp==1&&slug<55&&slug!=43&&slug!=74"};
    const int ncuts = sizeof(cuts)/sizeof(*cuts);
    Int_t sign[ncuts] = {1,1,-1,-1};

    int nDL = sizeof(device_name)/sizeof(*device_name);
    TGraphErrors* gr[ncuts];
    TF1* fit[ncuts];
    TGraph* grRMS[ncuts];
    TF1* fitRMS[ncuts];
    TLine* line[5];
    line[0] = new TLine(56,-30000,56,30000);
    line[1] = new TLine(59,-30000,59,30000);
    line[2] = new TLine(60,-30000,60,30000);
    line[3] = new TLine(62,-30000,62,30000);
    line[4] = new TLine(69,-30000,69,30000);
    for(int iDL=0;iDL<nDL;iDL++){
    for(int icuts=0;icuts<ncuts;icuts++){
   Int_t  npt = ch->Draw(Form("%d*%s.mean:%s.err:%s.rms:slug:Entry$",sign[icuts],device_name[iDL].Data(),device_name[iDL].Data(),device_name[iDL].Data()),cuts[icuts].Data(),"goff");
    double* mean = new double[npt];
    double* mean_error = new double[npt];
    double* rms = new double[npt];
    double* slug = new double[npt];
    double* errX = new double[npt];
    for(int ipt=0;ipt<npt;ipt++){
      mean[ipt] = *(ch->GetV1()+ipt);
      mean_error[ipt] = *(ch->GetV2()+ipt);
      rms[ipt] = *(ch->GetV3()+ipt);
      slug[ipt] = *(ch->GetV4()+ipt);
      errX[ipt] = 0.0;
    }
    gr[icuts]=new TGraphErrors(npt,slug,mean,errX,mean_error);
    gr[icuts]->SetMarkerStyle(20);
    gr[icuts]->SetMarkerColor(icuts+6);
    gr[icuts]->Draw("AP");
    gr[icuts]->Fit("pol0");
    fit[icuts] = gr[icuts]->GetFunction("pol0");

    grRMS[icuts]=new TGraph(npt,slug,rms);
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
    mg->SetTitle(Form("%s_mean/ppm;%s;%s_mean/ppm",device_name[iDL].Data(),"slug",device_name[iDL].Data()));
    mg->Draw("AP");
    mg->Fit("pol0");
    TF1* Gfit = mg->GetFunction("pol0");
    for(int i=0;i<5;i++){
      line[i]->SetLineColor(40+i);
      line[i]->SetLineWidth(1);
//      line[i]->Draw();
    }
    c1->cd(2);
    gPad->SetPad(0.0,0.0,1.0,0.2);
    TString text[ncuts];
    double mean[ncuts] = {0.0};
    double error[ncuts] = {0.0};
    double ChiSq[ncuts] = {0.0};
    int NDF[ncuts] = {0};
    TString wein_ihwp[ncuts] = {"Right IN","Left OUT","Right OUT","Left IN"};
    for(int icuts=0;icuts<ncuts;icuts++){
      if(icuts<ncuts){
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
    TText* textA[5];
    textA[0] = new TText(0.80,0.75,"ATLin Moved");
    textA[1] = new TText(0.80,0.60,"ATL's Moved");
    textA[2] = new TText(0.80,0.45,"All ATs Moved");
    textA[3] = new TText(0.80,0.30,"All ATs Moved");
    textA[4] = new TText(0.80,0.15,"All ATs Moved");
    for(int i=0;i<5;i++){
      textA[i]->SetTextColor(40+i);
      textA[i]->SetTextSize(.2);
//      textA[i]->Draw();
    }
//    outfile<<device_name[iDL].Data()<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d",mean[0],error[0],ChiSq[0],NDF[0])<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d",mean[1],error[1],ChiSq[1],NDF[1])<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d",mean[2],error[2],ChiSq[2],NDF[2])<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d",mean[3],error[3],ChiSq[3],NDF[3])<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d",Gfit->GetParameter(0),Gfit->GetParError(0),Gfit->GetChisquare(),Gfit->GetNDF())<<endl;

    c1->SaveAs(Form("plots/overload/device_%d_mean.pdf",iDL));
    c1->cd(1);
    gPad->SetGrid();
    gPad->SetPad(0.0,0.2,1.0,1.0);
    TMultiGraph* mgRMS = new TMultiGraph("mgRMS","mgRMS");
    mgRMS->Add(grRMS[0]);
    mgRMS->Add(grRMS[1]);
    mgRMS->Add(grRMS[2]);
    mgRMS->Add(grRMS[3]);
    mgRMS->SetTitle(Form("%s_rms/ppm;%s;%s_rms/ppm",device_name[iDL].Data(),"slug",device_name[iDL].Data()));
    mgRMS->Draw("AP");
    mgRMS->Fit("pol0");
    TF1* GfitRMS = mgRMS->GetFunction("pol0");
    for(int i=0;i<5;i++){
 //     line[i]->Draw();
    }
    c1->cd(2);
    gPad->SetPad(0.0,0.0,1.0,0.2);
    for(int icuts=0;icuts<ncuts;icuts++){
      if(icuts<ncuts){
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
    for(int i=0;i<5;i++){
//      textA[i]->Draw();
    }
    outfile<<device_name[iDL].Data()<<"\t"<<Form("%4.3f \t %4.3f \t %4.1f/%d \t %4.3f",Gfit->GetParameter(0),Gfit->GetParError(0),Gfit->GetChisquare(),Gfit->GetNDF(),GfitRMS->GetParameter(0))<<endl;
    c1->SaveAs(Form("plots/overload/device_%d_rms.pdf",iDL));
    }
    gSystem->Exec(Form("pdfunite ./plots/overload/device_0_*.pdf ./plots/overload/device_1_*.pdf ./plots/overload/device_2_*.pdf ./plots/overload/device_3_*.pdf ./plots/overload/device_4_*.pdf ./plots/overload/device_5_*.pdf ./plots/overload/device_6_*.pdf ./plots/overload/device_7_*.pdf ./plots/overload/device_8_*.pdf ./plots/overload/device_9_*.pdf ./plots/overload/device_10_*.pdf ./plots/overload/device_11_*.pdf ./plots/overload/device_12_*.pdf ./plots/overload/device_13_*.pdf ./plots/overload/device_14_*.pdf ./plots/overload/device_15_*.pdf ./plots/overload/grand_AT_%s.pdf","26-75"));
    gSystem->Exec(Form("rm -rf plots/overload/device_*.pdf"));
    }
