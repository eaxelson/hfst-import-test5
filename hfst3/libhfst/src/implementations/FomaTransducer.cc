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

#include "FomaTransducer.h"

#ifndef MAIN_TEST
namespace hfst { namespace implementations {


  // ---------- FomaInputStream functions ----------

    /** Create a FomaInputStream that reads from stdin. */
  FomaInputStream::FomaInputStream(void)
  {
    input_file = stdin;
  }
    /** Create a FomaInputStream that reads from file \a filename. */
    FomaInputStream::FomaInputStream(const std::string &filename_):
      filename(std::string(filename_))
  {
    if (filename == std::string())
      { input_file = stdin; }
    else {
      input_file = fopen(filename.c_str(),"r");
      if (input_file == NULL)
        { 
          HFST_THROW(StreamNotReadableException); }
    }
  }

    /** Close the stream. */
  void FomaInputStream::close(void)
  {
    if (input_file == NULL)
      { return; }
    if (filename.c_str()[0] != 0)
      {
    fclose(input_file);
    input_file = NULL;
      }
  }
  
  char FomaInputStream::stream_get() {
    return (char) fgetc(input_file); }

  short FomaInputStream::stream_get_short() 
  {
    short i;
    assert(1 == fread(&i,sizeof(i),1,input_file));
    return i;
  }

  void FomaInputStream::stream_unget(char c) {
    ungetc ( (int)c, input_file ); }

  bool FomaInputStream::is_eof(void)
  {
    int c = getc(input_file);
    bool retval = (feof(input_file) != 0);
    ungetc(c, input_file);
    return retval;
  }
  
  bool FomaInputStream::is_bad(void)
  {
    return is_eof();
  }
  
  bool FomaInputStream::is_good(void)
  {
    return not is_bad();
  };
  
  bool FomaInputStream::is_fst(void)
  {
    return is_fst(input_file);
  }
  
  bool FomaInputStream::is_fst(FILE * f)
  {
    if (f == NULL)
      { return false; }
    int c = getc(f);
    ungetc(c, f);
    return c == 31 || c == (int)'#';
  }
  
  bool FomaInputStream::is_fst(std::istream &s)
  {
    int c = s.peek();
    return s.good() && (c == 31 || c == (int)'#');
  }

  /* Skip the identifier string "FOMA_TYPE" */
  void FomaInputStream::skip_identifier_version_3_0(void)
  { 
    char foma_identifier[10];
    int foma_id_count = fread(foma_identifier,10,1,input_file);
    if (foma_id_count != 1)
      { 
        HFST_THROW(NotTransducerStreamException); }
    if (0 != strcmp(foma_identifier,"FOMA_TYPE"))
      {
        HFST_THROW(NotTransducerStreamException); }
  }
  
  void FomaInputStream::skip_hfst_header(void)
  {
    char hfst_header[6];
    int header_count = fread(hfst_header,6,1,input_file);
    if (header_count != 1)
      {  
        HFST_THROW(NotTransducerStreamException); }
    //int c = fgetc(input_file);
    //switch (c)
    //{
    // case 0:
    try { skip_identifier_version_3_0(); }
    //catch (NotTransducerStreamException e) { throw e; }
    catch (const HfstException e)
      { throw e; }
    //break;
    //default:
    //assert(false);
    //}
  }

    void FomaInputStream::ignore(unsigned int n)
    { 
      for (unsigned int i=0; i<n; i++)
    fgetc(input_file);
    }

  fsm * FomaInputStream::read_transducer()
  {
    if (is_eof())
      return NULL;
    struct fsm * t = FomaTransducer::read_net(input_file);
    if (t == NULL) {
      HFST_THROW(NotTransducerStreamException);
    }
    return t;
  };

  // ---------- FomaOutputStream functions ----------

  FomaOutputStream::FomaOutputStream(void)
  { ofile = stdout; }

    FomaOutputStream::FomaOutputStream(const std::string &str):
      filename(std::string(str))
  {
    if (filename != std::string()) {
      ofile = fopen(filename.c_str(), "wb");
      if (ofile == NULL) {
        HFST_THROW(StreamNotReadableException);
      }
    } 
    else {
      ofile = stdout;
    }
  }

  void FomaOutputStream::close(void) 
  {
    if (filename != std::string())
      { fclose(ofile); }
  }

