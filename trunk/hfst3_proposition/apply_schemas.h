HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *),
 ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,int),
 int n,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, Key, Key),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,Key, 
					    Key),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  Key,Key),
 Key k1, Key k2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				  KeyPair),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					    KeyPair),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  KeyPair, KeyPair),
 KeyPair kp1, KeyPair kp2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  hfst::implementations::LogFst *),
 HfstTransducer &another,
 ImplementationType type);
