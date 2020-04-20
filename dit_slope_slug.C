void dit_slope_slug(int slug, int ihwp, int wein, int arm){
	TChain* ch = new TChain("dit");
	ch->Add(Form("/lustre19/expphy/volatile/halla/parity/ditherOutput/rootfiles_alldet_pass1/dit_alldet_slopes_slug%d.root",slug));
	TString dv_list[] = {"usl","usr","atl1","atl2","atr1","atr2"};
	TString iv_list[] = {"4aX","4aY","4eX","4eY","11X12X"};
	int nDV = sizeof(dv_list)/sizeof(*dv_list);
	int nIV = sizeof(iv_list)/sizeof(*iv_list);
	TFile f(Form("./rootfiles/dithering/slugs/slugAvgslope/slug%d.root",slug),"recreate");
	TTree slope("slope","slope");
	TString name;
	TBranch* b1;
	double p1;
	for(int iDV=0;iDV<nDV;iDV++){
	for(int iIV=0;iIV<nIV;iIV++){
	ch->Draw(Form("%s_%s:cyclenum>>h_%d_%d",dv_list[iDV].Data(),iv_list[iIV].Data(),iDV,iIV),"","goff");
	TH2D* h = (TH2D*)gDirectory->FindObject(Form("h_%d_%d",iDV,iIV));
	TCanvas* c1 = new TCanvas("c1","c1",1000,600);
	h->SetMarkerStyle(20);
	h->SetTitle(Form("%s_%s;cyclenum;slope(ppm/um)",dv_list[iDV].Data(),iv_list[iIV].Data()));
	h->Draw();
	h->Fit("pol0");
	TF1* fit = h->GetFunction("pol0");
	name = Form("%s_%s",dv_list[iDV].Data(),iv_list[iIV].Data());
	b1 = slope.Branch(name,&p1,"mean/D");
	p1 = fit->GetParameter(0);
	b1->Fill();
	b1->ResetAddress();
//	cout<<Form("%s_%s:",dv_list[iDV].Data(),iv_list[iIV].Data())<<"\t"<<p1<<endl;
	c1->SaveAs(Form("./plots/dithering/slope/device_%d_%d.pdf",iDV,iIV));
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
	gSystem->Exec(Form("pdfunite ./plots/dithering/slope/device_0_0.pdf ./plots/dithering/slope/device_0_1.pdf ./plots/dithering/slope/device_0_2.pdf ./plots/dithering/slope/device_0_3.pdf ./plots/dithering/slope/device_0_4.pdf ./plots/dithering/slope/device_1_0.pdf ./plots/dithering/slope/device_1_1.pdf ./plots/dithering/slope/device_1_2.pdf ./plots/dithering/slope/device_1_3.pdf ./plots/dithering/slope/device_1_4.pdf ./plots/dithering/slope/device_2_0.pdf ./plots/dithering/slope/device_2_1.pdf ./plots/dithering/slope/device_2_2.pdf ./plots/dithering/slope/device_2_3.pdf ./plots/dithering/slope/device_2_4.pdf ./plots/dithering/slope/device_3_0.pdf ./plots/dithering/slope/device_3_1.pdf ./plots/dithering/slope/device_3_2.pdf ./plots/dithering/slope/device_3_3.pdf ./plots/dithering/slope/device_3_4.pdf ./plots/dithering/slope/device_4_0.pdf ./plots/dithering/slope/device_4_1.pdf ./plots/dithering/slope/device_4_2.pdf ./plots/dithering/slope/device_4_3.pdf ./plots/dithering/slope/device_4_4.pdf ./plots/dithering/slope/device_5_0.pdf ./plots/dithering/slope/device_5_1.pdf ./plots/dithering/slope/device_5_2.pdf ./plots/dithering/slope/device_5_3.pdf ./plots/dithering/slope/device_5_4.pdf ./plots/dithering/slope/slug%d.pdf",slug));
	gSystem->Exec(Form("rm -rf ./plots/dithering/slope/device*.pdf")); 
}
