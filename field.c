#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

static TokenTable s_Tokens[] = {
    {"TIME", FC_TIME},
    {"\\@", FC_DateTimePicture},
    {"HYPERLINK", FC_HYPERLINK},
    {"TOC", FC_TOC},
    {"\\o", FC_TOC_FROM_RANGE},
    {"PAGEREF", FC_PAGEREF},
    {"EMBED", FC_EMBED},
    {"EDITTIME", FC_EDITTIME},
    {"*", FC_OTHER}
};

static unsigned int
s_mapNameToToken (const char *name)
{
    unsigned int k;
    for (k = 0; k < FieldCodeTableSize; k++)
      {
	  if (s_Tokens[k].m_name[0] == '*')
	      return k;
	  else if (!(strcasecmp (s_Tokens[k].m_name, name)))
	      return k;
      }
    return 0;
}


int
lookahead (char *token, char test1, char test2)
{
    int ret = 0;
    while ((*token == test1) || (*token == test2))
      {
	  token++;
	  ret++;
      }
    return (ret);
}

int
wvHandleDateTimePicture (char *retstring, size_t max, char *token,
			 time_t * mytime)
{
    int no;
    struct tm *current;
    char timestr[4096];
    char temp[64];
    timestr[0] = '\0';

    if (!token)
	return (0);
    current = localtime (mytime);
    while (*token)
      {
	  switch (*token)
	    {
	    case 'A':
		if ((strlen (token) > 5) && (0 == strncmp (token, "AM/PM", 5)))
		  {
		      strcat (timestr, "%p");
		      token += 5;
		  }
		break;
	    case 'a':
		if ((strlen (token) > 5) && (0 == strncmp (token, "AM/PM", 5)))
		  {
		      strcat (timestr, "%P");
		      token += 5;
		  }
		break;
	    case 'M':
		no = lookahead (token, 'M', 'M');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		      sprintf (temp, "%d", current->tm_mon);
		      strcat (timestr, temp);
		      break;
		  case 2:
		      strcat (timestr, "%m");
		      break;
		  case 3:
		      strcat (timestr, "%b");
		      break;
		  default:
		      strcat (timestr, "%B");
		      break;
		  }
		break;
	    case 's':
	    case 'S':
		no = lookahead (token, 's', 'S');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		  default:
		      strcat (timestr, "%S");
		      break;
		  }
		break;
	    case 'd':
	    case 'D':
		no = lookahead (token, 'd', 'D');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		      sprintf (temp, "%d", current->tm_wday);
		      strcat (timestr, temp);
		      break;
		  case 2:
		      strcat (timestr, "%d");
		      break;
		  case 3:
		      strcat (timestr, "%a");
		      break;
		  default:
		      strcat (timestr, "%A");
		      break;
		  }
		break;
	    case 'y':
	    case 'Y':
		no = lookahead (token, 'y', 'Y');
		token += (no - 1);
		switch (no)
		  {
		  case 2:
		      strcat (timestr, "%y");
		      break;
		  default:
		      strcat (timestr, "%Y");
		      break;
		  }
		break;
	    case 'h':
		no = lookahead (token, 'h', 'h');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		      sprintf (temp, "%d", current->tm_hour % 12);
		      strcat (timestr, temp);
		      break;
		  default:
		      strcat (timestr, "%I");
		      break;
		  }
		break;
	    case 'H':
		no = lookahead (token, 'H', 'H');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		      sprintf (temp, "%d", current->tm_hour);
		      strcat (timestr, temp);
		      break;
		  default:
		      strcat (timestr, "%H");
		      break;
		  }
		break;
	    case 'm':
		no = lookahead (token, 'm', 'm');
		token += (no - 1);
		switch (no)
		  {
		  case 1:
		      sprintf (temp, "%d", current->tm_min);
		      strcat (timestr, temp);
		      break;
		  default:
		      strcat (timestr, "%M");
		      break;
		  }
		break;
	    case '\"':
		break;
	    case '`':
		break;
	    default:
		temp[0] = *token;
		temp[1] = '\0';
		strcat (timestr, temp);
		break;
	    }
	  token++;
      }
    return (strftime (retstring, max, timestr, current));
}


