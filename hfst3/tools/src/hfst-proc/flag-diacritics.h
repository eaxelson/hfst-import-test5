#ifndef _FLAG_DIACRITICS_H_
#define _FLAG_DIACRITICS_H_

#include "hfst-proc.h"

enum FdOperator {Pop, Nop, Rop, Dop, Cop, Uop};

typedef unsigned short FdFeature;
typedef short FdValue;

class FdOperation
{
 private:
  FdOperator op;
  FdFeature feature;
  FdValue value;
  std::string name;
 public:
  FdOperation(FdOperator op, FdFeature feat, FdValue val, const std::string& str):
    op(op), feature(feat), value(val), name(str) {}
  
  FdOperator Operator(void) const { return op; }
  FdFeature Feature(void) const { return feature; }
  FdValue Value(void) const { return value; }
  std::string Name(void) const { return name; }
  
  static FdOperator char_to_operator(char c)
  {
    switch (c) {
    case 'P': return Pop;
    case 'N': return Nop;
    case 'R': return Rop;
    case 'D': return Dop;
    case 'C': return Cop;
    case 'U': return Uop;
    default:
      throw;
    }
  }
  
  static bool is_diacritic(const std::string& diacritic_str);
};

class FdTable
{
 private:
  // Used for generating IDs that stand in for feature and value strings
  std::map<std::string, FdFeature> feature_map;
  std::map<std::string, FdValue> value_map;
  
  std::map<SymbolNumber, FdOperation> operations;
 public:
  FdTable();
  void define_diacritic(SymbolNumber symbol, const std::string& str);
  
  FdFeature num_features() const { return feature_map.size(); }
  bool is_diacritic(SymbolNumber symbol) const;
  const FdOperation* get_operation(SymbolNumber symbol) const;
  
  bool is_valid_string(SymbolNumberVector symbols);
};

class FdState
{
 private:
  const FdTable& table;
  
  // This is indexed with values of type FdFeature
  std::vector<FdValue> values;
  
  bool error_flag;
 public:
  FdState(const FdTable& table):
    table(table), values(table.num_features()), error_flag(false) {}
  FdState(const FdState& o):
    table(o.table), values(o.values), error_flag(o.error_flag) {}
  
  bool apply_operation(const FdOperation& op);
  bool apply_operation(SymbolNumber symbol);
  bool fails();
  void reset();
};

#endif
