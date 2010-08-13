%{
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <set>
#include <map>
#include <string>
#define YYDEBUG 0

using std::set;
using std::string;
using std::map;

extern int yylineno;
void yyerror(const char* text);
int yylex();

static FILE* lexcfile = stdout;
static FILE* twolcfile = stdout;
static FILE* errfile = stdout;
static FILE* symfile = stdout;
static FILE* twolsymfile = stdout;

enum flagtypes {BYTE_FLAG, WORD_FLAG, UTF8_FLAG, NUM_FLAG, AF_FLAG} flagtype = BYTE_FLAG;
bool has_afs = false;

static char* compound_flag;
static char* compound_begin;
static char* compound_middle;
static char* compound_end;
static unsigned int compound_parts = 0;

static set<string> sigma;
static set<string> pi;

static unsigned long pfx_read = 0;
static unsigned long sfx_read = 0;
static unsigned long af_read = 0;
static unsigned long rep_read = 0;

static bool has_key = false;

static bool lexicon_prefixes_written = false;
static set<long unsigned int> contexts_written;
static set<long unsigned int> lexicons_written;
static map<string,set<string> > deletion_contexts;
static map<string,set<string> > lexicon_contexts;

static set<string> needed_flags;

static
set<string>*
gather_alphabet(const char* word)
{
    set<string>* alphabets = new set<string>;
    unsigned short u8len = 0;
    const char *s = word;
    while (*s != '\0')
      {
        unsigned char c = static_cast<unsigned char>(*s);
        if (c <= 127)
          {
            u8len = 1;
          }
        else if ((c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16))
          { 
            u8len = 4;
          }
        else if ((c & (128 + 64 + 32)) == (128 + 64 + 32))
          {
            u8len = 3;
          }
        else if ((c & (128 + 64)) == (128 + 64))
          {
            u8len = 2;
          }
        else
          {
            fprintf(stderr, "Suspicious UTF8 %s in parser?\n", word);
          }
        char* nextu8 = static_cast<char*>(calloc(sizeof(char), u8len+1));
        memcpy(nextu8, s, u8len);
        nextu8[u8len] = '\0';
        alphabets->insert(nextu8);
        free(nextu8);
        s += u8len;
    }
    return alphabets;
}

static
unsigned long
parse_flag(char** flagstring, flagtypes flagtype)
{
    unsigned long flagnumber = 0;
    switch (flagtype)
    {
    case BYTE_FLAG:
        flagnumber = static_cast<unsigned int>(static_cast<unsigned char>(**flagstring));
        *flagstring += 1;
        break;
    case WORD_FLAG:
        memcpy(&flagnumber, *flagstring, 2);
        *flagstring += 2;
        break;
    case UTF8_FLAG:
      {
        unsigned char c = static_cast<unsigned char>(**flagstring);
        size_t u8len = 1;
        if (c <= 127)
          {
            u8len = 1;
          }
        else if ((c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16))
          { 
            u8len = 4;
          }
        else if ((c & (128 + 64 + 32)) == (128 + 64 + 32))
          {
            u8len = 3;
          }
        else if ((c & (128 + 64)) == (128 + 64))
          {
            u8len = 2;
          }
        else
          {
            fprintf(stderr, "Suspicious UTF8 %s in parser?\n", *flagstring);
          }
        char* nextu8 = static_cast<char*>(calloc(sizeof(char), u8len+1));
        memcpy(nextu8, *flagstring, u8len);
        nextu8[u8len] = '\0';
        memcpy(&flagnumber, nextu8, u8len);
        *flagstring += u8len;
        break;
      }
    case NUM_FLAG:
      {
        char* endptr = *flagstring;
        flagnumber = strtoul(*flagstring, &endptr, 10);
        if (*endptr == '\0')
        {
            *flagstring = endptr;
        }
        else if (*endptr == ',')
        {
            *flagstring = endptr+1;
        }
        else
        {
            fprintf(stderr, "Suspicious numstring %s in parser?\n", *flagstring);
            *flagstring = endptr;
        }
        break;
      }
    case AF_FLAG:
      {
        char* endptr = *flagstring;
        flagnumber = strtoul(*flagstring, &endptr, 10);
        if (*endptr == '\0')
        {
            *flagstring = endptr;
        }
        else
        {
            fprintf(stderr, "Suspicious afstring %s in parser?\n", *flagstring);
            *flagstring = endptr;
        }
        break;
      }
    default:
        fprintf(stderr, "DATOISSA VIRHE END\n");
        break;
    }
    return flagnumber;
}
static
void
handle_setline(const char* setname)
{
    if (strcmp(setname, "UTF-8") != 0)
    {
        fprintf(stderr, "SETs other than UTF-8 are not supported! "
               "Use recode or iconv\n");
    }
}

static
void
handle_flagline(const char* flagname)
{
    if (strcmp(flagname, "long") == 0)
    {
        printf("Parsing WORD FLAGS\n");
        flagtype = WORD_FLAG;
    }
    else if (strcmp(flagname, "UTF-8") == 0)
    {
        printf("Parsing UTF-8 FLAGS\n");
        flagtype = UTF8_FLAG;
    }
    else if (strcmp(flagname, "num") == 0)
    {
        printf("Parsing NUM FLAGS\n");
        flagtype = NUM_FLAG;
    }
    else
    {
        fprintf(stderr, "Unknown FLAG type %s\n", flagname);
    }
}

static
void
handle_keyline(const char* keysets)
{
    char* ks = strdup(keysets);
    char* ed_set = strtok(ks, "|");
    fprintf(errfile, "LEXICON HUNSPELL_KEY\n");
    while (ed_set != NULL)
    {
        printf("KEY confusion set: %s\n", ed_set);
        set<string>* ed_chars = gather_alphabet(ed_set);
        for (set<string>::const_iterator s1 = ed_chars->begin();
             s1 != ed_chars->end();
             ++s1)
        {
            for (set<string>::const_iterator s2 = ed_chars->begin();
                 s2 != ed_chars->end();
                 ++s2)
            {
                if (*s1 != *s2)
                {
                    fprintf(errfile, "%s:%s\tHUNSPELL_error_Sigma_1\t\"weight: 100\"\t;\n",
                            s1->c_str(), s2->c_str());
                    fprintf(errfile, "%s:%s\tHUNSPELL_error_Sigma_1\t\"weight: 100\"\t;\n",
                            s2->c_str(), s1->c_str());
                }
            }
        }
        ed_set = strtok(NULL, "|");
    }
    has_key = true;
}

static
void
handle_tryline(const char* tryset)
{
    printf("TRY confusion set: %s\n", tryset);
    set<string>* ed_chars = gather_alphabet(tryset);
    fprintf(errfile, "LEXICON HUNSPELL_TRY\n");
    for (set<string>::const_iterator s1 = ed_chars->begin();
         s1 != ed_chars->end();
         ++s1)
    {
        fprintf(errfile, "%s:0\tHUNSPELL_error_Sigma_1\t\"weight: 1000\"\t;\n",
                s1->c_str());
        fprintf(errfile, "0:%s\tHUNSPELL_error_Sigma_1\t\"weight: 1000\"\t;\n",
                s1->c_str());
        for (set<string>::const_iterator s2 = ed_chars->begin();
             s2 != ed_chars->end();
             ++s2)
        {
            if (*s1 != *s2)
            {
                fprintf(errfile, "%s:%s\tHUNSPELL_error_Sigma_1\t\"weight: 1000\"\t;\n",
                        s1->c_str(), s2->c_str());
                fprintf(errfile, "%s:%s\tHUNSPELL_error_Sigma_1\t\"weight: 1000\"\t;\n",
                        s2->c_str(), s1->c_str());
            }
        }
    }
}

static
void
handle_repline(const char* orig, const char* repl)
{
    static bool lexicon_written = false;
    static double error_weight = 0;
    if (!lexicon_written)
    {
        fprintf(errfile, "LEXICON HUNSPELL_REP\n");
        lexicon_written = true;
    }
    fprintf(errfile, "%s:%s\tHUNSPELL_error_Sigma_1\t\"weight: %f\"\t;\n",
            orig, repl, error_weight);
    error_weight += 0.333;
    rep_read++;
}

static
void
handle_af_data(const char* conts)
{
    has_afs = true;
    char* s = strdup(conts);
    static unsigned long af_count = 1;
    fprintf(lexcfile, "LEXICON HUNSPELL_AF_%ld\n", af_count);
    af_count++;
    while (*s != '\0')
    {
        unsigned long d = parse_flag(&s, flagtype);
        fprintf(lexcfile, "\tHUNSPELL_FLAG_%lu\t;\n", d);
    }
    fprintf(lexcfile, "\n");
    af_read++;
}

static
void
write_prefix_lexicon_entry(unsigned long cont, const char* morph,
                           unsigned long next_cont)
{
    if (!lexicon_prefixes_written)
    {
        fprintf(lexcfile, "LEXICON HUNSPELL_pfx\n");
        lexicon_prefixes_written = true;
    }
    char* incoming_prefix_flag = static_cast<char*>(malloc(sizeof(char)*strlen("@P.NEEDFLAG12345678901234567890.ON@0")));
    char* need_flag = static_cast<char*>(malloc(sizeof(char)*strlen("@P.NEEDFLAG12345678901234567890@0")));
    char* deletion_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("{12345678901234567890%%<}0")));
    sprintf(incoming_prefix_flag, "@P.NEEDFLAG%lu.ON@", cont);
    sprintf(need_flag, "@D.NEEDFLAG%lu@", cont);
    sprintf(deletion_context_tag, "{%lu%%<}", cont);
    sigma.insert(incoming_prefix_flag);
    sigma.insert(deletion_context_tag);
    sigma.insert(need_flag);
    needed_flags.insert(need_flag);
    if (next_cont != 0)
    {
        char* outgoing_prefix_flag = static_cast<char*>(malloc(sizeof(char)*strlen("@P.NEEDPREFIX12345678901234567890@0")));
        sprintf(outgoing_prefix_flag, "@P.NEEDPREFIX%lu@", next_cont);
        sigma.insert(outgoing_prefix_flag);
        fprintf(lexcfile, "%s%s%s%s\tHUNSPELL_pfx\t;\n",
                morph, incoming_prefix_flag, outgoing_prefix_flag,
                deletion_context_tag);
    }
    else
    {
        fprintf(lexcfile, "%s%s%s\tHUNSPELL_dic_root\t;\n",
                morph, incoming_prefix_flag, deletion_context_tag);
    }
}

