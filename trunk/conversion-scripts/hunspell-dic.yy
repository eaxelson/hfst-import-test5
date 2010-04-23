%{
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <set>

extern int yylineno;
void yyerror(const char* text);
int yylex();

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

static
void
handle_wordline(const char* word, const char* conts)
{
    const char* cont = conts;
    words_read++;
    while (*cont != '\0')
      {
        fprintf(lexcfile, "%s\tHUNBYTE%u\t;\n", word, *cont);
        cont++;
      }
}

static
void
handle_wordline(const char* word, unsigned long cont)
{
    words_read++;
    fprintf(lexcfile, "%s\tHUNNUM%lu\t;\n", word, cont);
}

static
void
handle_numbered_wordline(const char* word, unsigned long cont,
                         unsigned long number)
{
    static bool error_said = false;
    if (!error_said)
      {
        fprintf(stderr, "Extra numbers at the end of line?\n");
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
        fprintf(stderr, "Extra numbers at the end of line?\n");
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

%token <number> COUNT EXTRA_NUMBER CONTNUM
%token <string> WORD EXTRA_STRING CONTSTRING
%token ERROR

%%

DIC_FILE: COUNT WORDLIST {
        printf("%lu/%lu words read\n", words_read, $1);
        expected_strings = $1;
        }
        ;

WORDLIST: WORDLIST WORDLINE 
        | WORDLINE
        ;

WORDLINE: WORD CONTSTRING {
            handle_wordline($1, $2);
            free($1);
            free($2);
            }
        | WORD CONTNUM {
            handle_wordline($1, $2);
            free($1);
        }
        | WORD CONTNUM EXTRA_NUMBER {
            handle_numbered_wordline($1, $2, $3);
            free($1);
            }
        | WORD CONTSTRING EXTRA_STRING {
            handle_stringed_wordline($1, $2, $3);
            free($1);
            free($2);
            free($3);
        }
        | WORD {
            handle_wordline($1, "");
            free($1);
        }
        | WORD EXTRA_NUMBER {
            handle_numbered_wordline($1, "", $2);
            free($1);
        }
        | CONTNUM {
            handle_wordline("", $1);
        }
        | CONTSTRING {
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
