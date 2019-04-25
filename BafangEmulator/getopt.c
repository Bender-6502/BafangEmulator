#include "getopt.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

int opterr   = 1;	/* if error message should be printed */
int optind   = 1;	/* index into parent argv vector */
int optopt   = 1;	/* character checked for validity */
int optreset = 0;	/* reset getopt */
char *optarg = NULL;	/* argument associated with option */

#define _DIAGASSERT(x) assert(x)
#define	BADCH	(int)'?'
#define	BADARG	(int)':'
#define	EMSG	""


static char * __progname(char * nargv0)
{
  char * tmp;

  _DIAGASSERT(nargv0 != NULL);

  tmp = (char*)strrchr(nargv0, '/');
  if (tmp)
    tmp++;
  else
    tmp = nargv0;

  return(tmp);
}


/*
* getopt --
*	Parse argc/argv argument vector.
*/
int getopt(int nargc, char * const *nargv, const char *ostr)
{
  static char *place = EMSG;		/* option letter processing */
  char *oli;				/* option letter list index */

  _DIAGASSERT(nargv != NULL);
  _DIAGASSERT(ostr != NULL);

  if (optreset || !*place) 
  {		
    /* update scanning pointer */
    optreset = 0;
    if (optind >= nargc || *(place = nargv[optind]) != '-') 
    {
      place = EMSG;
      return (-1);
    }

    if (place[1] && *++place == '-') 
    {	
      /* found "--" */
      /* ++optind; */
      place = EMSG;
      return (-2);
    }
  }					/* option letter okay? */

  if ((optopt = (int)*place++) == (int)':' || !(oli = (char*)strchr(ostr, optopt)))
  {
    /*
    * if the user didn't specify '-' as an option,
    * assume it means -1.
    */
    if (optopt == (int)'-')
      return (-1);
    if (!*place)
      ++optind;
    if (opterr && *ostr != ':')
      fprintf(stderr, "%s: illegal option -- %c\n", __progname(nargv[0]), optopt);

    return (BADCH);
  }

  if (*++oli != ':') 
  {			
    /* don't need argument */
    optarg = NULL;
    if (!*place)
      ++optind;
  }
  else 
  {				
    /* need an argument */
    if (*place)
    {
      optarg = place;
    }
    else if (nargc <= ++optind) 
    {	
      /* no arg */
      place = EMSG;
      if ((opterr) && (*ostr != ':'))
        fprintf(stderr, "%s: option requires an argument -- %c\n", __progname(nargv[0]), optopt);
      return (BADARG);
    }
    else
    {
      optarg = nargv[optind];
    }

    place = EMSG;
    ++optind;
  }

  return (optopt);			/* dump back option letter */
}


int getopt_long(int nargc, char *const *nargv, const char *options, const struct option *long_options, int *index)
{
  int retval;

  _DIAGASSERT(nargv != NULL);
  _DIAGASSERT(options != NULL);
  _DIAGASSERT(long_options != NULL);
  /* index may be NULL */

  if ((retval = getopt(nargc, nargv, options)) == -2) 
  {
    char *current_argv = nargv[optind++] + 2, *has_equal;
    int i, current_argv_len, match = -1;

    if (*current_argv == '\0') 
    {
      return(-1);
    }

    if ((has_equal = (char*)strchr(current_argv, '=')) != NULL)
    {
      current_argv_len = (int)(has_equal - current_argv);
      has_equal++;
    }
    else
    {
      current_argv_len = (int)strlen(current_argv);
    }

    for (i = 0; long_options[i].name; i++) 
    {
      if (strncmp(current_argv, long_options[i].name, current_argv_len))
        continue;

      if (strlen(long_options[i].name) == (unsigned)current_argv_len) 
      {
        match = i;
        break;
      }

      if (match == -1)
        match = i;
    }

    if (match != -1) 
    {
      if (long_options[match].has_arg == required_argument || long_options[match].has_arg == optional_argument) 
      {
        if (has_equal)
          optarg = has_equal;
        else
          optarg = nargv[optind++];
      }

      if ((long_options[match].has_arg == required_argument) && (optarg == NULL)) 
      {
        /*
        * Missing argument, leading :
        * indicates no error should be generated
        */
        if ((opterr) && (*options != ':'))
          fprintf(stderr, "%s: option requires an argument -- %s\n", __progname(nargv[0]), current_argv);
        return (BADARG);
      }
    }
    else 
    { 
      /* No matching argument */
      if ((opterr) && (*options != ':'))
        fprintf(stderr, "%s: illegal option -- %s\n", __progname(nargv[0]), current_argv);
      return (BADCH);
    }

    if (long_options[match].flag) 
    {
      *long_options[match].flag = long_options[match].val;
      retval = 0;
    }
    else
    {
      retval = long_options[match].val;
    }

    if (index)
      *index = match;
  }

  return(retval);
}
