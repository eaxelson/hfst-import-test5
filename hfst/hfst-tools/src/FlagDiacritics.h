#ifndef _FLAG_DIACRITICS_H_
#define _FLAG_DIACRITICS_H_
#include <set>
#include <map>
#include <string>
#include <cassert>
#include <vector>
#ifdef DEBUG
#include <iostream>
#endif
enum DiacriticOperator
  {
    Pop, Nop, Dop, Rop, Cop, Uop
  };

typedef std::map<short,DiacriticOperator> DiacriticOperators;
typedef std::map<short,std::string> DiacriticFeatures;
typedef std::map<short,std::string> DiacriticValues;
typedef std::map<std::string,std::string> FeatureValues;
typedef std::map<std::string,bool> FeaturePolarities;
typedef std::vector<unsigned short> KeyVector;
class  FlagDiacriticTable
{
  static DiacriticOperators diacritic_operators;
  static DiacriticFeatures diacritic_features;
  static DiacriticValues diacritic_values;
  FeatureValues feature_values;
  FeaturePolarities feature_polarities;
  bool error_flag;
  static bool is_genuine_diacritic(const std::string &diacritic_string);
  static void split_diacritic(short diacritic_number,
			      const std::string &diacritic_string);
  void set_positive_value(std::string &feature,
			  std::string &value);
  void set_negative_value(std::string &feature,
			  std::string &value);
  void disallow(std::string &feature,
		std::string &value);
  void require(std::string &feature,
	       std::string &value);
  void unify(std::string &feature,
	     std::string &value);
  void clear(std::string &feature);

 public:
  FlagDiacriticTable(void);
  static void define_diacritic(short diacritic_number,
			       const std::string &diacritic_string);
  void insert_number(short key_number);
  void reset(void);
  bool fails(void);
  static bool is_diacritic(short key_number);
  bool is_valid_string(const KeyVector * input_vector);
  KeyVector * filter_diacritics(KeyVector * input_vector);
#ifdef DEBUG
  static void display(short diacritic); 
#endif
};
#endif
