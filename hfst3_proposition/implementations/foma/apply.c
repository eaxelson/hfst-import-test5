/*     Foma: a finite-state toolkit and library.                             */
/*     Copyright © 2008-2009 Mans Hulden                                     */

/*     This file is part of foma.                                            */

/*     Foma is free software: you can redistribute it and/or modify          */
/*     it under the terms of the GNU General Public License version 2 as     */
/*     published by the Free Software Foundation. */

/*     Foma is distributed in the hope that it will be useful,               */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*     GNU General Public License for more details.                          */

/*     You should have received a copy of the GNU General Public License     */
/*     along with foma.  If not, see <http://www.gnu.org/licenses/>.         */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "foma.h"

#define RANDOM 1
#define ENUMERATE 2
#define MATCH 4
#define UP 8
#define DOWN 16
#define LOWER 32
#define UPPER 64
#define SPACE 128

#define FAIL 0
#define SUCCEED 1

#define DEFAULT_OUTSTRING_SIZE 2048
#define DEFAULT_LIMIT 1000
#define DEFAULT_STACK_SIZE 128

struct sigmatch {
    int signumber ;
    int consumes;
    struct sigmatch *next;
};

struct searchstack {
    int offset;
    int opos;
    int ipos;
    int zeroes;
    char *flagname;
    char *flagvalue;
    int flagneg;
};

struct flag_lookup {
    int type;
    char *name;
    char *value;
};

struct flag_list {
    char *name;
    char *value;
    short neg;
    struct flag_list *next;
};

extern int g_obey_flags;
extern int g_show_flags;
extern int g_print_space;
extern int g_print_pairs;

static int ptr, ipos, opos, zeroes, mode, printcount, *statemap = NULL, *marks = NULL, has_flags, apply_stack_ptr, apply_stack_top, oldflagneg, outstringtop, iterate_old = 0, sigmatch_size, iterator = 0;
static char *outstring, *instring = NULL, **sigs, *oldflagvalue;

static struct fsm *last_net = NULL;
static struct fsm_state *gstates = NULL;
static struct sigma *gsigma;
static struct flag_list *flag_list = NULL, *flist;
static struct flag_lookup *flag_lookup = NULL;
static struct sigmatch *sigmatch = NULL;
static struct searchstack *searchstack = NULL;

static int apply_append(int cptr, int sym);
static char *apply_net();
static void apply_create_statemap(struct fsm *net);
static void apply_create_sigarray(struct fsm *net);
static void apply_create_sigmatch();
static int apply_match_str(int symbol, int position);
static void apply_add_flag(char *name);
static int apply_check_flag(int type, char *name, char *value);
static void apply_clear_flags();

static void apply_stack_clear();
static int apply_stack_isempty();
static void apply_stack_pop ();
static void apply_stack_push (int sptr, int sipos, int sopos, int szeroes, char *sflagname, char *sflagvalue, int sflagneg);

char *apply_enumerate() {

    char *result = NULL;

    if (last_net == NULL || last_net->finalcount == 0)
        return (NULL);
    
    if (result != NULL)
        xxfree(result);

    if (iterator == 0) {
        iterate_old = 0;
        apply_create_sigmatch();
        result = apply_net();
        iterator++;
    } else {
        iterate_old = 1;
        result = apply_net();
    }
    return(result);
}

char *apply_words(struct fsm *net) {
    mode = DOWN + ENUMERATE + LOWER + UPPER;
    return(apply_enumerate());
}


char *apply_upper_words() {
    mode = DOWN + ENUMERATE + UPPER;
    return(apply_enumerate());
}

char *apply_lower_words() {
    mode = DOWN + ENUMERATE + LOWER;
    return(apply_enumerate());
}

char *apply_random_words() {
    apply_clear_flags();
    mode = DOWN + ENUMERATE + LOWER + UPPER + RANDOM;
    return(apply_enumerate());
}

char *apply_random_lower() {
    apply_clear_flags();
    mode = DOWN + ENUMERATE + LOWER + RANDOM;    
    return(apply_enumerate());
}

