HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *),
 fsm * (*foma_funct)(fsm *), 
 ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,int),
 fsm * (*foma_funct)(fsm *,int),
 int n,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, StringSymbol, StringSymbol),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,StringSymbol, 
					    StringSymbol),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  StringSymbol,StringSymbol),
 fsm * (*foma_funct)(fsm *, StringSymbol, StringSymbol),
 StringSymbol k1, StringSymbol k2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				  KeyPair),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					    KeyPair),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  KeyPair, KeyPair),
 fsm * (*foma_funct)(fsm *, KeyPair, 
				  KeyPair),
 KeyPair kp1, KeyPair kp2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  hfst::implementations::LogFst *),
 fsm * (*foma_funct)(fsm *,
		     fsm *),
 HfstTransducer &another,
 ImplementationType type);
