/***************************************************************************
 * This program is Copyright (C) 1986, 1987, 1988 by Jonathan Payne.  JOVE *
 * is provided to you without charge, and with no warranty.  You may give  *
 * away copies of JOVE, including sources, provided that this notice is    *
 * included in all the files.                                              *
 ***************************************************************************/

/* jove.h header file to be included by EVERYONE */

#include <setjmp.h>
#ifndef TUNED
# include "tune.h"
#endif
#if !defined(MAC)
# include <sys/types.h>
# include <string.h>
#else
# include <types.h>
#endif

#if defined(__STDC__) || defined(LINT_ARGS)
# define proto(x)        x
#else
# define proto(x)        ()
#endif

#define EOF	-1

/* typedef structure definitions */
#ifdef IPROCS
typedef struct process	Process;
#endif
typedef struct window	Window;
typedef struct position	Bufpos;
typedef struct mark	Mark;
typedef struct buffer	Buffer;
typedef struct line	Line;
typedef struct iobuf	IOBUF;

#include "buf.h"
#include "wind.h"
#include "io.h"
#if defined(IPROCS)
# include "iproc.h"
#endif
#include "dataobj.h"
#include "keymaps.h"
#include "argcount.h"
#include "util.h"
#include "vars.h"
#include "screen.h"
#include "style.h"

/* return codes for command completion (all < 0 because >= 0 are
   legitimate offsets into array of strings */

#define AMBIGUOUS	-2	/* matches more than one at this point */
#define UNIQUE		-3	/* matches only one string */
#define ORIGINAL	-4	/* matches no strings at all! */
#define NULLSTRING	-5	/* just hit return without typing anything */

/* values for the `flags' argument to complete */
#define NOTHING		0	/* opposite of RET_STATE */
#define RET_STATE	1	/* return state when we hit return */
#define RCOMMAND	2	/* we are reading a joverc file */
#define CASEIND		4	/* map all to lower case */

#define FORWARD		1
#define BACKWARD	-1

#define ARG_CMD		1
#define LINECMD		2
#define KILLCMD		3	/* so we can merge kills */
#define YANKCMD		4	/* so we can do ESC Y (yank-pop) */

extern jmp_buf	mainjmp;

/* setjmp/longjmp args for DoKeys() mainjmp */
#define FIRSTCALL	0
#define ERROR		1
#define COMPLAIN	2	/* do the error without a getDOT */
#define QUIT		3	/* leave this level of recursion */

#define QUIET		1	/* sure, why not? */

#define YES_NODIGIT	2

#define INT_OKAY	0
#define INT_BAD		-1

extern char	NullStr[];
extern char	*ProcFmt;

extern int
	InMacDefine,	/* are we defining a macro right now? */

	LastKeyStruck,

	TOabort,	/* flag set by Typeout() */
	errormsg,	/* last message was an error message
			   so don't erase the error before it
			   has been read */
	RecDepth,	/* recursion depth */
	InputPending,	/* nonzero if there is input waiting to
			   be processed */

	InJoverc,
	Interactive,

	Crashing,	/* we are in the middle of crashing */
	Asking,		/* are we on read a string from the terminal? */
	InRealAsk,	/* are we currently executing real_ask()? */
	inIOread;	/* so we know whether we can do a redisplay. */

extern char	Minibuf[LBSIZE];

#define MESG_SIZE 128
extern char	mesgbuf[MESG_SIZE];

#define CATCH \
{\
	jmp_buf	sav_jmp; \
\
	push_env(sav_jmp); \
	if (setjmp(mainjmp) == 0) {

#define ONERROR \
	} else { \

#define ENDCATCH \
	} \
	pop_env(sav_jmp); \
}

#include "externs.h"