char *apply_random_upper() {
    apply_clear_flags();
    mode = DOWN + ENUMERATE + UPPER + RANDOM;
    return(apply_enumerate());
}

void apply_reset_iterator() {
    iterator = 0;
}

void apply_free_sigmatch() {
    struct sigmatch *sigm, *sigp;
    int i;
    if (sigmatch == NULL)
        return;
    for (i=0; i < sigmatch_size;i++) {

        sigm = (sigmatch+i)->next;
        while (sigm != NULL) {
            sigp = sigm;
            sigm = sigm->next;
            xxfree(sigp);
        }
    }
    xxfree(sigmatch);
    sigmatch = NULL;
    sigmatch_size = 0;
}

/* Frees memory associated with applies */
void apply_clear() {
    if (statemap != NULL) {
        xxfree(statemap);
        statemap = NULL;
    }
    if (sigmatch != NULL) {
        apply_free_sigmatch();
    }
    if (marks != NULL) {
        xxfree(marks);
        marks = NULL;
    }
    if (searchstack != NULL) {
        xxfree(searchstack);
        searchstack = NULL;
    }
    if (sigs != NULL) {
        xxfree(sigs);
        sigs = NULL;
    }
    if (flag_lookup != NULL) {
        xxfree(flag_lookup);
        flag_lookup = NULL;
    }
    last_net = NULL;
    apply_reset_iterator();
}

char *apply_updown(char *word) {

    char *result = NULL;

    if (last_net == NULL || last_net->finalcount == 0)
        return (NULL);
    
    if (result != NULL) {
        xxfree(result);
        result = NULL;
    }

    if (word == NULL) {
        iterate_old = 1;
        result = apply_net();
    }
    else if (word != NULL) {
        iterate_old = 0;
        if (instring != NULL) {
            xxfree(instring);
        }
        instring = xxstrdup(word);
        apply_create_sigmatch();
        result = apply_net();
    }
    return(result);
}

char *apply_down(char *word) {

    mode = DOWN;    
    return(apply_updown(word));

}

char *apply_up(char *word) {

    mode = UP;    
    return(apply_updown(word));

}

void apply_init(struct fsm *net) {

    srand((unsigned int) time(NULL));

    apply_reset_iterator();
    last_net = net;

    if (outstring != NULL)
        xxfree(outstring);

    outstring = xxmalloc(sizeof(char)*DEFAULT_OUTSTRING_SIZE);
    outstringtop = DEFAULT_OUTSTRING_SIZE;
    *outstring = '\0';
    gstates = net->states;
    gsigma = net->sigma;
    printcount = 1;
    apply_create_statemap(net);
    if (searchstack == NULL) {
	searchstack = xxmalloc(sizeof(struct searchstack) * DEFAULT_STACK_SIZE);
	apply_stack_top = DEFAULT_STACK_SIZE;
    }
    apply_create_sigarray(net);
}

int apply_stack_isempty () {
    if (apply_stack_ptr == 0) {
	return 1;
    }
    return 0;
}

void apply_stack_clear () {
    apply_stack_ptr = 0;
}

void apply_stack_pop () {
    apply_stack_ptr--;
    ptr = (searchstack+apply_stack_ptr)->offset;
    ipos = (searchstack+apply_stack_ptr)->ipos;
    opos = (searchstack+apply_stack_ptr)->opos;    
    zeroes = (searchstack+apply_stack_ptr)->zeroes;
    if (has_flags && (searchstack+apply_stack_ptr)->flagname != NULL) {
	/* Restore flag */
	for (flist = flag_list; flist != NULL; flist = flist->next) {
	    if (strcmp(flist->name, (searchstack+apply_stack_ptr)->flagname) == 0) {
		break;
	    }	    
	}
	if (flist == NULL)
	    perror("***Nothing to pop");
	flist->value = (searchstack+apply_stack_ptr)->flagvalue;
	flist->neg = (searchstack+apply_stack_ptr)->flagneg;
    }
}