// extend removal by context if necessary
static
char*
overlay_twol_contexts(const char* remove, const char* context)
{
    const char *s = context;
    const char *r = remove;
    bool context_any = false;
    bool remove_none = false;
    if (s == NULL)
    {
        context_any = true;
        s = strdup("");
    }
    else if ((strcmp(context, ".") == 0) || (strlen(context) == 0))
    {
        context_any = true;
    }
    if (r == NULL)
    {
        remove_none = true;
        r = strdup("");
    }
    if (remove_none && context_any)
    {
        return strdup("");
    }
    else if (remove_none)
    {
        return strdup(context);
    }
    else if (context_any)
    {
        return strdup(remove);
    }
    else
    {
        // join context by copying maximum span of removes and continuing by
        // context
        char* joint_context = static_cast<char*>(calloc(sizeof(char),(strlen(context)*3 + strlen(remove)*3 + 1)));
        char* j = static_cast<char*>(calloc(sizeof(char),(strlen(context)*3 + strlen(remove)* + 1)));
        bool was_colon = false;
        const char* r_start = r;
        const char* s_start = s;
        const char* j_start = j;
        // start from the end
        while (*r != '\0')
        {
            r++;
        }
        while (*s != '\0')
        {
            s++;
        }
        while (r < r_start)
        {
            *j = *r;
            j++;
            if (*r == ':')
            {
                was_colon = false;
            }
            else if (*r == '0')
            {
                was_colon = true;
            }
            r--;
            if (!was_colon && (s < s_start))
            {
                s--;
            }
        }
        while (s < s_start)
        {
            *j = *s;
            j++;
            s--;
        }
        char* rv = joint_context;
        while (j < j_start)
        {
            *rv = *j;
            j--;
            rv++;
        }
        return joint_context;
    }
    assert(false);
    return NULL;
}

