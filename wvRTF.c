#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include "getopt.h"
#include "config.h"
#include "wv.h"

/* By Dom Lachowicz (cinamod@hotmail.com) */

/* i don't like appearing to use printf */
#define output printf

typedef struct _rtfUserData
{
  int cFont;
  int cCol;
  int bIsBold;
  int bIsItal;
  int bIsStrike;
  int bIsSup;
  int bIsSub;
} rtfUserData;

/* our 4 magical functions */
int charProc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid);
int specCharProc(wvParseStruct *ps, U16 eachchar, CHP * achp);
int eleProc(wvParseStruct *ps, wvTag tag, void *props, int dirty);
int docProc(wvParseStruct *ps, wvTag tag);

/* this gets printed at the top of each document in the {\fonttbl section */

struct _fontMapping
{
  char *word;
  char *rtf;
};

/* TODO: build me up appropriately */
static struct _fontMapping fontMap[] =
{
  {"Arial", "Arial"},
  {"Bitstream Charter", "Bitstream Charter"},
  {"Bookman", "Bookman"},
  {"Courier", "Courier"},
  {"Courier New", "Courier New"},
  {"Century Schoolbook", "Century Schoolbook"},
  {"Dingbats", "Dingbats"},
  {"Goth", "Goth"},
  {"Nimbus Sans", "Nimbus Sans"},
  {"Palladio", "Palladio"},
  {"Standard Symbol", "Standard Symbol"},
  {"Symbol", "Symbol"},
  {"Times", "Times New Roman"},
  {"Times New Roman", "Times New Roman"},
};

#define FontTblSize (sizeof(fontMap)/sizeof(fontMap[0]))
#define DFL_FONT_INDEX 3 /* I map this to whatever "Times New Roman" is */
static void
output_fonttable(void)
{
  int i;

  output("{\\fonttbl\n");

  for(i=0; i<FontTblSize; i++)
    output("{\\f%d\\fnil\\fcharset0\\fprq0\\fttruetype %s;}\n", i, fontMap[i].rtf);
  
  output("}\n");
}

/* map the MSWord name to the corresponding RTF name index */
static int 
mapFont(const char * name)
{
  int k;

  for (k=0; k<FontTblSize; k++)
    if (!strcasecmp(fontMap[k].word, name))
      return k;

  return DFL_FONT_INDEX;
}

#undef DFL_FONT_INDEX
#undef FontTblSize

/* this gets printed at the top of each document in the {\colortbl section */
static int colorTable[][3] = 
{
  {0x00, 0x00, 0x00}, /* black */
  {0x00, 0x00, 0xff}, /* blue */
  {0x00, 0xff, 0xff}, /* cyan */
  {0x00, 0xff, 0x00}, /* green */
  {0xff, 0x00, 0xff}, /* magenta */
  {0xff, 0x00, 0x00}, /* red */
  {0xff, 0xff, 0x00}, /* yellow */
  {0xff, 0xff, 0xff}, /* white */
  {0x00, 0x00, 0x80}, /* dark blue */
  {0x00, 0x80, 0x80}, /* dark cyan */
  {0x00, 0x80, 0x00}, /* dark green */
  {0x80, 0x00, 0x80}, /* dark magenta */
  {0x80, 0x00, 0x00}, /* dark red */
  {0x80, 0x80, 0x00}, /* dark yellow */
  {0x80, 0x80, 0x80}, /* dark gray */
  {0xc0, 0xc0, 0xc0}, /* light gray */
};

/* rtf names for the color_table as above */
static char *rtfColors[] =
{
  "\\cf1", /* black */
  "\\cf2", /* blue */
  "\\cf3", /* cyan */
  "\\cf4", /* green */
  "\\cf5", /* magenta */
  "\\cf6", /* red */
  "\\cf7", /* yellow */
  "\\cf8", /* white */
  "\\cf9", /* dark blue */
  "\\cf10", /* dark cyan */
  "\\cf11", /* dark green */
  "\\cf12", /* dark magenta */
  "\\cf13", /* dark red */
  "\\cf14", /* dark yellow */
  "\\cf15", /* dark gray */
  "\\cf16", /* light gray */
};