static void apply_stack_push (int sptr, int sipos, int sopos, int szeroes, char *sflagname, char *sflagvalue, int sflagneg) {
    if (apply_stack_ptr == apply_stack_top) {
	searchstack = xxrealloc(searchstack, sizeof(struct searchstack)* (apply_stack_top*2));
	apply_stack_top *= 2;
    }
    (searchstack+apply_stack_ptr)->offset     = sptr;
    (searchstack+apply_stack_ptr)->ipos       = sipos;
    (searchstack+apply_stack_ptr)->opos       = opos;
    (searchstack+apply_stack_ptr)->zeroes     = szeroes;
    (searchstack+apply_stack_ptr)->flagname   = sflagname;
    (searchstack+apply_stack_ptr)->flagvalue  = sflagvalue;
    (searchstack+apply_stack_ptr)->flagneg    = sflagneg;

    apply_stack_ptr++;
    
}

char *apply_net() {

    char *fname, *fvalue;
    int curr_ptr, eatupi, eatupo, symin, symout, f, fneg; 
    int m;
    
/*     We perform a basic DFS on the graph, with two minor complications:       */

/*     1. We keep a mark for each state which indicates whether it is seen      */
/*        on the current "run."  If we reach a marked state, we terminate.      */
/*        As we pop a position, we also unmark the state we came from.          */
/*        If we're matching against a string, we terminate iff the mark         */
/*        is set and ipos is the same as last time we saw the flag, i.e.        */
/*        we've traversed a loop without consuming an input symbol              */
 
/*     2. If the graph has flags, we push the previous flag value when          */
/*        traversing a flag-modifying arc.  This is because a flag may          */
/*        may have been set during the previous "run" and may not apply.        */
/*        Since we're doing a DFS, we can be sure to return to the previous     */
/*        global flag state by just remembering that last flag change.          */

    
    if (iterate_old == 1)
        goto resume;

    ptr = 0; ipos = 0; opos = 0; zeroes = 0;
    apply_stack_clear();

    /* "The use of four-letter words like goto can occasionally be justified */
    /*  even in the best of company." Knuth (1974). */

    goto L2;

    while(!apply_stack_isempty()) {
	
	apply_stack_pop();

	if ((gstates+ptr)->state_no != (gstates+ptr+1)->state_no) {
	    *(marks+(gstates+ptr)->state_no) = -1;
	    continue;
	}
	ptr++;

	/* Follow arc & push old position */

    L1:
	for (curr_ptr = ptr, f = 0; (gstates+curr_ptr)->state_no == (gstates+ptr)->state_no && (gstates+curr_ptr)-> in != -1; curr_ptr++) {

	    symin  = ((mode&DOWN) == DOWN) ? (gstates+curr_ptr)->in  : (gstates+curr_ptr)->out;
	    symout = ((mode&DOWN) == DOWN) ? (gstates+curr_ptr)->out : (gstates+curr_ptr)->in;
	    
	    if ((eatupi = apply_match_str(symin, ipos)) != -1) {

		eatupo = apply_append(curr_ptr, symout);
		if (g_obey_flags && has_flags && ((flag_lookup+symin)->type &(FLAG_UNIFY|FLAG_CLEAR|FLAG_POSITIVE|FLAG_NEGATIVE))) {
		    fname = flist->name;
		    fvalue = oldflagvalue;
		    fneg = oldflagneg;
                    
/* 		    printf("Pushing: [%s][%s][%i]\n",fname,fvalue,fneg); */
		} else {
		    fname = fvalue = NULL;
		    fneg = 0;
		}

                apply_stack_push(curr_ptr, ipos, opos, zeroes, fname, fvalue, fneg);

                if (eatupi == 0 && ((gstates+curr_ptr)->target == (gstates+curr_ptr)->state_no))
		    zeroes++;
		f = 1;
                ptr = *(statemap+(gstates+curr_ptr)->target);
		ipos = ipos+eatupi;
		opos = opos+eatupo;
		break;
	    }
	}

	/* There were no more edges on this vertex? */
	if (!f) {
	    /* Unmark, break */
	    *(marks+(gstates+ptr)->state_no) = -1;
	    continue;
	}
	
	/* Print accumulated string */

    L2:
	if ((gstates+ptr)->final_state == 1 && (((mode & ENUMERATE) == ENUMERATE) || (ipos == strlen(instring)))) {
            /* Stick a 0 to endpos to avoid getting old accumulated gunk strings printed */
            *(outstring+opos) = '\0';
	    if ((mode & RANDOM) == RANDOM) {
		if (rand() % 2) {
                    return(strdup(outstring));
		}
            } else {
                //printf("printing [%s]\n",outstring);
                return(strdup(outstring));
            }
        }

    resume:
	
	/* Check Mark */
	m = *(marks+(gstates+ptr)->state_no);
	
	/* If we're randomly generating strings, we randomly decide whether or not */
	/* to obey the restriction against looping */

	if (m != -1 && (((mode & RANDOM)) == RANDOM)) {
	    if (!(rand() % 2)) {
		continue;
	    }
	
	} else if ((m != -1)  && ((mode & ENUMERATE) == ENUMERATE)) {
	    continue;
   	} else if (m != -1 && (mode & ENUMERATE) != ENUMERATE && zeroes > (strlen(instring)+1)) { 
  	    continue; 
        }

	/* Mark */
	*(marks+(gstates+ptr)->state_no) = 1;

	*(marks+(gstates+ptr)->state_no) = ipos;

	goto L1;
    }

    apply_stack_clear();
    return NULL;
}