static
char*
twolize_context(const char* context, bool in_removes = false)
{
    const char* s = context;
    if (context == NULL)
    {
        return strdup("");
    }
    char* twol_context = static_cast<char*>(calloc(sizeof(char),strlen(context)*4+1));
    char* next_pair = twol_context;
    unsigned short u8len = 0;
    bool in_bracket = false;
    bool first_in_bracket = false;
    while (*s != '\0')
    {
        unsigned char c = static_cast<unsigned char>(*s);
        if (c == 0)
        {
            u8len = 0;
            fprintf(stderr, "Unexpected end of string parsing %s\n", context);
            break;
        }
        else if (c <= 127)
          {
            u8len = 1;
          }
        else if ((c & (128 + 64 + 32 + 16)) == (128 + 64 + 32 + 16))
          { 
            u8len = 4;
          }
        else if ((c & (128 + 64 + 32)) == (128 + 64 + 32))
          {
            u8len = 3;
          }
        else if ((c & (128 + 64)) == (128 + 64))
          {
            u8len = 2;
          }
        else
          {
            fprintf(stderr, "Suspicious UTF8 %s in parser?\n", s);
          }
        bool do_not_print_caret_please = false;
        if (in_bracket && (*s == ']')) {
            in_bracket = false;
        }
        else if (first_in_bracket && (*s == '^'))
        {
            *next_pair = '\\';
            next_pair++;
            *next_pair = '[';
            next_pair++;
            do_not_print_caret_please = true;
        }
        else if (in_bracket && !first_in_bracket)
        {
            *next_pair = '|';
            next_pair++;
            *next_pair = ' ';
            next_pair++;
        }
        first_in_bracket = false;
        if (!in_bracket && (*s == '['))
        {
            in_bracket = first_in_bracket = true;
        }
        if (!do_not_print_caret_please)
        {
            if ((u8len == 1) && ((*s == '-')))
            {
                *next_pair = '%';
                next_pair++;
            }
            memcpy(next_pair, s, u8len);
        }
        next_pair += u8len;
        if (!in_bracket)
        {
            *next_pair = ':';
            next_pair++;
            if (in_removes)
            {
                *next_pair = '0';
                next_pair++;
            }
        }
        *next_pair = ' ';
        next_pair++;
        s += u8len;
    }
    next_pair = '\0';
    return twol_context;
}

// for each deleted character, add possible contexts of deletion in form of
// [ADDED MORPH] [CONTEXT TAG] [LEFT CONTEXT] _ [RIGHT CONTEXT] ;
static
void
save_prefix_deletion_contexts(unsigned long cont, const char* remove,
                              const char* morph, const char* match)
{
    char* deletion_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%<%%}0")));
    sprintf(deletion_context_tag, "%%{%lu%%<%%}", cont);
    char* deleted_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%<%%}:00")));
    sprintf(deleted_context_tag, "%%{%lu%%<%%}:0", cont);
    pi.insert(deleted_context_tag);
    char* twol_context_del = twolize_context(remove, true);
    char* twol_context_match = twolize_context(match);
    char* twol_context_add = twolize_context(morph);
    char* twol_context_delmatch = overlay_twol_contexts(twol_context_del, twol_context_match);
    char* twol_context_del_dup = strdup(twol_context_del);
    char* context_pair = strtok(twol_context_del_dup, " ");
    while (context_pair)
    {
        if (strstr(context_pair, ":0") != NULL)
        {
            char* cur_pos = strstr(twol_context_del, context_pair);
            char* rightc = static_cast<char*>(malloc(sizeof(char)*strlen(twol_context_del)));
            char* leftc = static_cast<char*>(malloc(sizeof(char)*strlen(twol_context_del)));
            leftc = static_cast<char*>(memcpy(leftc, twol_context_del, cur_pos-twol_context_del));
            leftc[cur_pos-twol_context_del] = '\0';
            rightc = strdup(cur_pos+strlen(context_pair));
            char* deletion_context = static_cast<char*>(malloc(1024));
            sprintf(deletion_context, "%s %s %s _ %s ;",
                    twol_context_add, deleted_context_tag,
                    leftc, rightc);
            // keep only unique contexts
            set<string> dcs = deletion_contexts[context_pair];
            dcs.insert(deletion_context);
            deletion_contexts[context_pair] = dcs;
            // save deletion to twol alphabet
            pi.insert(context_pair);
            // also save id pair to twol alphabet
            char* ident = strdup(context_pair);
            char* colon = strstr(ident, ":");
            if (colon != NULL)
            {
                *colon = '\0';
            }
            pi.insert(ident);
        }
        context_pair = strtok(NULL, " ");
    }
    char* morph_to_pi = strdup(twol_context_add);
    char* nupi = strtok(morph_to_pi, " ");
    while (nupi != NULL)
    {
        char* ident = strdup(nupi);
        char* colon = strstr(ident, ":");
        if (colon != NULL)
        {
            *colon = '\0';
        }
        if (*ident == '-')
        {
            pi.insert("%-");
        }
        else
        {
            pi.insert(ident);
        }
        nupi = strtok(NULL, " ");
    }
}