  void FomaOutputStream::write(const char &c)
  {
    fputc(c,ofile);
  }
    
    void FomaOutputStream::write_transducer(struct fsm * transducer) 
  { 
    if (1 != FomaTransducer::write_net(transducer, ofile)) {
      HFST_THROW_MESSAGE
        (HfstFatalException,
         "an error happened when writing a foma transducer");
    }
  }

  
  fsm * FomaTransducer::create_empty_transducer(void)
  {    
    struct fsm * retval = fsm_empty_set();
    return retval;
  }
  
  fsm * FomaTransducer::create_epsilon_transducer(void)
  {
    return fsm_empty_string();
  }
  
  void FomaTransducer::harmonize(struct fsm *net1, struct fsm *net2)
  {
    fsm_merge_sigma(net1, net2);
  }

  void FomaTransducer::delete_foma(struct fsm * net)
  {
    fsm_destroy(net);
  }

    fsm * FomaTransducer::define_transducer(const std::string &symbol)
  {     
    fsm * retval = fsm_symbol(strdup(symbol.c_str()));
    fsm_count(retval);
    return retval;
  }

  fsm * FomaTransducer::define_transducer
    (const std::string &isymbol, const std::string &osymbol)
  { 
    return fsm_cross_product( fsm_symbol(strdup(isymbol.c_str())), 
                              fsm_symbol(strdup(osymbol.c_str())) );
    // should either argument be deleted?
  }
  
  fsm * FomaTransducer::define_transducer(const StringPairVector &spv)
  {
    if (spv.empty())
      return fsm_empty_string();
    
    int state_number=0;

    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    for (StringPairVector::const_iterator it = spv.begin(); 
         it != spv.end(); it++) 
      {
        char *in = strdup(it->first.c_str());
        char *out = strdup(it->second.c_str());
        fsm_construct_add_arc(h, state_number, state_number+1, in, out);
        state_number++;
      }
    
    fsm_construct_set_initial(h, 0);
    fsm_construct_set_final(h, state_number);    

    net = fsm_construct_done(h);
    fsm_count(net);

    sigma_add_special (0, net->sigma);
    sigma_add_special (1, net->sigma);
    sigma_add_special (2, net->sigma);
    
    return net;      
  }

  fsm * FomaTransducer::define_transducer(const StringPairSet &sps, bool cyclic)
  {
    if (sps.empty())
      return fsm_empty_string();
    
    int source=0;
    int target = (cyclic) ? 0 : 1;

    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    for (StringPairSet::const_iterator it = sps.begin(); it != sps.end(); it++) 
      {
    char *in = strdup(it->first.c_str());
    char *out = strdup(it->second.c_str());
    fsm_construct_add_arc(h, source, target, in, out);
      }
    
    fsm_construct_set_initial(h, source);
    fsm_construct_set_final(h, target);    

    net = fsm_construct_done(h);
    fsm_count(net);
    
    return net;      
  }

  fsm * FomaTransducer::define_transducer
    (const std::vector<StringPairSet> &spsv)
  {
    if (spsv.empty())
      return fsm_empty_string();
    
    int state_number=0;

    struct fsm_construct_handle *h;
    struct fsm *net;
    h = fsm_construct_init(strdup(std::string("").c_str()));
    
    for (std::vector<StringPairSet>::const_iterator it = spsv.begin(); 
         it != spsv.end(); it++) 
      {
        for (StringPairSet::const_iterator it2 = (*it).begin(); 
             it2 != (*it).end(); it2++ ) {
          char *in = strdup(it2->first.c_str());
          char *out = strdup(it2->second.c_str());
          fsm_construct_add_arc(h, state_number, state_number+1, in, out);
        }
        state_number++;
      }
    
    fsm_construct_set_initial(h, 0);
    fsm_construct_set_final(h, state_number);    

    net = fsm_construct_done(h);
    fsm_count(net);
    
    return net;      
  }

  fsm * FomaTransducer::copy(fsm * t)
  {     
    return fsm_copy(t);
  }
  
  fsm * FomaTransducer::determinize(fsm * t)
  {     
    // returns the argument, so a copy is taken here
    return fsm_determinize(fsm_copy(t));
  }
  