int apply_append (int cptr, int sym) {

    char *astring, *bstring, *pstring;
    int symin, symout, len, alen, blen;

    symin = (gstates+cptr)->in;
    symout = (gstates+cptr)->out;
    astring = *(sigs+symin);
    bstring = *(sigs+symout);

    if (symin == UNKNOWN) 
        astring = "?";
    if (symout == UNKNOWN)
        bstring = "?";
    if (symin == IDENTITY)
        astring = "@";
    if (symout == IDENTITY)
        bstring = "@";
    if (symin == EPSILON)
        astring = "0";
    if (symout == EPSILON)
        bstring = "0";

    alen = strlen(astring);
    blen = strlen(bstring);

    while (alen + blen + opos + 3 >= outstringtop) {
	outstring = xxrealloc(outstring, sizeof(char) * (outstringtop * 2));
	outstringtop *= 2;
    }

    if (has_flags && !g_show_flags && (flag_lookup+symin)->type) {
	astring = ""; alen = 0;
    }
    if (has_flags && !g_show_flags && (flag_lookup+symout)->type) {
	bstring = ""; blen = 0;
    }
    if ((mode & ENUMERATE) == ENUMERATE) {
	/* Print both sides separated by colon */
	/* if we're printing "words" */
	if ((mode & (UPPER | LOWER)) == (UPPER|LOWER)) {

	    if (astring == bstring) {
		strcpy(outstring+opos, astring);
		len = alen;
	    } else {
		strcpy(outstring+opos, astring);
		strcpy(outstring+opos+alen,":");
		strcpy(outstring+opos+alen+1,bstring);
		len = alen+blen+1;
	    }
	}

	/* Print one side only */
	if ((mode & (UPPER|LOWER)) != (UPPER|LOWER)) {

	    if (symin == EPSILON) {
		astring = ""; alen = 0;
	    }
	    if (symout == EPSILON) {
		bstring = ""; blen = 0;
	    }
	    if ((mode & (UPPER|LOWER)) == UPPER) {
		pstring = astring; 
		len = alen;
	    } else {
		pstring = bstring;
		len = blen;
	    }
	    strcpy(outstring+opos, pstring);
	}
    }
    if ((mode & ENUMERATE) != ENUMERATE) {
	/* Print pairs is ON and symbols are different */
	if (g_print_pairs && (symin != symout)) {

	    if (symin == UNKNOWN && (mode & DOWN) == DOWN)
		strncpy(astring, instring+ipos, 1);
	    if (symout == UNKNOWN && (mode & UP) == UP)
		strncpy(bstring, instring+ipos, 1);
	    strcpy(outstring+opos, "<");
	    strcpy(outstring+opos+1, astring);
	    strcpy(outstring+opos+alen+1,":");
	    strcpy(outstring+opos+alen+2,bstring);
	    strcpy(outstring+opos+alen+blen+2,">");
	    len = alen+blen+3;
	}

	else if (sym == IDENTITY) {
	    /* Apply up/down */
	    strncpy(outstring+opos, instring+ipos, 1);
	    strncpy(outstring+opos+1,"", 1);
	    len = 1;
	} else if (sym == EPSILON) {
	    return(0);
	} else {
	    if ((mode & DOWN) == DOWN) {
		pstring = bstring;
		len = blen;
	    } else {
		pstring = astring;
		len = alen;
	    }
	    strcpy(outstring+opos, pstring);
	}
    }
    if (g_print_space && len > 0) {
	strcpy(outstring+opos+len, " ");
	len++;
    }
    return(len);
}

