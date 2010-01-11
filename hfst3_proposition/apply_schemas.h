HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 HFST_IMPLEMENTATIONS::LogFst * (*log_ofst_funct)(HFST_IMPLEMENTATIONS::LogFst *),
 ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 HFST_IMPLEMENTATIONS::LogFst * (*log_ofst_funct)(HFST_IMPLEMENTATIONS::LogFst *,int),
 int n,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, Key, Key),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,Key, 
					    Key),
 HFST_IMPLEMENTATIONS::LogFst * (*log_ofst_funct)(HFST_IMPLEMENTATIONS::LogFst *,
						  Key,Key),
 Key k1, Key k2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				  KeyPair),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					    KeyPair),
 HFST_IMPLEMENTATIONS::LogFst * (*log_ofst_funct)(HFST_IMPLEMENTATIONS::LogFst *,
						  KeyPair, KeyPair),
 KeyPair kp1, KeyPair kp2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 HFST_IMPLEMENTATIONS::LogFst * (*log_ofst_funct)(HFST_IMPLEMENTATIONS::LogFst *,
						  HFST_IMPLEMENTATIONS::LogFst *),
 HfstTransducer &another,
 ImplementationType type);
