HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 int n,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, Key, Key),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,Key, 
					    Key),
 Key k1, Key k2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				  KeyPair),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					    KeyPair),
 KeyPair kp1, KeyPair kp2,ImplementationType type);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 HfstTransducer &another,
 ImplementationType type);
