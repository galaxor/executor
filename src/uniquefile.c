/* Copyright 1995 by Abacus Research and
 * Development, Inc.  All rights reserved.
 */

#if !defined (OMIT_RCSID_STRINGS)
char ROMlib_rcsid_uniquefile[] =
	    "$Id: uniquefile.c 88 2005-05-25 03:59:37Z ctm $";
#endif

#include "rsys/common.h"
#include "rsys/uniquefile.h"
#include "rsys/file.h"


/* This function fills in RESULT with the pathname for a unique file
 * of a specified form.  RESULT will be a 0-terminated Pascal string.
 * It returns TRUE iff successful, else FALSE.  If all names matching
 * the template are already taken by existing files, returns FALSE.
 * TEMPLATE indicates the form of the filename: it should have exactly
 * one "*" character, which will be replaced with an alphanumeric
 * character to try to obtain a unique filename.
 */

boolean_t
unique_file_name (const char *template, const char *default_template,
		  Str255 result)
{
  static const char suff[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  const char *try_suff;
  char *try;
  char *replace;
  struct stat sbuf;
  time_t oldest_time;

  /* The template string must have a '*'. */
  if (!template || !strchr (template, '*'))
    template = default_template;

  /* Default to an empty string. */
  result[0] = 0;
  result[1] = '\0';

  /* Make sure the resulting string won't be too long. */
  if (strlen (template) + 1 >= 255)
    return FALSE;

  try = copystr (template);
  
  if (try)
    {
      /* Try to find a unique file name.  If we fail, choose the oldest. */
      oldest_time = 0;
      replace = strrchr (try, '*');
      for (try_suff = suff; *try_suff; try_suff++)
	{
	  *replace = *try_suff;
	  if (Ustat (try, &sbuf) != 0 && errno == ENOENT)
	    {
	      /* If this file name isn't taken, grab it !*/
	      strcpy ((char *) result + 1, try);
	      result[0] = strlen ((char *) result + 1);
	      free (try);
	      return TRUE;
	    }
	}
    }
  /* Failed! */
  return FALSE;
}