int
wvHandleTotalField (char *command)
{
    int ret = 0;
    unsigned int tokenIndex;
    char *token;

    if (*command != 0x13)
      {
	  wvError (("field did not begin with 0x13\n"));
	  return (1);
      }
    strtok (command, "\t, ");
    while ((token = strtok (NULL, "\t, ")))
      {
	  tokenIndex = s_mapNameToToken (token);
	  switch (s_Tokens[tokenIndex].m_type)
	    {
	    case FC_HYPERLINK:
		token = strtok (NULL, "\"\" ");
		printf ("</a>");
		break;
	    default:
		break;
	    }
      }
    return (ret);
}

int
wvHandleCommandField (char *command)
{
    int ret = 0;
    unsigned int tokenIndex;
    char *token;
    char datestr[4096];
    time_t mytime;

    if (*command != 0x13)
      {
	  wvError (("field did not begin with 0x13\n"));
	  return (1);
      }
    strtok (command, "\t, ");
    while ((token = strtok (NULL, "\t, ")))
      {
	  tokenIndex = s_mapNameToToken (token);
	  switch (s_Tokens[tokenIndex].m_type)
	    {
	    case FC_HYPERLINK:
		token = strtok (NULL, "\"\" ");
		printf ("<a href=\"%s\">", token);
		break;
	    case FC_EMBED:
		wvError (("embed\n"));
		token = strtok (NULL, "\t, ");
		printf ("<!--%s-->", token);
		break;
	    case FC_PAGEREF:
		token = strtok (NULL, "\"\" ");
		printf ("<a href=\"#%s\">", token);
		break;
	    case FC_EDITTIME:
		token = strtok (NULL, "\"\" ");
		break;
	    case FC_TOC:
		wvTrace (("toc\n"));
		break;
	    case FC_TOC_FROM_RANGE:
		token = strtok (NULL, "\"\" ");
#ifdef DEBUG
		if (token)
		    wvTrace (("toc range is %s\n", token));
#endif
		break;
	    case FC_TIME:
		wvError (("time token\n"));
		ret = 1;
		time (&mytime);
		break;
	    case FC_DateTimePicture:
		wvTrace (("DateTimePicture\n"));
		token = strtok (NULL, "\"\"");
		if (wvHandleDateTimePicture (datestr, 4096, token, &mytime))
		    printf ("%s", datestr);
		else
		    wvError (
			     ("date and time field function returned nothing\n"));
		break;
	    default:
		break;
	    }
      }
    return (ret);
}

U16 command[40000];
U16 argumen[40000];

int
fieldCharProc (wvParseStruct * ps, U16 eachchar, U8 chartype, U16 lid)
{
    static U16 *which;
    static int i, depth;
    char *a;
    static char *c = NULL;
    static int ret;

    if (eachchar == 0x13)
      {
	  a = NULL;
	  ret = 1;
	  if (depth == 0)
	    {
		which = command;
		command[0] = 0;
		argumen[0] = 0;
		i = 0;
	    }
	  depth++;
      }
    else if (eachchar == 0x14)
      {
	  if (depth == 1)
	    {
		command[i] = 0;
		c = wvWideStrToMB (command);
		if (wvHandleCommandField (c))
		    ret = 1;
		else
		    ret = 0;

		wvError (
			 ("command %s, ret is %d\n", wvWideStrToMB (command),
			  ret));
		wvFree (c);
		which = argumen;
		i = 0;
	    }
      }
    if (i >= 40000)
      {
	  wvError (("WHAT!\n"));
	  exit (-10);
      }

    which[i] = eachchar;
    if (chartype)
	which[i] = wvHandleCodePage (which[i], lid);
    i++;

    if (eachchar == 0x15)
      {
	  depth--;
	  if (depth == 0)
	    {
		which[i] = 0;
		a = wvWideStrToMB (argumen);
		c = wvWideStrToMB (command);
		wvHandleTotalField (c);
		wvFree (a);
		wvFree (c);
	    }
      }
    return (ret);
}
