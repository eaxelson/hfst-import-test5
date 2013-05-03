#ifndef HFST_OL_HANDLE_H_
#define HFST_OL_HANDLE_H_

#include "hfst-ol-lib.h"

namespace hfst_ol
{

    class BadFileHandleException: public std::exception
    {
    public:
        virtual const char* what() const throw()
        {
            return("Bad file handle passed");
        }
    };
  
    enum TransducerType {
        UnweightedT, WeightedT, Unweighted_flaggedT, Weighted_flaggedT, UndefinedT};

    union TransducerPointer {
        Transducer* tr;
        TransducerFd* tr_fd;
        TransducerW* tr_w;
        TransducerWFd* tr_wfd;
    };
  
    class TransducerHandle
    {
    private:
    
        TransducerPointer transducer_ptr;
        TransducerType type;
        TransducerHeader* header;
    
    public:
    
    TransducerHandle():
        type(UndefinedT),
            header(NULL)
            {
                transducer_ptr.tr = NULL;
            }

        TransducerHandle(FILE * f)
        {
            initialize(f);
        }

        void initialize(FILE * f);

        bool set_input(char * str);

        OutputVector returnAnalyses(void);

        bool isWeighted(void);
        bool hasFlagDiacritics(void);
        bool isDeterministic(void);
        bool isInputDeterministic(void);
        bool isMinimized(void);
        bool isCyclic(void);
        bool hasEpsilonEpsilonTransitions(void);
        bool hasInputEpsilonTransitions(void);
        bool hasInputEpsilonCycles(void);
        unsigned short symbolCount(void);
        unsigned short inputSymbolCount(void);
        unsigned int stateCount(void);
        unsigned int transitionCount(void);
  
    };
}




#endif // HFST_OL_LIB_TRANSDUCER_HANDLE_H_
