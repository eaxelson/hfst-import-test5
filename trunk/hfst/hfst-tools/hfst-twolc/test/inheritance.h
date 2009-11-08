class X
{
 protected:
  int i;
 public:
 X(int x):
  i(x) {};
};

class Y : public X
{
 public:
 Y(int x):
  X(x) {};
};

class Z : public Y
{
 protected:
  int j;
 public:
 Z(int x):
  Y(x)
  {
    j = i;
  }
  
};
