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

/** @file apply_schemas.h
    \brief declarations for HFST functions that take two or more parameters */

HfstTransducer &apply(
#if HAVE_SFST
 SFST::Transducer * (*sfst_funct)(SFST::Transducer *),
#endif
#if HAVE_OPENFST
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *),
#endif
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *),
#endif

#if HAVE_FOO
 FooTransducer * (*foo_funct)(FooTransducer *),
#endif

 bool foo /* makes sure there is always a parameter after the function pointer parameters,
	   * so commas between parameters are easier to handle */
);  

HfstTransducer &apply(
#if HAVE_SFST
 SFST::Transducer * (*sfst_funct)(SFST::Transducer *,int),
#endif
#if HAVE_OPENFST
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,int),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,int),
#endif
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *,int),
#endif
 int n);

HfstTransducer &apply(
#if HAVE_SFST
 SFST::Transducer * (*sfst_funct)(SFST::Transducer *, String, String),
#endif
#if HAVE_OPENFST
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,String, 
					    String),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  String,String),
#endif
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *, String, String),
#endif
 String k1, String k2);

HfstTransducer &apply(
#if HAVE_SFST
 SFST::Transducer * (*sfst_funct)(SFST::Transducer *,
				  SFST::Transducer *),
#endif
#if HAVE_OPENFST
 fst::StdVectorFst * (*tropical_ofst_funct)(fst::StdVectorFst *,
					    fst::StdVectorFst *),
 hfst::implementations::LogFst * (*log_ofst_funct)(hfst::implementations::LogFst *,
						  hfst::implementations::LogFst *),
#endif
#if HAVE_FOMA
 fsm * (*foma_funct)(fsm *,
		     fsm *),
#endif
 HfstTransducer &another );