  fsm * FomaTransducer::minimize(fsm * t)
  {     
    // returns the argument, so a copy is taken here
    return fsm_minimize(fsm_copy(t));
  }
  
  fsm * FomaTransducer::remove_epsilons(fsm * t)
  {
    // returns the argument, so a copy is taken here
    return fsm_epsilon_remove(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_star(fsm * t)
  {     
    return fsm_kleene_star(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_plus(fsm * t)
  {     
    return fsm_kleene_plus(fsm_copy(t));
  }
  
  fsm * FomaTransducer::repeat_n(fsm * t, unsigned int n)
  {     
    return fsm_concat_n(t, n);
  }
  
  fsm * FomaTransducer::repeat_le_n(fsm * t, unsigned int n)
  { 
    return fsm_concat_m_n(t,0,n);
  }
  
  fsm * FomaTransducer::optionalize(fsm * t)
  { 
    return fsm_optionality(fsm_copy(t));
  }
  
  fsm * FomaTransducer::invert(fsm * t)
  {
    return fsm_invert(fsm_copy(t));
  }
  
  fsm * FomaTransducer::reverse(fsm * t)
  { 
    return fsm_reverse(fsm_copy(t));
  }
  
  fsm * FomaTransducer::extract_input_language(fsm * t)
  { 
    return fsm_upper(fsm_copy(t));
  }
  
  fsm * FomaTransducer::extract_output_language(fsm * t)
  {
    return fsm_lower(fsm_copy(t));
  }
  
  fsm * FomaTransducer::substitute(fsm * t,String old_symbol,String new_symbol)
  {
    return fsm_substitute_symbol(t, 
                                 strdup(old_symbol.c_str()), 
                                 strdup(new_symbol.c_str()));
  }
  
    /* Conversion to HfstBasicTransducer is now used instead. */
  fsm * FomaTransducer::insert_freely(fsm * t, const StringPair &symbol_pair)
  {
    char* epsilon = strdup("@_EPSILON_SYMBOL_@");
    char* epsilon_marker = strdup("@_EPSILON_SYMBOL_MARKER_");
    char* identity = strdup("@_IDENTITY_SYMBOL_@");
    fsm * eps_marked = fsm_substitute_symbol(t, epsilon, 
                                             epsilon_marker);
    fsm * ins = fsm_kleene_star( 
          fsm_union( fsm_symbol(identity), 
                 fsm_cross_product( fsm_symbol(epsilon),
                        fsm_symbol(strdup(symbol_pair.second.c_str())) ) ) );
    fsm * comp = fsm_substitute_symbol( fsm_compose(eps_marked, ins),
                    epsilon,
                    strdup(symbol_pair.first.c_str()) );
    return fsm_substitute_symbol( comp,
                  epsilon_marker,
                  epsilon);
    free(epsilon);
    free(epsilon_marker);
    free(identity);
    // marker should be removed from sigma.. 
    // (HfstBasicTransducer is now used instead)
  }
  
  fsm * FomaTransducer::compose
  (fsm * t1, fsm * t2)
  {
    return fsm_compose(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::concatenate
  (fsm * t1, fsm * t2)
  {
    return fsm_concat(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::disjunct
  (fsm * t1, fsm * t2)
  {
    return fsm_union(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::intersect
  (fsm * t1, fsm * t2)
  {
    return fsm_intersect(fsm_copy(t1), fsm_copy(t2));
  }

  fsm * FomaTransducer::subtract
  (fsm * t1, fsm * t2)
  {
    return fsm_minus(fsm_copy(t1), fsm_copy(t2));
  }

  bool FomaTransducer::are_equivalent
  (fsm * t1, fsm * t2)
  {
    return fsm_isempty(fsm_union(fsm_minus(fsm_copy(t1),fsm_copy(t2)),
                                 fsm_minus(fsm_copy(t1),fsm_copy(t2))));    
  }

  bool FomaTransducer::is_cyclic(fsm * t)
  {
    fsm_topsort(t);
    return !(t->is_loop_free);
  }
  
  
  static bool extract_paths
  (fsm * t, int state,
   std::map<int,unsigned short> all_visitations, 
   std::map<int, unsigned short> path_visitations,
   /*std::vector<char>& lbuffer, int lpos, 
     std::vector<char>& ubuffer, int upos,*/
   ExtractStringsCb& callback, int cycles,
   std::vector<hfst::FdState<int> >* fd_state_stack, 
   bool filter_fd, 
   /*bool include_spv,*/ StringPairVector &spv)
  {
    if(cycles >= 0 && path_visitations[state] > cycles)
      return true;
    all_visitations[state]++;
    path_visitations[state]++;
    
    /*
    if(lpos > 0 && upos > 0)
    {
      lbuffer[lpos] = 0;
      ubuffer[upos] = 0;
      
      //check finality
      bool final = false;
      for(int i=0; ((t->states)+i)->state_no != -1; i++)
      {
        fsm_state* s = (t->states)+i;
        if(s->state_no == state && s->final_state == 1)
        {
          final = true;
          break;
        }
      }
      
      hfst::WeightedPath<float> path(&lbuffer[0],&ubuffer[0],0);
      if (include_spv) {
        path.spv = spv;
        path.is_spv_in_use = true;
      }
      hfst::ExtractStringsCb::RetVal ret = callback(path, final);
      if(!ret.continueSearch || !ret.continuePath)
      {
        path_visitations[state]--;
        return ret.continueSearch;
      }
    }
    */

    if (spv.size() != 0)
      {
        //check finality
        bool final = false;
        for(int i=0; ((t->states)+i)->state_no != -1; i++)
          {
            fsm_state* s = (t->states)+i;
            if(s->state_no == state && s->final_state == 1)
              {
                final = true;
                break;
              }
          }
        
        hfst::HfstTwoLevelPath path(0, spv);
        hfst::ExtractStringsCb::RetVal ret = callback(path, final);
        if(!ret.continueSearch || !ret.continuePath)
          {
            path_visitations[state]--;
            return ret.continueSearch;
          }
      }
    

    // find and sort transitions
    std::vector<fsm_state*> sorted_arcs;
    for(int i=0; ((t->states)+i)->state_no != -1; i++)
    {
      fsm_state* s = (t->states)+i;
      if(s->state_no == state && s->target != -1)
      {
        size_t j;
        for(j=0; j<sorted_arcs.size(); j++)
          if (all_visitations[s->target] 
              < all_visitations[sorted_arcs[j]->target])
            break;
        sorted_arcs.push_back(NULL);
        for( size_t k=sorted_arcs.size()-1; k>j; k-- )
          sorted_arcs[k] = sorted_arcs[k-1];
        sorted_arcs[j] = s;
      }
    }
    

    bool res = true;
    for(size_t i=0; i<sorted_arcs.size() && res == true; i++)
    {
      fsm_state* arc = sorted_arcs[i];
      
      bool added_fd_state = false;
      
      if (fd_state_stack) {
        if(fd_state_stack->back().get_table().get_operation(arc->in) != NULL) {
          fd_state_stack->push_back(fd_state_stack->back());
          if(fd_state_stack->back().apply_operation(arc->in))
            added_fd_state = true;
          else {
            fd_state_stack->pop_back();
            continue; // don't follow the transition
          }
        }
      }
      
      /*
      int lp=lpos;
      int up=upos;
      
      if(arc->in != 0 && 
         (!filter_fd || 
          fd_state_stack->back().get_table().get_operation(arc->in)==NULL))
      {
        //find the key in sigma
        char* c=NULL;
        for(struct sigma* sig=t->sigma; sig!=NULL&&sig->symbol!=NULL; 
            sig=sig->next)
        {
          if(sig->number == arc->in)
          {
            c = sig->symbol;
            break;
          }
        }
        size_t clen = strlen(c);
        if(lpos+clen >= lbuffer.size())
          lbuffer.resize(lbuffer.size()*2, 0);
        strcpy(&lbuffer[lpos], c);
        lp += clen;
      }
      if(arc->out != 0 && 
         (!filter_fd || 
          fd_state_stack->back().get_table().get_operation(arc->out)==NULL))
      {
        //find the key in sigma
        char* c=NULL;
        for(struct sigma* sig=t->sigma; sig!=NULL&&sig->symbol!=NULL; 
            sig=sig->next)
        {
          if(sig->number == arc->out)
          {
            c = sig->symbol;
            break;
          }
        }
        size_t clen = strlen(c);
        if(upos+clen > ubuffer.size())
          ubuffer.resize(ubuffer.size()*2, 0);
        strcpy(&ubuffer[upos], c);
        up += clen;
      }
      */      


      /* Handle spv here. Special symbols (flags, epsilons) 
         are always inserted. */

      std::string istring("");
      std::string ostring("");
    
      //find the key in sigma
      char* c_in=NULL;
      for(struct sigma* sig=t->sigma; sig!=NULL&&sig->symbol!=NULL; 
          sig=sig->next)
        { if(sig->number == arc->in) {
            c_in = sig->symbol;
            break; }
        }

      //find the key in sigma
      char* c_out=NULL;
      for(struct sigma* sig=t->sigma; sig!=NULL&&sig->symbol!=NULL; 
          sig=sig->next) {
        if(sig->number == arc->out) {
          c_out = sig->symbol;
          break; }
      }

      if (!filter_fd || 
          fd_state_stack->back().get_table().get_operation(arc->in)==NULL)
        istring = strdup(c_in);

      if (!filter_fd || 
          fd_state_stack->back().get_table().get_operation(arc->out)==NULL)
        ostring = strdup(c_out);

      spv.push_back(StringPair(istring, ostring));

      res = extract_paths(t, arc->target, all_visitations, path_visitations,
                            /*lbuffer, lp, ubuffer, up,*/ callback, cycles,
                            fd_state_stack, filter_fd,
                            /*include_spv,*/ spv);

      spv.pop_back();

      if(added_fd_state)
        fd_state_stack->pop_back();
    }
    
    path_visitations[state]--;
    return res;
  }
  
  static const int BUFFER_START_SIZE = 64;
  
  void FomaTransducer::extract_paths
  (fsm * t, ExtractStringsCb& callback,
   int cycles, FdTable<int>* fd, bool filter_fd 
   /*bool include_spv*/)  
  {
    //std::vector<char> lbuffer(BUFFER_START_SIZE, 0);
    //std::vector<char> ubuffer(BUFFER_START_SIZE, 0);
    std::map<int, unsigned short> all_visitations;
    std::map<int, unsigned short> path_visitations;
    std::vector<hfst::FdState<int> >* fd_state_stack 
      = (fd==NULL) ? NULL : new std::vector<hfst::FdState<int> >
      (1, hfst::FdState<int>(*fd));
    
    StringPairVector spv;
    bool res = true;
    for (int i=0; ((t->states)+i)->state_no != -1 && res == true; i++) {
      if (((t->states)+i)->start_state == 1)
        res = hfst::implementations::extract_paths
          (t, ((t->states)+i)->state_no, all_visitations, path_visitations,
           /*lbuffer, 0, ubuffer, 0,*/ callback, cycles, fd_state_stack, 
           filter_fd, /*include_spv,*/ spv);
    }
  }

    void FomaTransducer::extract_random_paths
    (const fsm *t , HfstTwoLevelPaths &results, int max_num)
    {
      (void)t;
      (void)results;
      (void)max_num;
      HFST_THROW(FunctionNotImplementedException);
    }
  
  void FomaTransducer::insert_to_alphabet(fsm * t, const std::string &symbol)
  {
    sigma_add(strdup(symbol.c_str()), t->sigma);
  }    

  StringSet FomaTransducer::get_alphabet(fsm *t)
  {
    StringSet alpha;
    for(struct sigma* p = t->sigma; p!=NULL; p=p->next)
    {
      if (p->symbol == NULL)
        break;
      alpha.insert(std::string(p->symbol));
    }
    // it seems that the specail symbols are not always included, but foma
    // is still aware of them..
    alpha.insert(internal_epsilon);
    alpha.insert(internal_unknown);
    alpha.insert(internal_identity);
    return alpha;
  }

  FdTable<int>* FomaTransducer::get_flag_diacritics(fsm * t)
  {
    FdTable<int>* table = new FdTable<int>();
    for(struct sigma* p = t->sigma; p!=NULL; p=p->next)
    {
      if (p->symbol == NULL)
        break;
      if(FdOperation::is_diacritic(p->symbol))
        table->define_diacritic(p->number, p->symbol);
    }
    return table;
  }

  void FomaTransducer::print_test(fsm * t)
  {
    net_print_att(t, stdout);
  }

    // HFST additions

    static int io_gets(FILE *infile, char *target);
    static inline int explode_line (char *buf, int *values);

    /* Read foma transducer . */
    struct fsm * FomaTransducer::read_net(FILE *infile) {
      
    unsigned int READ_BUF_SIZE=4096; 
    char buf[READ_BUF_SIZE];
    struct fsm *net;
    struct fsm_state *fsm;
    
    char *new_symbol;
    int i, items, new_symbol_number, laststate, lineint[5], *cm;
    char last_final = 0;

    if (io_gets(infile, buf) == 0) {
        return NULL;
    }
    
    char* empty = strdup("");
    net = fsm_create(empty);
    free(empty);
    
    if (strcmp(buf, "##foma-net 1.0##") != 0) {
        printf("File format error foma!\n");
        return NULL;
    }
    io_gets(infile, buf);
    if (strcmp(buf, "##props##") != 0) {
        printf("File format error props!\n");
        return NULL;
    }
    /* Properties */
    io_gets(infile, buf);
    sscanf(buf, "%i %i %i %i %i %lld %i %i %i %i %i %i %s", 
           &net->arity, 
           &net->arccount, 
           &net->statecount, 
           &net->linecount, 
           &net->finalcount, 
           &net->pathcount, 
           &net->is_deterministic, 
           &net->is_pruned, 
           &net->is_minimized, 
           &net->is_epsilon_free, 
           &net->is_loop_free, 
           &net->is_completed, 
           buf);
    strcpy(net->name, buf);
    //*net_name = strdup(buf);
    io_gets(infile, buf);

    /* Sigma */
    if (strcmp(buf, "##sigma##") != 0) {
        printf("File format error sigma!\n");
        return NULL;
    }
    net->sigma = sigma_create();
    for (;;) {
      io_gets(infile, buf);
        if (buf[0] == '#') break;
        new_symbol = strstr(buf, " ");
        new_symbol[0] = '\0';
        new_symbol++;
        sscanf(buf,"%i", &new_symbol_number);
        sigma_add_number(net->sigma, new_symbol, new_symbol_number);
    }
    /* States */
    if (strcmp(buf, "##states##") != 0) {
        printf("File format error!\n");
        return NULL;
    }
    net->states = (fsm_state*) malloc(net->linecount*sizeof(struct fsm_state));
    fsm = net->states;
    laststate = -1;
    for (i=0; ;i++) {
      io_gets(infile, buf);
        if (buf[0] == '#') break;

        /* scanf is just too slow here */

        //items = sscanf(buf, "%i %i %i %i %i",&lineint[0], 
        //&lineint[1], &lineint[2], &lineint[3], &lineint[4]);

        items = explode_line(buf, &lineint[0]);

        switch (items) {
        case 2:
            (fsm+i)->state_no = laststate;
            (fsm+i)->in = lineint[0];
            (fsm+i)->out = lineint[0];
            (fsm+i)->target = lineint[1];
            (fsm+i)->final_state = last_final;
            break;
        case 3:
            (fsm+i)->state_no = laststate;
            (fsm+i)->in = lineint[0];
            (fsm+i)->out = lineint[1];
            (fsm+i)->target = lineint[2];
            (fsm+i)->final_state = last_final;
            break;
        case 4:
            (fsm+i)->state_no = lineint[0];
            (fsm+i)->in = lineint[1];
            (fsm+i)->out = lineint[1];
            (fsm+i)->target = lineint[2];
            (fsm+i)->final_state = lineint[3];
            laststate = lineint[0];
            last_final = lineint[3];
            break;
        case 5:
            (fsm+i)->state_no = lineint[0];
            (fsm+i)->in = lineint[1];
            (fsm+i)->out = lineint[2];
            (fsm+i)->target = lineint[3];
            (fsm+i)->final_state = lineint[4];
            laststate = lineint[0];
            last_final = lineint[4];
            break;
        default:
            printf("File format error\n");
            return NULL;
        }
        if (laststate > 0) {
            (fsm+i)->start_state = 0;
        } else if (laststate == -1) {
            (fsm+i)->start_state = -1;
        } else {
            (fsm+i)->start_state = 1;
        }

    }
    if (strcmp(buf, "##cmatrix##") == 0) {
        cmatrix_init(net);
        cm = net->medlookup->confusion_matrix;
        for (;;) {
      io_gets(infile, buf);
            if (buf[0] == '#') break;
            sscanf(buf,"%i", &i);
            *cm = i;
            cm++;
        }     
    }
    if (strcmp(buf, "##end##") != 0) {
        printf("File format error!\n");
        return NULL;
    }
    return(net);
}

static int io_gets(FILE *infile, char *target) {
    int i;
    int c = getc(infile);
    for (i = 0; c != '\n' && c != '\0'; i++) {
        *(target+i) = c;
    c = getc(infile);
    }   
    *(target+i) = '\0';
    if (c == '\0')
      ungetc(c, infile);
    return(i);
}

static inline int explode_line (char *buf, int *values) {

    int i, j, items;
    j = i = items = 0;
    for (;;) {
        for (i = j; *(buf+j) != ' ' && *(buf+j) != '\0'; j++) { }
        if (*(buf+j) == '\0') {
            *(values+items) = atoi(buf+i);
            items++;
            break;
        } else{
            *(buf+j) = '\0';
            *(values+items) = atoi(buf+i);
            items++;
            j++;
            i = j;
        }
    }
    return(items);
}

    int FomaTransducer::write_net(fsm * net, FILE * outfile) {

    struct sigma *sigma;
    struct fsm_state *fsm;
    int i, maxsigma, laststate, *cm;

    /* Header */
    fprintf(outfile, "%s","##foma-net 1.0##\n");

    /* Properties */
    fprintf(outfile, "%s","##props##\n");
    fprintf(outfile, "%i %i %i %i %i %lld %i %i %i %i %i %i %s\n",
            net->arity, 
            net->arccount, 
            net->statecount, 
            net->linecount, 
            net->finalcount, 
            net->pathcount, 
            net->is_deterministic, 
            net->is_pruned, 
            net->is_minimized, 
            net->is_epsilon_free, 
            net->is_loop_free, 
            net->is_completed, 
            net->name);
    
    /* Sigma */
    fprintf(outfile, "%s","##sigma##\n");
    for (sigma = net->sigma; sigma != NULL && sigma->number != -1; 
         sigma = sigma->next) {
        fprintf(outfile, "%i %s\n",sigma->number, sigma->symbol);
    }

    /* State array */
    laststate = -1;
    fsm = net->states;
    fprintf(outfile, "%s","##states##\n");
    for (fsm = net->states; fsm->state_no !=-1; fsm++) {
        if (fsm->state_no != laststate) {
            if (fsm->in != fsm->out) {
                fprintf(outfile, "%i %i %i %i %i\n",
                        fsm->state_no, fsm->in, fsm->out, fsm->target, 
                        fsm->final_state);
            } else {
                fprintf(outfile, "%i %i %i %i\n",fsm->state_no, fsm->in, 
                        fsm->target, fsm->final_state);
            }
        } else {
            if (fsm->in != fsm->out) {
                fprintf(outfile, "%i %i %i\n", fsm->in, fsm->out, fsm->target);
            } else {
                fprintf(outfile, "%i %i\n", fsm->in, fsm->target);
            }
        }
        laststate = fsm->state_no;
    }
    /* Sentinel for states */
    fprintf(outfile, "-1 -1 -1 -1 -1\n");

    /* Store confusion matrix */
    if (net->medlookup != NULL && net->medlookup->confusion_matrix != NULL) {

        fprintf(outfile, "%s","##cmatrix##\n");
        cm = net->medlookup->confusion_matrix;
        maxsigma = sigma_max(net->sigma)+1;
        fprintf(outfile, "maxsigma is: %i\n",maxsigma);
        for (i=0; i < maxsigma*maxsigma; i++) {
            fprintf(outfile, "%i\n", *(cm+i));
        }
    }

    /* End */
    fprintf(outfile, "%s","##end##\n");
    return(1);

    }

  } }

#else // MAIN_TEST was defined
#include <cstdlib>
#include <cassert>
#include <iostream>
using namespace hfst::implementations;

int main(int argc, char * argv[]) 
{
    std::cout << "Unit tests for " __FILE__ ":";
    std::cout << std::endl << "ok" << std::endl;
    return EXIT_SUCCESS;
}
#endif // MAIN_TEST
