#include <iostream>
#include <cstdio>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

#include "hfst/HfstTransducer.h"
#include "hfst/HfstInputStream.h"
#include "hfst/HfstOutputStream.h"
#include "hfst/implementations/HfstTransitionGraph.h"


#include <boost/config.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/range/adaptor/reversed.hpp>



using namespace boost;
using namespace std;


using namespace hfst;
using namespace implementations;
using implementations::HfstBasicTransducer;
using implementations::HfstBasicTransition;
using implementations::HfstState;

set<HfstState> kernelSet {};

void error(int status, int error, const char* format, ...)
{
    cerr << "ERROR: " << format << "\n";
}

void add_kernel_successors_to_kernel(const HfstBasicTransducer & fsm, const HfstState s)
{
    //if state is in kernel
    if ( kernelSet.find(s) != kernelSet.end() )
    {
         // check all targets
        for (auto tr_it = fsm[s].begin(); tr_it != fsm[s].end(); ++tr_it)
        {
            HfstState target = tr_it->get_target_state ();
            // for all targets that are not in kernel, call function that adds it to kernel and again do the checking
            if ( kernelSet.find(target) == kernelSet.end() )
            {
                //add to kernel
               // cout << "Inserting predcessor state " << target << endl;
                kernelSet.insert(target);
                
                //check its targets
                add_kernel_successors_to_kernel(fsm, target);
                
            }
        
         
        }
    }
        
}

typedef std::set<std::pair<HfstState, std::string>> Key;
namespace std
{
    template <>
    struct hash <Key>
    {
        size_t operator()(const Key &K) const
        {
           // cout << "Hashing!" << endl;
            const auto Iter = K.begin();
            if(Iter == K.end())
                return 0;
             std::size_t seed = 0;
            for (const auto Iter : K )
            {
                hash_combine(seed,hash_value(Iter.first));
                hash_combine(seed,hash_value(Iter.second));
            }
            return (seed%100);
        }
    };
}

 
 
 //encode transitions
 //returns decodeMap
HfstSymbolPairSubstitutions encode_fsm(HfstBasicTransducer & fsm_encoded )
{
    int st = 0;
    int newAlph = 0;
    HfstSymbolPairSubstitutions encodeMap;
    HfstSymbolPairSubstitutions decodeMap;


    for ( auto it = fsm_encoded.begin(); it != fsm_encoded.end(); ++it, ++st)
    {
        for ( auto tr_it = fsm_encoded[st].begin(); tr_it != fsm_encoded[st].end(); ++tr_it)
        {
            HfstState target = tr_it->get_target_state ();
            String in = tr_it->get_input_symbol();
            String out = tr_it->get_output_symbol();
            float weight = tr_it->get_weight();
            
            pair <String, String > old_pair = {in, out};
            
            
            //if it wasn't already inserted, insert 
            if ( encodeMap.find(old_pair) == encodeMap.end() )
            {
                pair <String, String > new_pair = {to_string(newAlph), to_string(newAlph)};
                //if it doesn't already exist, add to map
                encodeMap[old_pair] = new_pair;
                decodeMap[new_pair] = old_pair;
                
                newAlph++;
            }
        }
    }

    //cout << encodeMap.size()  << endl;
    //cout << "Substitute" << endl;
    fsm_encoded.substitute(encodeMap);
    fsm_encoded.prune_alphabet();
    
    return decodeMap;
}
//decode transitions
//
void decode_fsm(HfstBasicTransducer & fsm_encoded, const HfstSymbolPairSubstitutions & decodeMap)
{
    fsm_encoded.substitute(decodeMap);
    fsm_encoded.prune_alphabet();
}

// build the successor state set
set< pair<HfstState, String>> create_successor_state_alph_set(const HfstState q, const HfstBasicTransducer & fsm)
{
    set< pair<HfstState, String>> succ;
    HfstBasicTransducer::HfstTransitions transitionsFromState = fsm.transitions(q);

    for(const auto &it : transitionsFromState )
    {
        HfstState target = it.get_target_state ();
        String in = it.get_input_symbol();
        succ.insert(pair<HfstState, String>{target, in});
    }
    return succ;
}