#define RED(i)   colorTable[(i)][0]
#define GREEN(i) colorTable[(i)][1]
#define BLUE(i)  colorTable[(i)][2]
#define ClrTblSize (sizeof(colorTable)/sizeof(colorTable[0]))
static void
output_colortable(void)
{
  int i;

  output("{\\colortbl\n");

  for(i=0;i<ClrTblSize;i++)
    {
      output("\\red%d\\green%d\\blue\\%d;\n", RED(i), GREEN(i), BLUE(i));
    }
  
  output("}\n");
}
#undef RED
#undef GREEN
#undef BLUE
#undef ClrTblSize

static void
do_version(void)
{
  printf("wvRTF version 0.6.3\n");
}

static void
do_help(void)
{
  do_version();
  printf("(c) Dom Lachowicz 2000\n");
  printf("Usage:\n");
  printf("\t-c --charset=set\n");
  printf("\t-p --password=pass\n");
  printf("\t-v --version\n");
  printf("\t-? --help\n");
  printf("\nConverts MSWord documents to RTF\n");
}

char *charset=NULL;

int
main(int argc, char *argv[])
{
  FILE *input;
  char *fname, *password;
  int   ret;

  wvParseStruct ps;
  char *dir=NULL;

  rtfUserData ud;

  static struct option long_options[] =
  {
    {"charset",1,0,'c'},
    {"password",1,0,'p'},
    {"dir",1,0,'d'},
    {"version",0,0,'v'},
    {"help",0,0,'?'},
    {0,0,0,0}
  };

  int c, index=0;

  wvInitError();
  
  if(argc < 2)
    {
      do_help();
      return 1;
    }

  while (1)
    { 
      c = getopt_long (argc, argv, "?vc:p:d:", long_options, &index);
      if (c == -1)
	break;
      switch(c)
	{
	case '?':
	  do_help();
	  return 0;
	case 'v':
	  do_version();
	  return 0;
	case 'c':
	  if (optarg)
	    charset = optarg;
	  else
	    wvError(("No argument given to charset"));
	  break;
	case 'p':
	  if (optarg)
	    password = optarg;
	  else
	    wvError(("No password given to password option"));
	  break;
	case 'd':
	  if (optarg)
	    dir = optarg;
	  else
	    wvError(("No directory given to dir option"));
	  break;
	default:
	  do_help();
	  return -1;
	}
    }

  if (optind >= argc)
    {
      fprintf(stderr,"No file name given to open\n");
      return -1;
    }

  fname = argv[optind];

  input = fopen(fname,"rb");
  if (!input)
    {
      fprintf(stderr,"Failed to open %s: %s\n", fname, strerror(errno));
      return -1;
    }
  fclose(input);
  
  ret = wvInitParser(&ps,fname);
  ps.filename = fname;
  ps.dir = dir;
  
  memset(&ud, sizeof(ud), 0);
  ps.userData = &ud;

  if (ret & 0x8000)  /* Password protected? */
    {
      if ( (ret & 0x7fff) == WORD8)
	{
	  ret = 0;
	  if (password == NULL)
	    {
	      fprintf(stderr,"Password required, this is an encrypted document\n");
	      return -1;
	    }
	  else
	    {
	      wvSetPassword(password,&ps);
	      if (wvDecrypt97(&ps))
		{
		  wvError(("Incorrect Password\n"));
		  return -1;
		}
	    }
	}
      else if ( ( (ret & 0x7fff) == WORD7) || ( (ret & 0x7fff) == WORD6))
	{
	  ret=0;
	  if (password == NULL)
	    {
	      fprintf(stderr,"Password required, this is an encrypted document\n");
	      return -1;
	    }
	  else
	    {
	      wvSetPassword(password,&ps);
	      if (wvDecrypt95(&ps))
		{
		  wvError(("Incorrect Password\n"));
		  return -1;
		}
	    }
	}
    }

  if (ret)
    {
      wvError(("startup error\n"));
      wvOLEFree();
      return -1;
    }

  wvSetElementHandler(eleProc);
  wvSetDocumentHandler(docProc);
  wvSetCharHandler(charProc);
  wvSetSpecialCharHandler(specCharProc);

  wvText(&ps);

  /* free associated memory */
  wvOLEFree();

  return 0;
}

#define OUTPUT_CHAR(c)  do {output("%c", (c));} while(0)
#define OUTPUT_CHARS(s) do {output("%s", (s));} while(0)
#define ENSURE_BUF()    fflush(stdout)

