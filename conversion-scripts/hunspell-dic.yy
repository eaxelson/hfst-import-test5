%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <set>

extern int yylineno;
void yyerror(const char* text);
int yylex();

enum flagtypes {BYTE_FLAG, WORD_FLAG, UTF8_FLAG, NUM_FLAG, AF_FLAG} flagtype;

static FILE* lexcfile = stdout;
static unsigned long expected_strings = 0;
static unsigned long words_read = 0;

bool
strCmp(char* s, char* q)
{
    if (s == q)
    {
        return true;
    }
    else if ((s == 0) || (q == 0))
    {
        return false;
    }
    else
    {
        return strcmp(s, q);
    }
}

static std::set<char*,bool(*)(char*,char*)> alphabets(&strCmp);

static
void
gather_alphabet(const char* word)
{
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
        alphabets.insert(nextu8);
        free(nextu8);
        s += u8len;
      }
}

static
char*
escape_word(const char* word)
{
    char* rv = static_cast<char*>(calloc(sizeof(char), strlen(word)*2+1));
    const char* s = word;
    char* p = rv;
    while (*s != '\0')
    {
        if ((*s == '!') || (*s == ' ') || (*s == '%'))
        {
            *p = '%';
            p++;
        }
        *p = *s;
        p++;
        s++;
    }
    *p = '\0';
    return rv;
}

static
unsigned long
parse_flag(char** flagstring)
{
    unsigned long flagnumber = 0;
    switch (flagtype)
    {
    case BYTE_FLAG:
        flagnumber = static_cast<unsigned int>(**flagstring);
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
handle_wordline(const char* word, const char* conts)
{
    char* flags = strdup(conts);
    words_read++;
    char* escaped_word = escape_word(word);
    fprintf(lexcfile, "%s\tHUNSPELL_FIN\t;\n", escaped_word);
    unsigned long flag = 0;
    while (*flags != '\0')
      {
        flag = parse_flag(&flags);
        if (flagtype == AF_FLAG)
        {
            fprintf(lexcfile, "%s\tHUNSPELL_AF_%lu\t;\n", escaped_word, flag);
        }
        else
        {
            fprintf(lexcfile, "%s\tHUNSPELL_FLAG_%lu\t;\n", escaped_word, flag);
        }
      }
    free(escaped_word);
}

static
void
handle_wordline(const char* word, unsigned long cont)
{
    words_read++;
    char* escaped_word = escape_word(word);
    fprintf(lexcfile, "%s\tHUNSPELL_AF_%lu\t;\n", escaped_word, cont);
    free(escaped_word);
}

static
void
handle_numbered_wordline(const char* word, unsigned long cont,
                         unsigned long number)
{
    static bool error_said = false;
    if (!error_said)
      {
        fprintf(stderr, "AM number %lu ignored\n", number);
        error_said = true;
      }
    handle_wordline(word, cont);
}

static
void
handle_numbered_wordline(const char* word, const char* conts,
                         unsigned long number)
{
    static bool error_said = false;
    if (!error_said)
      {
        fprintf(stderr, "AM number %lu ignored\n", number);
        error_said = true;
      }
    handle_wordline(word, conts);
}

static
void
handle_stringed_wordline(const char* word, const char* conts,
                         const char* extra)
{
    static bool error_said = false;
    if (!error_said)
      {
        fprintf(stderr, "Extra strings at the end of line?\n");
        error_said = true;
      }
    handle_wordline(word, conts);
}

%}

%error-verbose
%locations

%union
{
    char* string;
    unsigned long number;
}

%token <number> COUNT EXTRA_NUMBER
%token <string> WORD EXTRA_STRING CONT_THING
%token ERROR

%%

DIC_FILE: COUNT WORDLIST {
        if (words_read > $1)
        {
            printf("%lu/%lu words read? "
                   "Normally this would indicate an error, "
                   "but in hunspell it seems quite common\n", words_read, $1);
        }
        else if (words_read < $1)
        {
            printf("%lu/%lu words read? "
                   "Check which entries are missing, "
                   "and add them by hand!\n", words_read, $1);
        }
        else
        {
            printf("%lu/%lu words read! "
                   "Everything worked out nicely!\n", words_read, $1);
        }
        expected_strings = $1;
        }
        ;

WORDLIST: WORDLIST WORDLINE 
        | WORDLINE
        ;

WORDLINE: WORD CONT_THING {
            handle_wordline($1, $2);
            free($1);
            free($2);
            }
        | WORD CONT_THING EXTRA_STRING {
            handle_stringed_wordline($1, $2, $3);
            free($1);
            free($2);
            free($3);
        }
        | WORD CONT_THING EXTRA_NUMBER {
            handle_numbered_wordline($1, $2, $3);
            free($1);
            free($2);
        }
        | WORD {
            handle_wordline($1, "");
            free($1);
        }
        | WORD EXTRA_NUMBER {
            handle_numbered_wordline($1, "", $2);
            free($1);
        }
        | CONT_THING {
            handle_wordline("", $1);
            free($1);
        }
        | COUNT {
            char* snum = static_cast<char*>(malloc(sizeof(char)*65));
            sprintf(snum, "%lu", $1);
            handle_wordline(snum, "");
            free(snum);
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
    if (argc > 1)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
        {
            printf("Usage: %s [FLAGSTYLE [INFILE [OUTFILE]]]\n", argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[1], "ASCII") == 0)
        {
            printf("Parsing ASCII FLAGS\n");
            flagtype = BYTE_FLAG;
        }
        else if (strcmp(argv[1], "LONG") == 0)
        {
            printf("Parsing WORD FLAGS\n");
            flagtype = WORD_FLAG;
        }
        else if (strcmp(argv[1], "UTF-8") == 0)
        {
            printf("Parsing UTF-8 FLAGS\n");
            flagtype = UTF8_FLAG;
        }
        else if (strcmp(argv[1], "NUM") == 0)
        {
            printf("Parsing NUM FLAGS\n");
            flagtype = NUM_FLAG;
        }
        else if (strcmp(argv[1], "AF") == 0)
        {
            flagtype = AF_FLAG;
        }
        else
        {
            printf("Incorrect flag type %s\n", argv[1]);
        }
    }
    if (argc > 2)
    {
        infilename = strdup(argv[2]);
        yyin = fopen(infilename, "r");
        if (yyin == NULL)
        {
            fprintf(stderr, "Could not open %s for reading\n", infilename);
            return EXIT_FAILURE;
        }
    }
    else
    {
        infilename = strdup("<stdin>");
    }
    if (argc > 3)
    {
        lexcfilename = strdup(argv[3]);
        lexcfile = fopen(lexcfilename, "w");
        if (lexcfile == NULL)
        {
            fprintf(stderr, "Could not open %s for writing\n", lexcfilename);
            return EXIT_FAILURE;
        }
    }
    else
    {
        lexcfilename = strdup("<stdout>");
    }
    printf("Reading from %s, writing to %s\n", infilename, lexcfilename);
    fprintf(lexcfile, "! This file contains hunspell %s dictionary "
                      "automatically translated to lexc format\n\n"
                      "LEXICON HUNSPELL_dic_root\n"
                      "\tHUNSPELL_dic\t;\n"
                      "\tHUNSPELL_pfx\t;\n\n"
                      "LEXICON HUNSPELL_dic\n", infilename);
    yyparse();
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