static
void
handle_pfx_line(const char* cont, 
               const char* remove, const char* add, const char* match)
{
    char* s = strdup(cont);
    unsigned long contnum = parse_flag(&s, flagtype);
    if (*s != '\0')
    {
        fprintf(stderr, "Junk at end of flag %s using current flagtype\n",
                cont);
    }
    write_prefix_lexicon_entry(contnum, add, 0);
    save_prefix_deletion_contexts(contnum, remove, add, match);
    pfx_read++;
}

static
void
handle_pfx_line_with_conts(const char* cont,
               const char* remove, const char* add, const char* conts,
                const char* match)
{
    // once without continuation
    handle_pfx_line(cont, remove, add, match);
    // once for each class
    char* s = strdup(cont);
    unsigned long contnum = parse_flag(&s, flagtype);
    char *c = strdup(conts);
    while (*c != '\0')
    {
        unsigned long nextcontnum = 0;
        if (has_afs)
        {
            nextcontnum = parse_flag(&c, AF_FLAG);
        }
        else
        {
            nextcontnum = parse_flag(&c, flagtype);
        }
        write_prefix_lexicon_entry(contnum, add, nextcontnum);
        save_prefix_deletion_contexts(contnum, remove, add, match);
    }
}

static
void
handle_stringed_pfx_line(const char* cont,
                        const char* remove, const char* add, const char* match,
                        const char* extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Extra junk at end of line: %s\n",
                extra);
        warned = true;
    }
    handle_pfx_line(cont, remove, add, match);
}

static
void
handle_stringed_pfx_line_with_conts(const char* cont,
                        const char* remove, const char* add, const char* conts,
                        const char* match, const char* extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Extra junk at end of line: %s\n",
                extra);
        warned = true;
    }
    handle_pfx_line_with_conts(cont, remove, add, conts, match);
}

static
void
handle_numbered_pfx_line(const char* cont,
                        const char* remove, const char* add, const char* match,
                        unsigned long extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Morphology AM stuff not supported: %lu\n",
                extra);
        warned = true;
    }
    handle_pfx_line(cont, remove, add, match);
}

static
void
handle_numbered_pfx_line_with_conts(const char* cont,
                        const char* remove, const char* add, const char* conts,
                        const char* match, unsigned long extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Morphology AM stuff not supported: %lu\n",
                extra);
        warned = true;
    }
    handle_pfx_line_with_conts(cont, remove, add, conts, match);
}

static
void
write_suffix_lexicon_entry(unsigned long cont, const char* morph,
                           unsigned long nextcont)
{
    // if someone's jumping back and forth between PFX SFX PFX, rewrite LEXICON
    lexicon_prefixes_written = false;
    if (lexicons_written.find(cont) == lexicons_written.end())
    {
        fprintf(lexcfile, "LEXICON HUNSPELL_FLAG_%lu\n", cont);
        lexicons_written.insert(cont);
    }
    char* incoming_suffix_flag = static_cast<char*>(malloc(sizeof(char)*strlen("@C.NEEDFLAG12345678901234567890@0")));
    char* deletion_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("{%%>12345678901234567890}0")));
    sprintf(incoming_suffix_flag, "@C.NEEDFLAG%lu@", cont);
    sprintf(deletion_context_tag, "{%%>%lu}", cont);
    sigma.insert(incoming_suffix_flag);
    sigma.insert(deletion_context_tag);
    if (nextcont != 0)
    {
        if (has_afs)
        {
            fprintf(lexcfile, "%s%s\tHUNSPELL_AF_%lu\t;\n",
                deletion_context_tag, morph,
                nextcont);
        }
        else
        {
            fprintf(lexcfile, "%s%s\tHUNSPELL_FLAG_%lu\t;\n",
                deletion_context_tag, morph,
                nextcont);
        }
    }
    else
    {
        fprintf(lexcfile, "%s%s\tHUNSPELL_FIN\t;\n",
                deletion_context_tag, morph);
    }
}