/* Match a symbol from sigma against the current position in string */
/* Return the number of symbols consumed in input string */
/* For flags, we consume 0 symbols of the input string, naturally */

int apply_match_str(int symbol, int position) {
    struct sigmatch *sigm;
    if ((mode & ENUMERATE) == ENUMERATE) {
	if (has_flags && (flag_lookup+symbol)->type) {
	    if (!g_obey_flags) {
		return 0;
	    }
	    if (apply_check_flag((flag_lookup+symbol)->type, (flag_lookup+symbol)->name, (flag_lookup+symbol)->value) == SUCCEED) {
		return 0;
	    } else {
		return -1;
	    }
	    
	}
	return (strlen(*(sigs+symbol)));
    }
    
    /* If symbol is a flag, we need to check consistency */

    if (has_flags && (flag_lookup+symbol)->type) {
	if (!g_obey_flags) {
	    return 0;
	}
	if (apply_check_flag((flag_lookup+symbol)->type, (flag_lookup+symbol)->name, (flag_lookup+symbol)->value) == SUCCEED) {
/* 	    printf("CF: [%i][%s][%s]", (flag_lookup+symbol)->type, (flag_lookup+symbol)->name, (flag_lookup+symbol)->value); */
	    return 0;
	} else {
	    return -1;
	}
    }

    for (sigm = sigmatch+position; sigm != NULL; sigm = sigm->next) {
	if (sigm->signumber == symbol)
	    return (sigm->consumes);
    }
    return -1;
}

void apply_create_statemap(struct fsm *net) {
    int i;
    struct fsm_state *fsm;
    fsm = net->states;
    if (statemap != NULL)
        xxfree(statemap);
    statemap = xxmalloc(sizeof(int)*net->statecount);
    if (marks != NULL)
        xxfree(marks);

    marks = xxmalloc(sizeof(int)*net->statecount);

    for (i=0; i<net->statecount; i++) {
	*(statemap+i) = -1;
	*(marks+i) = -1;
    }
    for (i=0; (fsm+i)->state_no != -1; i++) {
	if (*(statemap+(fsm+i)->state_no) == -1) {
	    *(statemap+(fsm+i)->state_no) = i;
	}
    }
}

