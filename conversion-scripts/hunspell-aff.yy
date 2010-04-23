%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <set>

extern int yylineno;
void yyerror(const char* text);
int yylex();

static FILE* lexcfile = stdout;
static FILE* twolcfile = stdout;

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
        else if (c & (128 + 64 + 32 + 16) == (128 + 64 + 32 + 16))
          { 
            u8len = 4;
          }
        else if (c & (128 + 64 + 32) == (128 + 64 + 32))
          {
            u8len = 3;
          }
        else if (c & (128 + 64) == (128 + 64))
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


%}

%error-verbose
%locations

%union
{
    char* string;
    unsigned long number;
}

%token <number> COUNT CONTNUM REP_COUNT AM_COUNT AF_COUNT COMPOUNDMIN_LINE
%token <string> WORD CONTSTRING SET_LINE TRY_LINE FLAG_LINE KEY_LINE 
                UNKNOWN_LINE
%token ERROR REP_LEADER SFX_LEADER PFX_LEADER COMPOUNDFLAG_LEADER 
    COMPOUNDBEGIN_LEADER COMPOUNDMIDDLE_LEADER COMPOUNDLAST_LEADER
    CIRCUMFIX_LEADER

%%

AFF_FILE: SECTIONLIST 
        ;

SECTIONLIST: SECTIONLIST SECTION 
        | SECTION
        ;

SECTION: SET_LINE {
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
            free($1);
            }
        | REPSECTION
        | PFXSECTION 
        | SFXSECTION
        | AFSECTION
        | AMSECTION
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
    if (argc > 1)
    {
        if ((strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
        {
            printf("Usage: %s [INFILE [OUTFILE]]\n", argv[0]);
            return EXIT_SUCCESS;
        }
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            fprintf(stderr, "Could not open %s for reading\n", argv[1]);
            return EXIT_FAILURE;
        }
        if (argc > 2)
        {
            lexcfile = fopen(argv[2], "w");
            if (lexcfile == NULL)
            {
                fprintf(stderr, "Could not open %s for writing\n", argv[2]);
                return EXIT_FAILURE;
            }
        }
    }
    fprintf(lexcfile, "! This file contains hunspell %s dictionary "
                      "automatically translated to lexc format\n"
                      "LEXICON Start\n", argv[1]);
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