// for each deleted character, add possible contexts of deletion in form of
// [LEFT CONTEXT] _ [RIGHT CONTEXT] [CONTEXT TAG] [ADDED MORPh] ;
static
void
save_suffix_deletion_contexts(unsigned long cont, const char* remove,
                              const char* morph, const char* match)
{
    char* deletion_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%>%%}0")));
    sprintf(deletion_context_tag, "%%{%%>%lu%%}", cont);
    char* deleted_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%>%%}:00")));
    sprintf(deleted_context_tag, "%%{%%>%lu%%}:0", cont);
    pi.insert(deleted_context_tag);
    char* twol_context_del = twolize_context(remove, true);
    char* twol_context_match = twolize_context(match);
    char* twol_context_add = twolize_context(morph);
    char* twol_context_delmatch = overlay_twol_contexts(twol_context_del, twol_context_match);
    char* twol_context_del_dup = strdup(twol_context_del);
    char* context_pair = strtok(twol_context_del_dup, " ");
    while (context_pair)
    {
        if (strstr(context_pair, ":0") != NULL)
        {
            char* cur_pos = strstr(twol_context_del, context_pair);
            char* rightc = static_cast<char*>(malloc(sizeof(char)*strlen(twol_context_del)));
            char* leftc = static_cast<char*>(malloc(sizeof(char)*strlen(twol_context_del)));
            leftc = static_cast<char*>(memcpy(leftc, twol_context_del, cur_pos-twol_context_del));
            leftc[cur_pos-twol_context_del] = '\0';
            rightc = strdup(cur_pos+strlen(context_pair));
            char* deletion_context = static_cast<char*>(malloc(1024));
            sprintf(deletion_context, "%s _ %s %s %s ;",
                    leftc, rightc, deleted_context_tag,
                    twol_context_add);
            // keep only unique contexts
            set<string> dcs = deletion_contexts[context_pair];
            dcs.insert(deletion_context);
            deletion_contexts[context_pair] = dcs;
            // save deletion to twol alphabet
            pi.insert(context_pair);
            // also save id pair to twol alphabet
            char* ident = strdup(context_pair);
            char* colon = strstr(ident, ":");
            if (colon != NULL)
            {
                *colon = '\0';
            }
            pi.insert(ident);
        }
        context_pair = strtok(NULL, " ");
    }
    char* morph_to_pi = strdup(twol_context_add);
    char* nupi = strtok(morph_to_pi, " ");
    while (nupi != NULL)
    {
        char* ident = strdup(nupi);
        char* colon = strstr(ident, ":");
        if (colon != NULL)
        {
            *colon = '\0';
        }
        if (*ident == '-')
        {
            pi.insert("%-");
        }
        else
        {
            pi.insert(ident);
        }
        nupi = strtok(NULL, " ");
    }
}

static
void
save_suffix_context(unsigned long cont, const char* remove,
                      const char* morph, const char* match)
{
    char* deletion_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%>%%}0")));
    sprintf(deletion_context_tag, "%%{%%>%lu%%}", cont);
    char* deleted_context_tag = static_cast<char*>(malloc(sizeof(char)*strlen("%%{12345678901234567890%%>%%}:00")));
    sprintf(deleted_context_tag, "%%{%%>%lu%%}:0", cont);
    pi.insert(deleted_context_tag);
    char* twol_context_del = twolize_context(remove, true);
    char* twol_context_match = twolize_context(match);
    char* twol_context_add = twolize_context(morph);
    char* twol_context_delmatch = overlay_twol_contexts(twol_context_del, twol_context_match);
    size_t i = strlen(twol_context_del);
    size_t j = strlen(twol_context_match);
    size_t k = strlen(twol_context_delmatch);
    //fprintf(stderr, "WAH %s %zu + %s %zu = %s %zu\n", twol_context_del,
      //  i, twol_context_match, j, twol_context_delmatch, k);
    size_t l = strlen(twol_context_add);
 //   fprintf(stderr, "Aah %s %zu\n", twol_context_add, l);
    char* lexicon_context = static_cast<char*>(calloc(sizeof(char),k+l+1000));
    sprintf(lexicon_context, "%s _ %s ;\n", twol_context_delmatch, twol_context_add);
    set<string> dcs = lexicon_contexts[deleted_context_tag];
    dcs.insert(lexicon_context);
    lexicon_contexts[deleted_context_tag] = dcs;
}

static
void
handle_sfx_line(const char* cont, 
               const char* remove, const char* add, const char* match)
{
    char* s = strdup(cont);
    unsigned long contnum = parse_flag(&s, flagtype);
    if (*s != '\0')
    {
        fprintf(stderr, "Junk at end of flag %s using current flagtype\n",
                cont);
    }
    write_suffix_lexicon_entry(contnum, add, 0);
    save_suffix_deletion_contexts(contnum, remove, add, match);
    save_suffix_context(contnum, remove, add, match);
    sfx_read++;
}

static
void
handle_sfx_line_with_conts(const char* cont, 
               const char* remove, const char* add, const char* conts,
                const char* match)
{
    // once without continuation
    handle_sfx_line(cont, remove, add, match);
    // once for each class
    char* s = strdup(cont);
    unsigned long contnum = parse_flag(&s, flagtype);
    if (*s != '\0')
    {
        fprintf(stderr, "Junk at end of flag %s using current flagtype\n",
                cont);
    }
    char* c = strdup(conts);
    while ( *c != '\0')
    {
        unsigned long nextcontnum = 0;
        if (has_afs)
        {
            nextcontnum = parse_flag(&c, AF_FLAG);
        }
        else
        {
            nextcontnum = parse_flag(&c, flagtype);
        }
        write_suffix_lexicon_entry(contnum, add, nextcontnum);
        save_suffix_deletion_contexts(contnum, remove, add, match);
        save_suffix_context(contnum, remove, add, match);
    }
}

static
void
handle_stringed_sfx_line(const char* cont,
                        const char* remove, const char* add, const char* match,
                        const char* extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Extra junk at end of line: %s\n",
                extra);
        warned = true;
    }
    handle_sfx_line(cont, remove, add, match);
}

static
void
handle_stringed_sfx_line_with_conts(const char* cont,
                        const char* remove, const char* add, const char* conts,
                        const char* match, const char* extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Extra junk at end of line: %s\n",
                extra);
        warned = true;
    }
    handle_sfx_line_with_conts(cont, remove, add, conts, match);
}

