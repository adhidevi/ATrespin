void reg_slope_grand(){
    gStyle->SetOptFit(0);
    gStyle->SetTitleYOffset(1.2);
    TChain* ch = new TChain("slope");
    TString key = "26_54";
    ch->Add(Form("./rootfiles/postpan/slugs/slugAvgslope/grand_slug%s.root",key.Data()));
    ofstream outfile("./TextFile/grand_postpan_slope_slug30_40.csv");
      outfile<<"Channel"<<"\t\t"<<"Mean"<<"\t"<<"Error"<<"\t"<<"ChiSq/NDF"<<endl;
//    TString cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<55","arm==0&&wein==2&&ihwp==2&&slug<55","arm==0&&wein==1&&ihwp==2&&slug<55","arm==0&&wein==2&&ihwp==1&&slug<55"};
    TString cuts[] = {"arm==0&&wein==1&&ihwp==1&&slug<=40&&slug>=30","arm==0&&wein==2&&ihwp==2&&slug<=40&&slug>=30","arm==0&&wein==1&&ihwp==2&&slug<=40&&slug>=30","arm==0&&wein==2&&ihwp==1&&slug<=40&&slug>=30"};
    const int ncuts = sizeof(cuts)/sizeof(*cuts);
    Int_t sign[ncuts] = {1,1,1,1};
    TString DV[] = {"atl1","atl2","atr1","atr2","usl","usr"};
    TString IV[] = {"bpm4aX","bpm4aY","bpm4eX","bpm4eY","bpm11X"};
    int nDV = sizeof(DV)/sizeof(*DV);
    int nIV = sizeof(IV)/sizeof(*IV);
    TGraphErrors* gr[ncuts];
    TF1* fit[ncuts];
    TLine* line[5];
    line[0] = new TLine(56,-30000,56,30000);
    line[1] = new TLine(59,-30000,59,30000);
    line[2] = new TLine(60,-30000,60,30000);
    line[3] = new TLine(62,-30000,62,30000);
    line[4] = new TLine(69,-30000,69,30000);
    for(int iDV=0;iDV<nDV;iDV++){
    for(int iIV=0;iIV<nIV;iIV++){
    for(int icuts=0;icuts<ncuts;icuts++){
    Int_t  npt = ch->Draw(Form("%d*%s_%s:%s_%s_err:slug:Entry$",sign[icuts],DV[iDV].Data(),IV[iIV].Data(),DV[iDV].Data(),IV[iIV].Data()),cuts[icuts].Data(),"goff");
    double* mean = new double[npt];
    double* mean_error = new double[npt];
    double* slug = new double[npt];
    double* errX = new double[npt];
    for(int ipt=0;ipt<npt;ipt++){
      mean[ipt] = *(ch->GetV1()+ipt);
      mean_error[ipt] = *(ch->GetV2()+ipt);
      slug[ipt] = *(ch->GetV3()+ipt);
      errX[ipt] = 0.0;
    }
    gr[icuts]=new TGraphErrors(npt,slug,mean,errX,mean_error);
    gr[icuts]->SetMarkerStyle(20);
    gr[icuts]->SetMarkerColor(icuts+6);
    gr[icuts]->Draw("AP");
    gr[icuts]->Fit("pol0");
    fit[icuts] = gr[icuts]->GetFunction("pol0");
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
    mg->SetTitle(Form("%s_%s/(ppm/um);%s;%s_%s_mean/(ppm/um)",DV[iDV].Data(),IV[iIV].Data(),"slug",DV[iDV].Data(),IV[iIV].Data()));
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
//    if(icuts<ncuts){
      if(icuts==1||icuts==3){
    fit[icuts]->SetLineColor(icuts+6);
    mean[icuts] = fit[icuts]->GetParameter(0);
    error[icuts] = fit[icuts]->GetParError(0);
    ChiSq[icuts] = fit[icuts]->GetChisquare();
    NDF[icuts] = fit[icuts]->GetNDF();
      }
    text[icuts] = Form("%s: %4.5f#pm%4.5f #chi^{2}/NDF: %4.1f/%d",wein_ihwp[icuts].Data(),mean[icuts],error[icuts],ChiSq[icuts],NDF[icuts]);
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
    TString average = Form("Grand: %4.5f#pm%4.5f #chi^{2}: %4.1f/%d",Gfit->GetParameter(0),Gfit->GetParError(0),Gfit->GetChisquare(),Gfit->GetNDF());
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
    outfile<<Form("%s_%s",DV[iDV].Data(),IV[iIV].Data())<<"\t"<<Gfit->GetParameter(0)<<"\t\t"<<Gfit->GetParError(0)<<"\t"<<Gfit->GetChisquare()<<"/"<<Gfit->GetNDF()<<endl;
    c1->SaveAs(Form("plots/postpan/slope/device_%d_%d.pdf",iDV,iIV));
    }
    }
    gSystem->Exec(Form("pdfunite ./plots/postpan/slope//device_0_*.pdf ./plots/postpan/slope//device_1_*.pdf ./plots/postpan/slope/device_2_*.pdf ./plots/postpan/slope/device_3_*.pdf ./plots/postpan/slope/device_4_*.pdf ./plots/postpan/slope/device_5_*.pdf ./plots/postpan/slope/grand_AT_%s.pdf","30_40"));
    gSystem->Exec(Form("rm -rf plots/postpan/slope/device_*.pdf"));
    }