void apply_create_sigarray(struct fsm *net) {
    struct sigma *sig;
    struct fsm_state *fsm;
    
    int i, maxsigma;
    
    fsm = net->states;

    maxsigma = sigma_max(net->sigma);

    if (sigs != NULL)
        xxfree(sigs);
    
    sigs = xxmalloc(sizeof(char **)*(maxsigma+1));
    has_flags = 0;
    flag_list = NULL;

    for (sig = gsigma; sig != NULL ; sig = sig->next) {
	if (flag_check(sig->symbol)) {
	    has_flags = 1;
	    apply_add_flag(flag_get_name(sig->symbol));
/* 	    printf("Isflag: [%s]\n",sig->symbol); */
/* 	    printf("Type: [%i]\n",flag_get_type(sig->symbol)); */
/* 	    printf("Name: [%s]\n",flag_get_name(sig->symbol)); */
/* 	    printf("Value: [%s]\n",flag_get_value(sig->symbol)); */
	}
	*(sigs+(sig->number)) = xxstrdup(sig->symbol);
    }

    if (has_flags) {
        if (flag_lookup != NULL)
            xxfree(flag_lookup);

	flag_lookup = xxmalloc(sizeof(struct flag_lookup)*(maxsigma+1));
	for (i=0; i <= maxsigma; i++) {
	    (flag_lookup+i)->type = 0;
	    (flag_lookup+i)->name = NULL;
	    (flag_lookup+i)->value = NULL;
	}
	for (sig = gsigma; sig != NULL ; sig = sig->next) {
	    if (flag_check(sig->symbol)) {
		(flag_lookup+sig->number)->type = flag_get_type(sig->symbol);
		(flag_lookup+sig->number)->name = flag_get_name(sig->symbol);
		(flag_lookup+sig->number)->value = flag_get_value(sig->symbol);		
	    }
	}
    }
}

/* We need to know which symbols in sigma we can match for all positions */
/* in the input string.  Alternatively, if there is no input string as is the case */
/* when we just list the words or randomly search the graph, we can match */
/* any symbol in sigma. */

/* We create an array that points to a linked list of all the symbols we can match */
/* at any given position of the input string, together with the information about */
/* how many symbols we consume if we do match */

void apply_create_sigmatch() {
    struct sigma *sig;
    struct sigmatch *tmp_sig = NULL;
    int i, thismatch;

    if ((mode & ENUMERATE) == ENUMERATE) { return; }

    /* We create a sigmatch array only in case we match against a real string */

    if (sigmatch != NULL) {
        apply_free_sigmatch();
    }

    sigmatch_size = strlen(instring)+1;
    sigmatch = xxmalloc(sizeof(struct sigmatch)*sigmatch_size);

    /* We can always match EPSILON */
    for (i=0; i <= strlen(instring); i++) {
	(sigmatch+i)->signumber = EPSILON;
	(sigmatch+i)->consumes = 0;
	(sigmatch+i)->next = NULL;
    }

    for (i=0; i <= strlen(instring); i++) {
	thismatch = 0;
	for (sig = gsigma; sig != NULL ; sig = sig->next) {
	    if (sig->number == EPSILON || sig->number == UNKNOWN) {
		continue;
	    }
	    else if (strncmp((instring+i), sig->symbol, strlen(sig->symbol)) == 0) {
		/* add to list */
		if (thismatch == 0)
		    thismatch = 1;
		tmp_sig = xxmalloc(sizeof(struct sigmatch));
		tmp_sig->next = (sigmatch+i)->next;
		(sigmatch+i)->next = tmp_sig;
		
		tmp_sig->signumber = sig->number;
		tmp_sig->consumes = strlen(sig->symbol);
	    }
	}
	if (thismatch == 0 && i < strlen(instring)) {
	    /* Add ? to the list */
	    tmp_sig = xxmalloc(sizeof(struct sigmatch));
	    tmp_sig->next = (sigmatch+i)->next;
	    tmp_sig->signumber = UNKNOWN;
	    tmp_sig->consumes = 1;
	    (sigmatch+i)->next = tmp_sig;
	    tmp_sig = xxmalloc(sizeof(struct sigmatch));
	    tmp_sig->next = (sigmatch+i)->next;
	    tmp_sig->signumber = IDENTITY;
	    tmp_sig->consumes = 1;
	    (sigmatch+i)->next = tmp_sig;
	}
    }
}

