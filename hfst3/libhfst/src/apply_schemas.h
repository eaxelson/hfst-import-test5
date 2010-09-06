//       This program is free software: you can redistribute it and/or modify
//       it under the terms of the GNU General Public License as published by
//       the Free Software Foundation, version 3 of the License.
//
//       This program is distributed in the hope that it will be useful,
//       but WITHOUT ANY WARRANTY; without even the implied warranty of
//       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//       GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License
//       along with this program.  If not, see <http://www.gnu.org/licenses/>.

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *)
#if HAVE_FOMA
 ,
 fsm * (*foma_funct)(fsm *)
#endif
 );

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,int),
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *,int),
#endif
 int n);

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, String, String),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,String, 
					    String),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  String,String),
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *, String, String),
#endif
 String k1, String k2);

/*HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *, KeyPair, 
				  KeyPair),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,KeyPair, 
					    KeyPair),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  KeyPair, KeyPair),
 fsm * (*foma_funct)(fsm *, KeyPair, 
				  KeyPair),
				  KeyPair kp1, KeyPair kp2,ImplementationType type);*/

HfstTransducer &apply
(SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  hfst::implementations::LogFst *),
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *,
		     fsm *),
#endif
 HfstTransducer &another );