int 
charProc(wvParseStruct *ps, U16 eachchar, U8 chartype, U16 lid)
{

  /* convert incoming character to unicode */
  if (chartype)
    eachchar = wvHandleCodePage(eachchar, lid);

  /* take care of any oddities in Microsoft's character "encoding" */
  /* TODO: does the above code page handler take care of these? */
  if (chartype == 1 && eachchar == 146) eachchar = 39; /* apostrophe */

  switch (eachchar)
    {
    case 13: /* paragraph end */
      return 0;

    case 11: /* hard line break */
      break;

    case 12: /* page breaks, section marks */
      break;

    case 14: /* column break */
      break;

    case 19: /* field begin */
      /* flush current text buffer */
      ps->fieldstate++;
      ps->fieldmiddle = 0;
      return 0;
    case 20: /* field separator */
      ps->fieldmiddle = 1;
      return 0;
    case 21: /* field end */
      ps->fieldstate--;
      ps->fieldmiddle = 0;
      return 0;

    default:
      break;
    }

  if (ps->fieldstate && !ps->fieldmiddle) return 0;
	   
  OUTPUT_CHAR(eachchar);
  return 0;
}

int 
specCharProc(wvParseStruct *ps, U16 eachchar, CHP * achp)
{
  /* TODO: handle RTF images and fields */
  return 0;
}

int 
eleProc(wvParseStruct *ps, wvTag tag, void *props, int dirty)
{
  /* some word structures */
  PAP *apap;
  CHP *achp;
  SEP *asep;
  int iRes;

  switch(tag)
    {
    case SECTIONBEGIN:
      asep = (SEP*)props;
      break;

    case SECTIONEND:
      break;

    case PARABEGIN:
      OUTPUT_CHARS("\\par\\parad");

      apap = (PAP*)props;

      switch(apap->jc)
	{
	case 0: /* left */
	  break;
	case 1: /* center */
	  OUTPUT_CHARS("\\qc");
	  break;
	case 2: /* right */
	  OUTPUT_CHARS("\\qr");
	  break;
	default:
	  break;
	}

      break;

    case PARAEND: /* pretty much nothing */
      OUTPUT_CHAR('\n');
      break;

    case CHARPROPBEGIN:
      achp = (CHP*)props;

      OUTPUT_CHAR('{');

      /* text color */
      if (achp->ico) {
	OUTPUT_CHARS(rtfColors[achp->ico-1]);
      }  

      /* font family */
      {
	char *fname;
	if (!ps->fib.fFarEast) {
	  fname = wvGetFontnameFromCode(&ps->fonts, achp->ftcAscii);
	} else {
	  fname = wvGetFontnameFromCode(&ps->fonts, achp->ftcFE);
	}
	
	output("\\f%d", mapFont(fname));
      }

      /* font size */
      output("\\fs%d", achp->hps);

      /* italic text */
      if (achp->fItalic) {
	OUTPUT_CHARS("\\i");
      }

      /* bold text */
      if (achp->fBold) { 
	OUTPUT_CHARS("\\b");
      }
      
      /* underline and strike-through */
      if (achp->fStrike || achp->kul) {
	if (achp->fStrike && achp->kul) {
	  OUTPUT_CHARS("\\ul\\strike");
	} else if (achp->kul) {
	  OUTPUT_CHARS("\\ul");
	} else {
	  OUTPUT_CHARS("\\strike");
	}
      }
      
      /* sub/superscript */
      if (achp->iss == 1) {
	OUTPUT_CHARS("\\super");
      } else if (achp->iss == 2) {
	OUTPUT_CHARS("\\sub");
      }
      
      /* add the final space */
      OUTPUT_CHAR(' ');
      break;
      
    case CHARPROPEND:
      OUTPUT_CHAR('}');
      break;

    default:
      break;
    }

  return 0;
}

int 
docProc(wvParseStruct *ps, wvTag tag)
{
  switch(tag)
    {
    case DOCBEGIN:
      /* print out my rtf preamble */
      output("{\\rtf1\\ansi\\ansicpg1252\\deff0\n");
      
      /* now print out a font table */
      /* and a color table */
      output_fonttable();
      output_colortable();
      break;

    case DOCEND:
      output("}\n");
      ENSURE_BUF();
      break;

    default:
      break;
    }

  return 0;
}