static
void
handle_numbered_sfx_line(const char* cont,
                        const char* remove, const char* add, const char* match,
                        unsigned long extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Morphology AM stuff not supported: %lu\n",
                extra);
        warned = true;
    }
    handle_sfx_line(cont, remove, add, match);
}

static
void
handle_numbered_sfx_line_with_conts(const char* cont,
                        const char* remove, const char* add, const char* conts,
                        const char* match, unsigned long extra)
{
    static bool warned = false;
    if (!warned)
    {
        fprintf(stderr, "Morphology AM stuff not supported: %lu\n",
                extra);
        warned = true;
    }
    handle_sfx_line_with_conts(cont, remove, add, conts, match);
}

static
void
handle_unknownline(const char* line)
{
    fprintf(stderr, "skipped: %60s\n", line);
}

%}

%error-verbose
%locations

%union
{
    char* string;
    unsigned long number;
}

%token <number> COUNT REP_COUNT AM_COUNT AF_COUNT CONTNUMBER
                COMPOUNDMIN_LINE PFX_FIRSTLINE SFX_FIRSTLINE
%token <string> WORD CONT_THING SET_LINE TRY_LINE FLAG_LINE KEY_LINE 
                UNKNOWN_LINE UTF8_STRING BYTESTRING CONT LANG_LINE NAME_LINE 
                UTF8_ERRSTRING PFX_LEADER SFX_LEADER
                AF_LINE
                COMPOUNDFLAG_LINE 
                COMPOUNDBEGIN_LINE COMPOUNDMIDDLE_LINE COMPOUNDEND_LINE
                CIRCUMFIX_LINE
%token ERROR REP_LEADER AM_LINE ZERO

%%

AFF_FILE: SECTIONLIST
        ;

SECTIONLIST: SECTIONLIST SECTION
        | SECTION
        ;

SECTION: NAME_LINE {
            printf("Name was something like %s\n", $1);
        }
        | LANG_LINE {
            printf("Lang hacks are not handled: %s\n", $1);
        }
        | SET_LINE {
            handle_setline($1);
            free($1);
            }
        | TRY_LINE {
            handle_tryline($1);
            free($1);
            }
        | FLAG_LINE {
            handle_flagline($1);
            free($1);
            }
        | KEY_LINE {
            handle_keyline($1);
            free($1);
            }
        | UNKNOWN_LINE {
            handle_unknownline($1);
            }
        | COMPOUNDFLAG_LINE {
                 compound_flag = strdup($1);
        }
        | COMPOUNDBEGIN_LINE {
                 compound_begin = strdup($1);
        }
        | COMPOUNDMIDDLE_LINE {
                 compound_middle = strdup($1);
               }
        | COMPOUNDEND_LINE {
                 compound_end = strdup($1);
        }
        | COMPOUNDMIN_LINE {
                compound_parts = $1;
             }
        | CIRCUMFIX_LINE {
            /* circumfix = $1; */
        }
        | REPSECTION
        | PFXSECTIONS
        | SFXSECTIONS
        | AFSECTION
        | AMSECTION
        ;

AFSECTION: AF_COUNT AFLINES {
         printf("Read %lu/%lu affix compression lines AF\n", af_read, $1);
        }
        ;


AFLINES: AFLINES AFLINE 
       | AFLINE
       ;

AFLINE: AF_LINE {
        handle_af_data($1);
    }

AMSECTION: AM_COUNT AMLINES {
         printf("Ignoring %lu AM morphology compression lines\n", $1);
        }
        ;


AMLINES: AMLINES AMLINE
       | AMLINE
       ;

AMLINE: AM_LINE {
         /* skipping */
        }
        ;

REPSECTION: REP_COUNT REPLINES {
          printf("Read %lu/%lu replacement suggestion lines\n", rep_read, $1);
        }
        ;

REPLINES: REPLINES REPLINE
        | REPLINE
        ;

REPLINE: REP_LEADER UTF8_STRING UTF8_STRING {
       handle_repline($2, $3);
    }
    ;

SFXSECTIONS: SFXSECTIONS SFXSECTION
           | SFXSECTION
           ;

SFXSECTION: SFX_FIRSTLINE SFXLINES {
            printf("Read %lu/%lu suffixes\n", sfx_read, $1);
            sfx_read = 0;
        }
    ;

SFXLINES: SFXLINE SFXLINES
        | SFXLINE
        ;

