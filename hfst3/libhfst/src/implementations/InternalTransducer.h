namespace hfst {
  namespace implementations {
  
    class InternalTransducerLine {
    public:
      bool final_line;
      unsigned int origin;
      unsigned int target;
      std::string isymbol;
      std::string osymbol;
      float weight;
      
      InternalTransducerLine():
      final_line(false), origin(0), target(0), 
	isymbol(std::string("")), osymbol(std::string("")),
	weight((float)0) 
	  {};
      ~InternalTransducerLine() 
	{};
    };
    
    class HfstInternalTransducer;

    class HfstInternalTransducer {
    public:
      InternalTransducerLine line;
      HfstInternalTransducer * next;    

      HfstInternalTransducer(): line(InternalTransducerLine()), next(NULL) {};
      ~HfstInternalTransducer() {
	while (next != NULL)
	  delete next;
      }

    };

  }
}
