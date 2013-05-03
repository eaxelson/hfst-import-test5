#include "hfst-ol-handle.h"

namespace hfst_ol
{

void TransducerHandle::initialize(FILE * f)
{
    if (f == (FILE *) NULL)
    { throw BadFileHandleException(); }
    
    header = new TransducerHeader(f);
    TransducerAlphabet alphabet(f, header->symbol_count());
    
    if (alphabet.hasFlags() == false)
    {
        if (header->probe_flag(Weighted) == false)
        {
            type = UnweightedT;
            transducer_ptr.tr = new Transducer(f, *header, alphabet);
        }
        else
        {
            type = WeightedT;
            transducer_ptr.tr_w = new TransducerW(f, *header, alphabet);
        }
    }
    else
    {
        if (header->probe_flag(Weighted) == false)
        {
            type = Unweighted_flaggedT;
            transducer_ptr.tr_fd = new TransducerFd(f, *header, alphabet);
        }
        else
        {
            type = Weighted_flaggedT;
            transducer_ptr.tr_wfd = new TransducerWFd(f, *header, alphabet);
        }
    }
}

bool TransducerHandle::set_input(char * str)
{
    switch (type)
    {
    case UnweightedT:
        return transducer_ptr.tr->set_input(str);
        break;
    case WeightedT:
        return transducer_ptr.tr_w->set_input(str);
        break;
    case Unweighted_flaggedT:
        return transducer_ptr.tr_fd->set_input(str);
        break;
    case Weighted_flaggedT:
        return transducer_ptr.tr_wfd->set_input(str);
        break;
    }
}
  
OutputVector TransducerHandle::returnAnalyses(void)
{
    switch (type)
    {
    case UnweightedT:
        return transducer_ptr.tr->returnAnalyses();
        break;
    case WeightedT:
        return transducer_ptr.tr_w->returnAnalyses();
        break;
    case Unweighted_flaggedT:
        return transducer_ptr.tr_fd->returnAnalyses();
        break;
    case Weighted_flaggedT:
        return transducer_ptr.tr_wfd->returnAnalyses();
        break;
    }
}

bool TransducerHandle::isWeighted(void)
{
    if (type == WeightedT or type == Weighted_flaggedT)
    { return true; }
    return false;
}

bool TransducerHandle::hasFlagDiacritics(void)
{
    if (type == Unweighted_flaggedT or type == Weighted_flaggedT)
    {return true; }
    return false;
}

bool TransducerHandle::isDeterministic(void)
{ return header->probe_flag(Deterministic); }

bool TransducerHandle::isInputDeterministic(void)
{ return header->probe_flag(Input_deterministic); }

bool TransducerHandle::isMinimized(void)
{ return header->probe_flag(Minimized); }
  
bool TransducerHandle::isCyclic(void)
{ return header->probe_flag(Cyclic); }

bool TransducerHandle::hasEpsilonEpsilonTransitions(void)
{ return header->probe_flag(Has_epsilon_epsilon_transitions); }
  
bool TransducerHandle::hasInputEpsilonTransitions(void)
{ return header->probe_flag(Has_input_epsilon_transitions); }

bool TransducerHandle::hasInputEpsilonCycles(void)
{
    return (header->probe_flag(Has_input_epsilon_cycles) or\
            header->probe_flag(Has_unweighted_input_epsilon_cycles));
}

unsigned short TransducerHandle::symbolCount(void)
{ return header->symbol_count(); }
  
unsigned short TransducerHandle::inputSymbolCount(void)
{ return header->input_symbol_count(); }
  
unsigned int TransducerHandle::stateCount(void)
{ return header->state_count(); }
  
unsigned int TransducerHandle::transitionCount(void)
{ return header->transition_count(); }

}