SFXLINE: SFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_sfx_line($1, $2, $3, $4);
    }
    | SFX_LEADER UTF8_STRING ZERO UTF8_STRING {
        handle_sfx_line($1, $2, "", $4);
    }
    | SFX_LEADER ZERO UTF8_STRING UTF8_STRING {
        handle_sfx_line($1, NULL, $3, $4);
    }
    | SFX_LEADER ZERO ZERO UTF8_STRING {
        handle_sfx_line($1, NULL, "", $4);
    }
    | SFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING {
        handle_sfx_line_with_conts($1, $2, $3, $4, $5);
    }
    | SFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING {
        handle_sfx_line_with_conts($1, NULL, $3, $4, $5);
    }
    | SFX_LEADER ZERO ZERO CONT_THING UTF8_STRING {
        handle_sfx_line_with_conts($1, NULL, "", $4, $5);
    }
    | SFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING COUNT {
        handle_numbered_sfx_line($1, $2, $3, $4, $5);
    }
    | SFX_LEADER UTF8_STRING ZERO UTF8_STRING COUNT {
        handle_numbered_sfx_line($1, $2, "", $4, $5);
    }
    | SFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING COUNT {
        handle_numbered_sfx_line_with_conts($1, $2, $3, $4, $5, $6);
    }
    | SFX_LEADER ZERO UTF8_STRING UTF8_STRING COUNT {
        handle_numbered_sfx_line($1, NULL, $3, $4, $5);
    }
    | SFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING COUNT {
        handle_numbered_sfx_line_with_conts($1, NULL, $3, $4, $5, $6);
    }
    | SFX_LEADER ZERO ZERO CONT_THING UTF8_STRING COUNT {
        handle_numbered_sfx_line_with_conts($1, NULL, "", $4, $5, $6);
    }
    | SFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line($1, $2, $3, $4, $5);
    }
    | SFX_LEADER UTF8_STRING ZERO UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line($1, $2, "", $4, $5);
    }
    | SFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line_with_conts($1, $2, $3, $4, $5, $6);
    }
    | SFX_LEADER ZERO UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line($1, NULL, $3, $4, $5);
    }
    | SFX_LEADER ZERO ZERO UTF8_STRING UTF8_STRING{
        handle_stringed_sfx_line($1, NULL, "", $4, $5);
    }
    | SFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line_with_conts($1, NULL, $3, $4, $5, $6);
    }
    | SFX_LEADER ZERO ZERO CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_sfx_line_with_conts($1, NULL, "", $4, $5, $6);
    }
    ;

PFXSECTIONS: PFXSECTIONS PFXSECTION
           | PFXSECTION
           ;

PFXSECTION: PFX_FIRSTLINE PFXLINES {
            printf("Read %lu/%lu prefixes\n", pfx_read, $1);
            pfx_read = 0;
        }
    ;

PFXLINES: PFXLINE PFXLINES
        | PFXLINE ;

PFXLINE: PFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_pfx_line($1, $2, $3, $4);
    }
    | PFX_LEADER UTF8_STRING ZERO UTF8_STRING {
        handle_pfx_line($1, $2, "", $4);
    }
    | PFX_LEADER ZERO UTF8_STRING UTF8_STRING {
        handle_pfx_line($1, NULL, $3, $4);
    }
    | PFX_LEADER ZERO ZERO UTF8_STRING {
        handle_pfx_line($1, NULL, "", $4);
    }
    | PFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING {
        handle_pfx_line_with_conts($1, $2, $3, $4, $5);
    }
    | PFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING {
        handle_pfx_line_with_conts($1, NULL, $3, $4, $5);
    }
    | PFX_LEADER ZERO ZERO CONT_THING UTF8_STRING {
        handle_pfx_line_with_conts($1, NULL, "", $4, $5);
    }
    | PFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING COUNT {
        handle_numbered_pfx_line($1, $2, $3, $4, $5);
    }
    | PFX_LEADER UTF8_STRING ZERO UTF8_STRING COUNT {
        handle_numbered_pfx_line($1, $2, "", $4, $5);
    }
    | PFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING COUNT {
        handle_numbered_pfx_line_with_conts($1, $2, $3, $4, $5, $6);
    }
    | PFX_LEADER ZERO UTF8_STRING UTF8_STRING COUNT {
        handle_numbered_pfx_line($1, NULL, $3, $4, $5);
    }
    | PFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING COUNT {
        handle_numbered_pfx_line_with_conts($1, NULL, $3, $4, $5, $6);
    }
    | PFX_LEADER ZERO ZERO CONT_THING UTF8_STRING COUNT {
        handle_numbered_pfx_line_with_conts($1, NULL, "", $4, $5, $6);
    }
    | PFX_LEADER UTF8_STRING UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line($1, $2, $3, $4, $5);
    }
    | PFX_LEADER UTF8_STRING ZERO UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line($1, $2, "", $4, $5);
    }
    | PFX_LEADER UTF8_STRING UTF8_STRING CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line_with_conts($1, $2, $3, $4, $5, $6);
    }
    | PFX_LEADER ZERO UTF8_STRING UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line($1, NULL, $3, $4, $5);
    }
    | PFX_LEADER ZERO ZERO UTF8_STRING UTF8_STRING{
        handle_stringed_pfx_line($1, NULL, "", $4, $5);
    }
    | PFX_LEADER ZERO UTF8_STRING CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line_with_conts($1, NULL, $3, $4, $5, $6);
    }
    | PFX_LEADER ZERO ZERO CONT_THING UTF8_STRING UTF8_STRING {
        handle_stringed_pfx_line_with_conts($1, NULL, "", $4, $5, $6);
    }
    ;

%%
extern FILE* yyin;
int yyparse();
int yylex();

void
yyerror(const char* text)
{
    fprintf(stderr, "Parser error %s\n", text);
}

