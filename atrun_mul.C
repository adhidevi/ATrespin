void atrun_mul(int slug){
	TChain* ch = new TChain("mini");
	ch->Add(Form("./rootfiles/postpan/slugs/slug%d.root",slug));
	TCut user_cut = "asym_atl1_DEC!=0||asym_atl2_DEC!=0||asym_atr1_DEC!=0||asym_atr2_DEC!=0";

	ch->Scan("run:minirun:Entry$",user_cut);
	}
	