std::unordered_map< HfstState, set<HfstState>> compute_almost_equivalence(map<HfstState, set<HfstState> >  predecessorMap, HfstBasicTransducer fsm)
{
    std::unordered_map< HfstState, set<HfstState> > pi;
    unordered_set<HfstState> I;
    unordered_set<HfstState> P;
    
    std::unordered_map<std::set<std::pair<HfstState, std::string>>, int> h;

    
    
    int transducerSize = fsm.get_max_state() + 1;
    cout << "get_max_state " <<  transducerSize << endl;
    // initialise pi with singletons
    for ( int i = 0 ; i < transducerSize; ++i )
    {
       pair<HfstState, set<HfstState>> tmp{i, {i}};
       pi.insert(tmp);
       
       I.insert(i);
       P.insert(i);
    }
    
    //TODO get_max_state
    HfstState nonExistingState = 100;
    
//    fsm.prune_alphabet();
    StringSet alphabet = fsm.get_alphabet();
    
     cout << "Albhabet size:"  << alphabet.size() << endl ;

    

    
   

    /*
    cout << "alphabet: ";
    for (auto a : alphabet)
    {
        cout << a  << " " ;
    }
    cout << endl ;
    */
    cout << "I loop" <<endl ;
    int counter = 0;
    bool debug = false;
    while (!I.empty())
    {
        auto i_it = I.begin();
        HfstState q = *i_it;
        I.erase(i_it);
        
        if ( counter%1000 == 0 )
            debug = true;
        else
            debug = false;
        
        
        // build the successor state set
        // position in succ corresponds to alphabet symbol
        set< pair<HfstState, String>> succ; //target state, Input_symbol
        succ = create_successor_state_alph_set(q, fsm);

        /*
         cout << "succ: ";
        for (auto s : succ)
        {
            cout << s.first << " "<< s.second << ", " ;
        }
        cout <<"\n" <<endl;
*/
      //  cout << "succ if" <<endl ;

      
        auto succ_it = h.find(succ);
        if (succ_it != h.end())
        {   
            //cout<< "successor vector already in h, for state p"<<endl;
            
            HfstState p = succ_it->second;
          //  cout << "p " << p << " " ;
            set<HfstState> pip = pi[p];
            set<HfstState> piq = pi[q];
            
             // for efficiency's sake, swap p and q if needed
            if (pip.size() >= piq.size())
                swap(p,q);

            auto pit = P.find(p);
            if (pit != P.end())
                P.erase(pit);   // should always be the case
            pit = I.find(p);
            if (pit != I.end()) // is not always the case
                I.erase(pit);   // pop p from I
                
            // p and q are almost-equivalent, merge them
            //find predcessors of p (r1, r2 ...)
            //insert them to I
            //add transitions r -> q label?
            //remove transitions r -> p
            //delete state p
            
            // from predecessorMap to state q add r1, r2...
            // delete whole entry for p

            set<HfstState> predcessorSet = predecessorMap[p]; // that are element of P
            for (const auto &r : predcessorSet )
            {
                //r has to be element of P
                if (P.find(r) == P.end())
                    continue;
                

                //    print_transitions_from_state(p, fsm);
                
                
                
                I.insert(r);
                //cout << "Adding to I "<< r <<endl;
                
                
                                
                //erase from h!
                

                set< pair<HfstState, String>> succ_new; //target state, Input_symbol
                succ_new = create_successor_state_alph_set(r, fsm);
                //cout << "erasing from h, for r =" << r << endl;
                h.erase(succ_new);
  
                
                 //add transitions r -> q label?
                //remove transitions r -> p
                //delete state p
                

                HfstBasicTransducer::HfstTransitions transitionsFromState = fsm.transitions(r);
                for(auto it : transitionsFromState )
                {
                    if ( p == it.get_target_state() )
                    {
                        String in = it.get_input_symbol();
                        String out = it.get_output_symbol();
                        float weight = it.get_weight();

                        // Add a transition from state r to state q
                        fsm.add_transition(r, HfstBasicTransition(q, in, out, weight));
                  //      cout << "++adding tr: " << r << "->" << q << endl;
                        fsm.remove_transition(r, HfstBasicTransition(p, in, out, weight));
                    //    cout << "--removing tr: " << r << "->" << p << endl;
                    
                        predecessorMap[q].insert(r);
                    
                    
                    /* maybe not needed?
                        //erase all transitions going from p!
                        HfstBasicTransducer::HfstTransitions transitionsFrom_p = fsm.transitions(p);
                        for (const auto &it : transitionsFrom_p)
                        {
                             HfstState target = it.get_target_state();
                              String in = it.get_input_symbol();
                              String out = it.get_output_symbol();
                              float weight = it.get_weight();
                              fsm.remove_transition(p, HfstBasicTransition(target, in, out, weight));
                        }
                        */
                    }
                }

            }
            predecessorMap.erase(p);

            //erase p from values as well!!
            for(auto &i : predecessorMap )
            {
                i.second.erase(p);
            }
            
            
            
            set<HfstState> zero = {};
           // cout << "pi loop" <<endl ;
            for(auto pit = pi[p].begin();
                pit != pi[p].end(); ++pit)
            {
                // update q's class with p; empty p's class
                pi[q].insert(*pit);
            }
            pi[p] = zero;

            
        }
        try
        {
            h[succ] = q;
            
            /*
            cout << "h" << endl;
            for (auto it : h)
            {
                for (auto j:it.first)
                {
                    std::cout <<  j.first << " " ;
                }
                cout << "| " << it.second << endl;
            }
            std::cout << endl;
            
              */      
        #ifdef DEBUG
        {
            cout << counter << endl ;
            cout << "size of P " << P.size() << " --- size of I " << I.size() << endl;
            cout << "size of pi " << pi.size() << "-- h: " << h.size() << endl;
            
        }            
        #endif
            
            
            
            
            
            //for debugging
            if(counter%100 == 0 && false)
            {
                std::cout << "-------------------------------" << endl;
                cout << counter << endl;
                
                float k = h.load_factor();
                cout << "avg: " << k << endl;
                /*
                

                std::cout << "size = " << h.size() << std::endl;
                 unsigned n = h.bucket_count();
                 std::cout << "bucket_count = " << n << std::endl;
                 
                 for (unsigned i=0; i<n; ++i) {
                    std::cout << "bucket #" << i << " contains: ";
                    for (auto it = h.begin(i); it!=h.end(i); ++it)
                    {
                        std::cout << "[ " ;
                        for (auto j:it->first)
                        {
                            std::cout <<  j.first << " " << j.second << "  " ;
                        }
                        cout << "| " << it->second << " ] ";
                    }
                    std::cout << "\n";
                }
                */


            }
            counter++;
        }
        catch (std::bad_alloc& ba)
        {
            std::cerr << "bad_alloc caught: " << ba.what() << endl;
            std::cerr << "h size: " << h.size() << endl; 
        }
                
    }
    
    return pi;
}
void print_transitions_from_state(const HfstState p, const HfstBasicTransducer &fsm)
{
    HfstBasicTransducer::HfstTransitions transitionsFromState = fsm.transitions(p);

    for(const auto &it : transitionsFromState )
    {
        HfstState target = it.get_target_state ();
        String in = it.get_input_symbol();
        cout << p << "->" << target << " " << in <<endl;
        
    }
                        
}      


                  
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        error(EXIT_FAILURE, 0, "Function expects 1 argument - transducer");
        return 1;
    }
    const char* inFile = argv[1];


    std::cout << "input file: " << inFile << std::endl;


    HfstInputStream *in1;

    // Open files
    try
    {
        in1 = new HfstInputStream(inFile);
    }
    catch (StreamNotReadableException e)
    {
        printf("ERROR: File does not exist.\n");
        exit(1);
    }


    //let´s assume there is only one transducer in the file
    //or read just the first one
    if ( in1->is_bad() )
    {
        printf ( "ERROR: Stream cannot be read.\n" );
        exit(1);
    }

    cout << "Loading transducer" << endl;
    HfstTransducer t(*in1);
    //t.minimize();
    
    
    cout << "Creating bt" << endl;
    ImplementationType type = t.get_type();
    
    HfstBasicTransducer fsm(t);
    
    
   /*
    HfstOutputStream out1("original.hfst", TROPICAL_OPENFST_TYPE );
    HfstTransducer original(fsm, TROPICAL_OPENFST_TYPE );
   
    out1 << original; 
    out1.close();
    
    */
    
    
    
    //bool encode = true;
    HfstSymbolPairSubstitutions decodeMap;
    
    #ifdef ENCODE
    {
        cout << "Creating bt_encoded" << endl;

        //encode transitions into int/strings
        decodeMap = encode_fsm(fsm);
    }
    #endif

   

    
 

    
    
    
    
    
    
   
    map<HfstState, set<HfstState> > predecessorMap;
    
    // Creates boost graph (only transitions)
    // Creates predecessorMap
    // Adds to kernelSet states which have a self-loop
    adjacency_list<> G;
    HfstState s = 0;
    for (auto it = fsm.begin(); it != fsm.end(); ++it, ++s)
    {
        //for each transition, find input strings after epsilons (for target state)
        for (auto tr_it = fsm[s].begin(); tr_it != fsm[s].end(); ++tr_it)
        {
            
            HfstState target = tr_it->get_target_state ();
            //std::cout << s << " -> " << target << std::endl;
            
            add_edge(s, target, G);
            predecessorMap[target].insert(s);
            if (s == target)
            {
            //    cout << "Inserting loop state " << s << endl;
                kernelSet.insert(s);
            }
            
        }

    }
    int transducerSize = s ;
   std::cout << "-----Total number of states s : " << s << std::endl;
    
    /*
       cout << "predecessorMap" <<endl ;
          for ( auto i : predecessorMap)
          {
              cout << i.first << " | " ;
              for (auto j : i.second)
              {
                  cout << j << " ";
              }
              cout << endl;
          }
      */    
    /*
    std::cout << "My graph:" << std::endl;
    print_graph(G);
    std::cout << std::endl;
    */
    
    typedef graph_traits<adjacency_list<vecS, vecS, directedS> >::vertex_descriptor Vertex;
    std::vector<int> component(num_vertices(G)), discover_time(num_vertices(G));
    std::vector<default_color_type> color(num_vertices(G));
    std::vector<Vertex> root(num_vertices(G));
    int num = strong_components(G, make_iterator_property_map(component.begin(), get(vertex_index, G)), 
                              root_map(make_iterator_property_map(root.begin(), get(vertex_index, G))).
                              color_map(make_iterator_property_map(color.begin(), get(vertex_index, G))).
                              discover_time_map(make_iterator_property_map(discover_time.begin(), get(vertex_index, G))));

    std::cout << "Total number of components: " << num << std::endl;
    map< int, set<HfstState> > scc{};
    int i = 0;
    for ( auto c : component )
    {
        scc[c].insert(i);
        i++;
    }
 
    //go through all scc, find which have more than 2 elements, add them to kernelSet
    for (const auto compPair : scc )
    {
        if ( compPair.second.size() >= 2 )
        {
            for ( auto state : compPair.second )
            {
                if ( kernelSet.find(state) == kernelSet.end() )
                {
                  //  cout << "Inserting >=2 " << state << endl;
                    kernelSet.insert(state);
                }
            }
        }
     }

    s = 0;
    // go through all states
    // for each state, if it has a kernel predecessor, add that state as well
    for (auto it = fsm.begin(); it != fsm.end(); ++it, ++s)
    {
        add_kernel_successors_to_kernel(fsm, s);
    }
    
    //if it's not in kernel, add it to preamble set
    std::set<HfstState> preambleSet;
    for ( int i = 0 ; i < transducerSize; ++i )
    {
        if ( kernelSet.find(i) == kernelSet.end() )
        {
            preambleSet.insert(i);
        }
    }
    
      
      
      
      
      
      
      
      
      //compute almost eqivalence
  //  HfstBasicTransducer fsm_copy(fsm);
    std::unordered_map< HfstState, set<HfstState> > pi;
    pi = compute_almost_equivalence(predecessorMap, fsm);
    
   
    
    
    // merge states
    //for each state (in pi), if it's not empty, create kernel/preamble subsets
    //if psubset is not empty, merge states
    
    for (const auto &i : pi)
    {
        if( i.second.size() > 1 )
        {
            set<HfstState> psubset;
            set<HfstState> ksubset;
           // cout << endl;
            for ( auto state : i.second )
            {
                if ( kernelSet.find(state) != kernelSet.end() )
                {
                    //cout << state << " is in kernel" << endl;
                    ksubset.insert(state);
                }
                else
                {
                   // cout << state << " is in preamble" << endl;
                    psubset.insert(state);
                }
            }
            //if there are preamble states, merge them
            if (!psubset.empty())
            {
                HfstState q;
                //if kernel is not empty, merge all states to one kernel state
                //else merge everything to one preamble state
                if (!ksubset.empty()) q = *ksubset.begin();
                else                  q = *psubset.begin();
               // cout << "The chosen state is........ TADA...." << q << endl;

                //merge all states from psubset to q
                for(auto p : psubset)
                {
                    if (p != q)
                    {
                        //merge two states
                       // cout << "Merging " << p << " into " << q << endl;
                        set<HfstState> predcessorSet = predecessorMap[p]; // that are element of P
                        for (const auto &r : predcessorSet )
                        {
                            // get r->p symbols and weight
                            HfstBasicTransducer::HfstTransitions transitionsFromState = fsm.transitions(r);
                            for(auto it : transitionsFromState )
                            {
                                if ( p == it.get_target_state() )
                                {
                                    String in = it.get_input_symbol();
                                    String out = it.get_output_symbol();
                                    float weight = it.get_weight();

                                    // Add a transition from state r to state q
                                    fsm.add_transition(r, HfstBasicTransition(q, in, out, weight));
                              //      cout << "++adding tr: " << r << "->" << q << endl;
                                    fsm.remove_transition(r, HfstBasicTransition(p, in, out, weight));
                                //    cout << "--removing tr: " << r << "->" << p << endl;

                                    predecessorMap[q].insert(r);
                    
/*
                                    //erase all transitions going from p!
                                    HfstBasicTransducer::HfstTransitions transitionsFrom_p = fsm.transitions(p);
                                    for (const auto &it : transitionsFrom_p)
                                    {
                                         HfstState target = it.get_target_state();
                                          String in = it.get_input_symbol();
                                          String out = it.get_output_symbol();
                                          float weight = it.get_weight();
                                          fsm.remove_transition(p, HfstBasicTransition(target, in, out, weight));
                                    }
                                    */
                                }
                            }

                        }
                        predecessorMap.erase(p);

                        //erase p from values as well!!
                        for(auto &i : predecessorMap )
                        {
                            i.second.erase(p);
                        }
                    }
                }
                
            }
            
            
            
            
         }
    }
    

    #ifdef ENCODE
    {
        cout << "Decoding tr" << endl;

        //decode transitions into int/strings
        decode_fsm(fsm, decodeMap);
    }
    #endif
    
    //write in foma /dot
    //todo change to type
    HfstOutputStream out("hyperminimal.hfst", TROPICAL_OPENFST_TYPE );
    HfstTransducer hyperminimal(fsm, TROPICAL_OPENFST_TYPE );
    hyperminimal.minimize();





    
    HfstBasicTransducer tmp(hyperminimal);
    cout << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "SIZES: original: " << transducerSize << " hyperminimal: " << tmp.get_max_state() + 1 << endl;
    cout << "----------------------------------------------------------" << endl;
    
    out << hyperminimal; 
    out.close();

    
    
    
    
  
    
    
    //printing
    
    ofstream pi_file;
    pi_file.open ("pi.txt");
    
    for (const auto i : pi )
    {
        pi_file << i.first << " | ";
        for ( auto j : i.second )
        {
            pi_file  << j << " " ;
        }
        pi_file << endl ;
    }
   pi_file.close();
    
    cout << "\npi file done pi.txt\n" ;

    
    ofstream kernel_file;
    kernel_file.open ("kernel.txt");
    cout << "Kernel size: " << kernelSet.size() << "--" << endl ; 
    for ( auto kernelState : kernelSet )
    {
        kernel_file << kernelState << "\n";
    }
   kernel_file.close();
    
    cout << "kernel file done kernel.txt\n" ;
        
    
    cout << "Preamble size: " << preambleSet.size() << "--" << endl ; 
     ofstream preamble_file;
    preamble_file.open ("preamble.txt");
    for ( auto preambleState : preambleSet )
    {
      //  cout << preambleState << " " ;
         preamble_file << preambleState << "\n";
    }

    cout << "preamble file done preamble.txt\n" ;
      preamble_file.close();

    in1->close();
    delete in1;

    return 0;
 }