void apply_add_flag(char *name) {
    struct flag_list *flist, *flist_prev;
    if (flag_list == NULL) {
	flist = flag_list = xxmalloc(sizeof(struct flag_list));
    } else {
	for (flist = flag_list; flist != NULL; flist_prev = flist, flist = flist->next) {
	    if (strcmp(flist->name, name) == 0) {
		return;
	    }
	}
	flist = xxmalloc(sizeof(struct flag_list));
	flist_prev->next = flist;
    }
    flist->name = name;
    flist->value = NULL;
    flist->neg = 0;
    flist->next = NULL;
    return;
}

void apply_clear_flags() {
    struct flag_list *flist;
    for (flist = flag_list; flist != NULL; flist = flist->next) {
	flist->value = NULL;
	flist->neg = 0;
    }
    return;
}

/* Check for flag consistency by looking at the current states of */
/* flags in flist */

int apply_check_flag(int type, char *name, char *value) {
    struct flag_list *flist2;
    for (flist = flag_list; flist != NULL; flist = flist->next) {
	if (strcmp(flist->name, name) == 0) {
	    break;
	}
    }
    oldflagvalue = flist->value;
    oldflagneg = flist->neg;

    if (type == FLAG_UNIFY) {
	if (flist->value == NULL) {
	    flist->value = xxstrdup(value);
	    return SUCCEED;
	}
	else if (strcmp(value, flist->value) == 0 && flist->neg == 0) {
	    return SUCCEED;	    
	} else if (strcmp(value, flist->value) != 0 && flist->neg == 1) {
	    flist->value = xxstrdup(value);
	    flist->neg = 0;
	    return SUCCEED;
	}  
	return FAIL;
    }

    if (type == FLAG_CLEAR) {
	flist->value = NULL;
	flist->neg = 0;
	return SUCCEED;
    }

    if (type == FLAG_DISALLOW) {
	if (value == NULL && flist->value == NULL) {
	    return SUCCEED;
	}
	if (value != NULL && flist->value == NULL) {
	    return FAIL;
	}
	if (value == NULL && flist->value != NULL) {
	    return FAIL;
	}
	if (strcmp(value, flist->value) != 0) {
            if (flist->neg == 1)
                return FAIL;
            return SUCCEED;
	}
	if (strcmp(value, flist->value) == 0 && flist->neg == 1) {
            return SUCCEED;
        }
        return FAIL;
    }

    if (type == FLAG_NEGATIVE) {
	flist->value = value;
	flist->neg = 1;
	return SUCCEED;
    }

    if (type == FLAG_POSITIVE) {
	flist->value = value;
	flist->neg = 0;
	return SUCCEED;
    }

    if (type == FLAG_REQUIRE) {
	if (value == NULL) {
	    if (flist->value == NULL) {
		return FAIL;
	    } else {
		return SUCCEED;
	    }
	} else {
	    if (flist->value == NULL) {
		return FAIL;
	    }
	    if (strcmp(value, flist->value) != 0) {
		return FAIL;
	    } else {
                if (flist->neg == 1) {
                    return FAIL;
                }
		return SUCCEED;
	    }
	}       	
    }

    if (type == FLAG_EQUAL) {
	for (flist2 = flag_list; flist2 != NULL; flist2 = flist2->next) {
	    if (strcmp(flist2->name, value) == 0) {
		break;
	    }
	}

	if (flist2 == NULL && flist->value != NULL)
	    return FAIL;
	if (flist2 == NULL && flist->value == NULL) {
	    return SUCCEED;
	}
	if (flist2->value == NULL || flist->value == NULL) {
	    if (flist2->value == NULL && flist->value == NULL && flist->neg == flist2->neg) {
		return SUCCEED;
	    } else {
		return FAIL;
	    }
	}  else if (strcmp(flist2->value, flist->value) == 0 && flist->neg == flist2->neg) {
	    return SUCCEED;
	}
	return FAIL;	
    }
    printf("***Don't know what do with flag [%i][%s][%s]\n", type, name, value);
    return FAIL;
}
