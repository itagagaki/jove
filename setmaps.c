/************************************************************************
 * This program is Copyright (C) 1986 by Jonathan Payne.  JOVE is       *
 * provided to you without charge, and with no warranty.  You may give  *
 * away copies of JOVE, including sources, provided that this notice is *
 * included in all the files.                                           *
 ************************************************************************/

#define TXT_TO_C

#include "funcdefs.c"
#undef putchar	/* From jove.h via funcdefs.c, conflicts with STDIO */
#undef putc
#undef getc
#undef EOF
#undef FILE
#undef BUFSIZ
#include <stdio.h>

match(choices, what)
register struct cmd	choices[];
register char	*what;
{
	register int	len;
	int	i,
		found = 0,
		save,
		exactmatch = -1;

	len = strlen(what);
	for (i = 0; choices[i].Name != 0; i++) {
		if (*what != *choices[i].Name)
			continue;
		if (strncmp(what, choices[i].Name, len) == 0)
			return i;
	}
	return -1;
}

char *
PPchar(c)
int	c;
{
	static char	str[16];
	char	*cp = str;

	if (c & 0200) {
		c &= ~0200;
		strcpy(cp, "M-");
		cp += 2;
	}
	if (c == '\033')
		strcpy(cp, "ESC");
#ifdef IBMPC
	else if (c == '\377')
	        strcpy(cp, "M");
#endif /* IBMPC */
	else if (c < ' ')
		(void) sprintf(cp, "C-%c", c + '@');
	else if (c == '\177')
		strcpy(cp, "^?");
	else
		(void) sprintf(cp, "%c", c);
	return str;
}

extract(into, from)
char	*into,
	*from;
{
	from += 2;	/* Past tab and first double quote. */
	while ((*into = *from++) != '"')
		into += 1;
	*into = 0;
}

main()
{
	FILE	*ifile,
		*of;
	char	line[100],
		comname[70];
	int	comnum,
		ch,
		savech = -1,
		errors = 0;

	ifile = stdin;
	of = stdout;
	if (ifile == NULL || of == NULL) {
		printf("Cannot read input or write output.\n");
		exit(1);
	}
	while (fgets(line, sizeof line, ifile) != NULL) {
		if (strncmp(line, "#if", 3) == 0) {
			savech = ch;
			fprintf(of, line);
			continue;
		} else if (strncmp(line, "#else", 5) == 0) {
			if (savech == -1)
				fprintf(stderr, "WARNING: ifdef/endif mismatch!\n");
			else
				ch = savech;
			fprintf(of, line);
			continue;
		} else if (strncmp(line, "#endif", 6) == 0) {
			savech = -1;
			fprintf(of, line);
			continue;
		} else if (strncmp(line, "\t\"", 2) != 0) {
			fprintf(of, line);
			ch = 0;
			continue;
		}
		extract(comname, line);
		if (strcmp(comname, "unbound") == 0) 
			comnum = 12345;
		else {
			comnum = match(commands, comname);
			if (comnum < 0) {
				fprintf(stderr, "Warning: cannot find command \"%s\".\n", comname);
				errors += 1;
				comnum = 12345;
			}
		}
		if (comnum == 12345)
			fprintf(of, "	(data_obj *) 0,                 /* %s */\n", PPchar(ch++));
		else
			fprintf(of, "	(data_obj *) &commands[%d],	/* %s */\n", comnum, PPchar(ch++));
	}
	fclose(of);
	fclose(ifile);
	exit(errors);
}