int
main(int argc, char** argv)
{
    char* infilename = 0;
    char* lexcfilename = 0;
    char* twolcfilename = 0;
    char* twolcsymfilename = 0;
    char* errfilename = 0;
    char* symfilename = 0;
    if (argc > 1)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
        {
            printf("Usage: %s [INFILE [LEXCFILE [TWOLCFILE [AUXFILES...]]]]\n",
                   argv[0]);
            return EXIT_SUCCESS;
        }
        infilename = strdup(argv[1]);
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            fprintf(stderr, "Could not open %s for reading\n", argv[1]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        infilename = strdup("<stdin>");
        yyin = stdin;
    }
    if (argc > 2)
    {
        lexcfilename = strdup(argv[2]);
        lexcfile = fopen(argv[2], "w");
        if (lexcfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", argv[2]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        lexcfilename = strdup("<stdout>");
    }
    if (argc > 3)
    {
        twolcfilename = strdup(argv[3]);
        twolcfile = fopen(argv[3], "w");
        if (twolcfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", argv[3]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        twolcfilename = strdup("<stdout>");
    }
    if (argc > 4)
    {
        errfilename = strdup(argv[5]);
        errfile = fopen(argv[4], "w");
        if (errfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", argv[4]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        errfilename = strdup("<stdout>");
    }
    if (argc > 5)
    {
        symfilename = strdup(argv[5]);
        symfile = fopen(argv[5], "w");
        if (symfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", argv[5]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        symfilename = strdup("<stdout>");
    }
    if (argc > 6)
    {
        twolcsymfilename = strdup(argv[6]);
        twolsymfile = fopen(argv[6], "w");
        if (twolsymfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", argv[6]);
            return EXIT_FAILURE;
        }
    }
    else
    {
        twolcsymfilename = strdup("<stdout>");
    }
    printf("Reading from %s, writing to %s, %s\n"
           "%s, %s and %s\n", infilename, lexcfilename, twolcfilename,
           errfilename, symfilename, twolcsymfilename);
    fprintf(lexcfile, "! This file contains hunspell %s affixes "
                      "automatically translated to lexc format\n", infilename);
    fprintf(twolcfile, "! This file contains hunspell %s affix deletion rules "
                       "automatically translated to twolc format\n", infilename);
    fprintf(twolcfile, "Rules\n");
    fprintf(twolsymfile, "! This file contains hunspell %s affix deletion rules' symbol pairs\n", infilename);
    fprintf(errfile, "! This file contains hunspell %s error models REP, KEY, "
                     " and TRY "
                     "automatically translated to lexc format\n"
                     "\n"
                     "LEXICON HUNSPELL_error_root\n"
                     "  HUNSPELL_error_Sigma_0\t;\n", infilename);

#   if YYDEBUG
    yydebug = 1;
#   endif
    sigma.insert("@C.EXPECT_SUFFIX@");
    yyparse();
    fprintf(lexcfile, "LEXICON HUNSPELL_FIN\n");
    for (set<string>::const_iterator s = needed_flags.begin();
         s != needed_flags.end();
         ++s)
    {
        fprintf(lexcfile, "%s", s->c_str());
    }
    fprintf(lexcfile, "\t#\t;\n");
    fprintf(symfile, "Multichar_Symbols\n");
    for (set<string>::const_iterator s = sigma.begin();
         s != sigma.end();
         ++s)
    {
        fprintf(symfile, "%s ", s->c_str());
        if (s->at(0) != '@')
        {
            pi.insert(*s);
        }
    }
    fprintf(errfile, "\nLEXICON HUNSPELL_error_Sigma_0\n");
    for (set<string>::const_iterator s = pi.begin();
         s != pi.end();
         ++s)
    {
        if (s->find(":") == string::npos)
        {
            fprintf(errfile, "%s\tHUNSPELL_error_Sigma_0\t;\n", s->c_str());
        }
    }
    fprintf(errfile, "\tHUNSPELL_KEY\t\"weight:1\";\t\n"
                     "\tHUNSPELL_REP\t\"weight:100\";\t\n"
                     "\tHUNSPELL_TRY\t\"weight:1000\";\t\n"
                     "\t#\t;\n");
    fprintf(errfile, "\nLEXICON HUNSPELL_error_Sigma_1\n");
    for (set<string>::const_iterator s = pi.begin();
         s != pi.end();
         ++s)
    {
        if (s->find(":") == string::npos)
        {
            fprintf(errfile, "%s\tHUNSPELL_error_Sigma_1\t;\n", s->c_str());
        }
    }
    fprintf(errfile, "\t#\t;\n");
    fprintf(symfile, "\nLEXICON Root\n\tHUNSPELL_dic_root\t;\n");
    for (map<string, set<string> >::const_iterator context_sets = deletion_contexts.begin();
        context_sets != deletion_contexts.end();
        context_sets++)
    {
        string deleted = context_sets->first;
        set<string> contexts = context_sets->second;
        fprintf(twolcfile, "\"%s deletion contexts\"\n", deleted.c_str());
        fprintf(twolcfile, "%s <=> ", deleted.c_str());
        for (set<string>::const_iterator context = contexts.begin();
             context != contexts.end();
             ++context)
        {
            fprintf(twolcfile, "%s\n\t", context->c_str());
        }
        fprintf(twolcfile, "\n");
    }
    for (map<string, set<string> >::const_iterator context_sets = lexicon_contexts.begin();
        context_sets != lexicon_contexts.end();
        context_sets++)
    {
        string deleted = context_sets->first;
        set<string> contexts = context_sets->second;
        fprintf(twolcfile, "\"%s lexicon contexts\"\n", deleted.c_str());
        fprintf(twolcfile, "%s <=> ", deleted.c_str());
        for (set<string>::const_iterator context = contexts.begin();
             context != contexts.end();
             ++context)
        {
            fprintf(twolcfile, "%s\n\t", context->c_str());
        }
        fprintf(twolcfile, "\n");
    }
    fprintf(twolsymfile, "Alphabet\n");
    for (set<string>::const_iterator s = pi.begin();
         s != pi.end();
         ++s)
    {
        fprintf(twolsymfile, "%s\n", s->c_str());
    }
    fprintf(twolsymfile, ";\n");
    if (argc > 1)
    {
        fclose(yyin);
    }
    if (argc > 2)
    {
        fclose(lexcfile);
    }
    if (yynerrs > 0)
    {
        fprintf(stderr, "There were errors\n");
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
