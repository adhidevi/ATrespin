void MergeTree(int slug,int ihwp,int wein,int arm){
  TStopwatch tsw;
  TString runlist_path = "/u/group/halla/parity/software/japan_offline/prompt/prex-prompt/prex-runlist/good_list/";
  TString filename  = runlist_path+Form("slug%d.list",slug);
  FILE *runlist = fopen(filename.Data(),"r");
  if(runlist!=NULL){
    TFile *output = TFile::Open(Form("./rootfiles/dithering/slugs/slug%d.root",slug),"RECREATE");
    TTree* output_tree = new TTree("mini","mini");
    TList* list = new TList();
    vector<Int_t> run_idx;
    while(!feof(runlist)){
      Int_t run_number=0;
      fscanf(runlist,"%d/n",&run_number);
      if(run_number==0)
	continue;
      run_idx.push_back(run_number);
      TFile *this_file = TFile::Open(Form("./rootfiles/dithering/runs/run_%d.root",run_number));
      if(this_file!=NULL){
	TTree *this_tree = (TTree*)this_file->Get("mini");
	if(this_tree!=NULL)
	  list->Add(this_tree);
      }
    }
    fclose(runlist);
    output->cd();
    output_tree = TTree::MergeTrees(list);
    Int_t nslug, nihwp, nwein, narm;
    TBranch* branch_slug = output_tree->Branch("slug",&nslug);
    TBranch* branch_ihwp = output_tree->Branch("ihwp",&nihwp);
    TBranch* branch_wein = output_tree->Branch("wein",&nwein);
    TBranch* branch_arm = output_tree->Branch("arm",&narm);
    Int_t nevt = output_tree->GetEntries();
    for(int ievt=0;ievt<nevt;ievt++){
      output_tree->GetEntry(ievt);
      nslug=slug;
      nihwp=ihwp;
      nwein=wein;
      narm=arm;
      branch_slug->Fill();
      branch_ihwp->Fill();
      branch_wein->Fill();
      branch_arm->Fill();
    }
    output_tree->Write();
    output->Close();
  }
  tsw.Print();
}
