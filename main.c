/*
Released under GPL, written by Caolan.McNamara@ul.ie.

Copyright (C) 1998,1999 
	Caolan McNamara

Real Life: Caolan McNamara           *  Doing: MSc in HCI
Work: Caolan.McNamara@ul.ie          *  Phone: +353-61-202699
URL: http://skynet.csn.ul.ie/~caolan *  Sig: an oblique strategy
How would you have done it?
*/

/*
this software no longer requires laola
*/

extern char *wv_version;

/*

this code is often all over the shop, being more of an organic entity
that a carefully planed piece of code, so no laughing there at the back!

and send me patches by all means, but think carefully before sending me
a patch that doesnt fix a bug or add a feature but instead just changes
the style of coding, i.e no more thousand line patches that fix my 
indentation. (i like it this way)

*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include "config.h"
#include "wv.h"
#include "roman.h"
#include "utf.h"

extern int nofontfaces;
extern int riskbadole;
extern int NORMAL;
extern int notablewidth;
extern int ignoreheadings;
extern int noheaders;
extern int doannos;
extern int dorevs;
extern int padding;
extern int verpadding;
extern float tabsize;
extern int currentfontsize;
extern STTBF revisions;
extern int inarow;
extern int inacell;
extern int inatable;
extern int inafont;
extern char backgroundcolor[8];



extern char *symbolurl;
extern char *imagesurl;
extern char *imagesdir;
extern char *wingdingurl;
extern char *patternurl;
extern char *outputfilename;
extern char *errorfilename;
extern char *filename;
extern char *configfile;
extern char *truetypedir;
extern FILE *outputfile;
extern FILE *erroroutput;
extern element_style ele_style[3];


extern RETSIGTYPE reaper(int);
extern RETSIGTYPE timeingout(int );
extern void signal_handle (int sig, SigHandler * handler);

extern document_style *doc_style;

void usage( void )
    {
    fprintf(erroroutput,"Usage: mswordview [-v] [--version] [-n] [--nocredits] [-c] [--corehtmlonly]\n"
	                    "[-t seconds] [--timeout seconds] [-f points] [--defaultfontsize points] [-p url]\n"
						"[--patternurl url] [-s url] [--symbolurl url] [-d url] [--wingdingurl url] [-h]\n"
						"[--ignoreheadings] [-m] [--mainonly] [-b] [--riskbadole] [-e] [--nofontfaces]\n"
						"[-o] [--outputfile] [-g erroroutputfile] [--errorfile erroroutputfile] \n"
						"[-y tabvalue] [--tabsize tabvalue] [-i] [--imagesdir] [-j] [--imagesurl] \n"
						"[-k] [--notablewidth] [-a] [--noannotations] [-r] [--norevisions] [-x truetypedir] \n"
						"[--truetypedir truetypedir] [-z configfile] [--configfile configfile] filename.doc\n");
    exit(-1);
    }



int main(int argc,char **argv)
	{
	int ret=0;
	int timeout=-1;
	char *endptr;
	char *buffer;
	char fileinbuf[1024];
	
	FILE *filein;
	FILE *input;

	FILE *mainfd=NULL;
	FILE *tablefd0=NULL;
	FILE *tablefd1=NULL;
	FILE *data=NULL;
	FILE *summary=NULL;

	int c;
	int tail=1;
	int core=1;
	int index=0;

	chp achp;
	pap apap;

	static struct option long_options[] = 
		{
		{ "version",0 , 0, 'v' },
		{ "corehtmlonly",0 , 0, 'c' },
		{ "nocredits",0 , 0, 'n' },
		{ "noannotations",0 , 0, 'a' },
		{ "norevisions",0 , 0, 'r' },
		{ "timeout",1 , 0, 't' },
		{ "horizontalwhite",1,0,'w'},
		{ "verticalwhite",1,0,'u'},
		{ "symbolurl", 1,0,'s'},
		{ "wingdingurl", 1,0,'d'},
		{ "imagesdir", 1,0,'i'},
		{ "imagesurl", 1,0,'j'},
		{ "ignoreheadings", 0,0,'h'},
		{ "defaultfontsize", 1,0,'f'},
		{ "mainonly", 0,0,'m' },
		{ "riskbadole", 0,0,'b'},
		{ "nofontfaces", 0,0,'e'},
		{ "outputfile", 1,0,'c'},
		{ "errorfile", 1,0,'g'},
		{ "patternurl",1,0,'p'},
		{ "tabsize",1,0,'y'},
		{ "notablewidth",1,0,'k'},
		{ "configfile",1,0,'z'},
		{ "truetypedir",1,0,'x'},
		{ 0,      0, 0, '0' },
		};


	erroroutput=stderr;

	while (1)
		{
		c = getopt_long (argc, argv, "abcd:ef:g:hi:j:kmno:p:rs:t:u:vw:x:y:z:", long_options, &index);
		if (c == -1)
			break;
		switch(c)
			{
			case 'f':
				if (optarg)
					{
                   	NORMAL = strtol(optarg, &endptr, 10);
				   	if ((*optarg == '\0') || (*endptr  != '\0'))
						{
						fprintf(erroroutput,"f option must be followed with a number\n");
						NORMAL=20;
						}
					else
						NORMAL*=2;
				 	}
				else 
					{
					fprintf(erroroutput,"no val given for defaultfontsize (-f) \n");
					NORMAL=20;
					}
				break;
			case 'v':
				printf("%s, by Caolan.McNamara@ul.ie\n\nWeb pages for updates exist at \nhttp://www.gnu.org/~caolan/docs/MSWordView.html\nhttp://www.csn.ul.ie/~caolan/docs/MSWordView.html\n\n", wv_version);
				exit (0);
				break;
			case 'k':
				notablewidth=1;
				break;
			case 'm':
				noheaders=1;
				break;
			case 'e':
				nofontfaces=1;
				break;
			case 'b':
				riskbadole=1;
				break;
			case 'h':
				ignoreheadings=1;
				break;
			case 'n':
				tail=0;
				break;	
			case 'a':
				doannos=0;
				break;	
			case 'r':
				dorevs=0;
				break;	
			case 'u':
				if (optarg)
					{
                   	verpadding = strtol(optarg, &endptr, 10);
				   	if ((*optarg == '\0') || (*endptr  != '\0'))
						{
						fprintf(erroroutput,"u option must be followed with a number\n");
						verpadding=0;
						}
					if ((verpadding < 0) || (verpadding > 2))
						{
						fprintf(erroroutput,"u option accepts on 0,1,2 as an argument, not %d\n",verpadding);
						verpadding=0;
						}
				 	}
				else 
					{
					fprintf(erroroutput,"no val given for verticalpadding (-u) \n");
					verpadding=0;
					}
				
				
				break;	
			case 't':
				if (optarg)
					{
                   	timeout = strtol(optarg, &endptr, 10);
				   	if ((*optarg == '\0') || (*endptr  != '\0'))
						fprintf(erroroutput,"t option must be followed with a number\n");
				 	}
				else 
					fprintf(erroroutput,"no val given for timeout option (-t)\n");
				break;
			case 'o':
				if (optarg)
					{
					outputfilename = (char *)malloc(strlen(optarg)+1);
					if (outputfilename == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(outputfilename,optarg);
					}
                else
					{
                    fprintf(erroroutput,"no val given, assuming standard\n");
					}
                break;
			case 'g':
				if (optarg)
					{
					errorfilename = (char *)malloc(strlen(optarg)+1);
					if (errorfilename == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(errorfilename,optarg);
					}
                else
					{
                    fprintf(erroroutput,"no val given, assuming standard\n");
					}
                break;
			case 'j':
				if (optarg)
					{
					imagesurl = (char *)malloc(strlen(optarg)+1);
					if (imagesurl == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(imagesurl,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for imagesurl\n");
                break;
			case 'i':
				if (optarg)
					{
					imagesdir = (char *)malloc(strlen(optarg)+1);
					if (imagesdir == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(imagesdir,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for imagesdir\n");
                break;
			case 's':
				if (optarg)
					{
					symbolurl = (char *)malloc(strlen(optarg)+1);
					if (symbolurl == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(symbolurl,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for symbolurl\n");
                break;
			case 'p':
				if (optarg)
					{
					patternurl = (char *)malloc(strlen(optarg)+1);
					if (patternurl == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(patternurl,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for patternurl\n");
                break;
			case 'd':
				if (optarg)
					{
					wingdingurl = (char *)malloc(strlen(optarg)+1);
					if (wingdingurl == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(wingdingurl,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for wingdingurl\n");
                break;
			case 'x':
				if (optarg)
					{
					truetypedir = (char *)malloc(strlen(optarg)+1);
					if (truetypedir == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(truetypedir,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for truetypedir\n");
                break;
			case 'z':
				if (optarg)
					{
					configfile = (char *)malloc(strlen(optarg)+1);
					if (configfile == NULL)
						{
						fprintf(erroroutput,"no mem\n");
						exit(-1);
						}
					strcpy(configfile,optarg);
					}
                else
                    fprintf(erroroutput,"no val given for configfile\n");
                break;
			case 'w':
				if (optarg)
					{
                   	padding = strtol(optarg, &endptr, 10);
				   	if ((*optarg == '\0') || (*endptr  != '\0'))
						{
						fprintf(erroroutput,"w option must be followed with a number\n");
						padding=0;
						}
					if ((padding < 0) || (padding > 5))
						{
						fprintf(erroroutput,"w option must be followed with a number from 0..5\n");
						padding=0;
						}
				 	}
				else 
					{
					fprintf(erroroutput,"no val given for horizontalwhite option (-w)\n");
					padding=0;
					}
				break;
			case 'y':
				if (optarg)
					{
                   	tabsize = strtod(optarg, &endptr);
				   	if ((*optarg == '\0') || (*endptr  != '\0'))
						{
						fprintf(erroroutput,"tabsize (y) option must be followed with a number\n");
						tabsize=-1;
						}
				 	}
				else 
					{
					fprintf(erroroutput,"no val given for defaultfontsize (-f) \n");
					tabsize = -1;
					}
				break;
			case 'c':
				core=0;
				break;
			default:
				usage();
				break;
			}
		}

	if (argc <= optind) 
		{
		usage();
		}

	if (errorfilename != NULL)
		{
		if (!(strcmp(errorfilename,"-")))
			erroroutput = stdout;
		else
			erroroutput = fopen(errorfilename,"w");
		if (erroroutput== NULL)
			{
			fprintf(erroroutput,"couldnt open %s for writing\n",errorfilename);
			return(-1);
			}
		}
	else
		erroroutput = erroroutput;
		

	if (optind < argc)
		filename = strdup(argv[optind]);
	else 
		filename =NULL;

	if (filename == NULL)
		{
		fprintf(erroroutput,"The file %s doesn't exist\n",filename);
		return(ret);
		}

	/*set SIGCHLD handled*/
	signal_handle (SIGCHLD, reaper);

	currentfontsize = NORMAL;
	
	if (timeout != -1)
		{
		signal_handle (SIGALRM, timeingout);
		/*well abort after this number of seconds*/
		alarm(timeout);
		}

	if (tabsize == -1)
		{
		if ( (padding == 0) || (padding == 3))
			tabsize=SPACEPIXELS;
		else
			tabsize=8;
		}
	else
		if ( (padding == 0) || (padding == 3))
			tabsize=tabsize/8;

	revisions.nostrings=0;	/*messy as hell*/

	input = fopen(filename,"rb");
	ret = wvOLEDecode(input,&mainfd,&tablefd0,&tablefd1,&data,&summary);
	if (ret)
		{
		fprintf(erroroutput,"Sorry main document stream couldnt be found in doc \n%s\n",filename);
		fprintf(erroroutput,"if this *is* a word 8 file, it appears to be corrupt\n");
		fprintf(erroroutput,"remember, mswordview cannot handle rtf,word 6 or word 7 etc\n");

		buffer = (char *) malloc(strlen(filename) +3 + strlen("file "));
		sprintf(buffer,"file \"%s\"",filename);
		filein = popen(buffer,"r");
		if (filein != NULL)
			{
			fprintf(erroroutput,"for your information, the utility \n\"file %s\" reports ...\n\n",filename);
			while (fgets(fileinbuf,1024,filein) != NULL)
				fprintf(erroroutput,"%s",fileinbuf);
			}
		free(buffer);
		ret=10;
		if (riskbadole) 
			ret = decode_word8(mainfd,tablefd0,tablefd1,data,core);
		}
	else
		{
		ret = decode_word8(mainfd,tablefd0,tablefd1,data,core);
		wvOLEFree();
		}

	if ((ret != -1) && (ret != 10))
		{
		init_pap(&apap);
		if (inacell)
			{
			init_chp(&achp);
			decode_e_chp(&achp);
			decode_e_specials(&apap,&achp,NULL);

			fprintf(outputfile,"\n</TD>\n");
			backgroundcolor[0] = '\0';
			}

		if (inarow)
			fprintf(outputfile,"\n</tr>\n");
		if (inatable)
			fprintf(outputfile,"\n</table>\n");

		do_indent(&apap);

		init_chp(&achp);
		decode_e_chp(&achp);
		if (inafont)
			{
			fprintf(outputfile,"%s\n",ele_style[FONT].end);
			inafont=0;
			}
		decode_e_specials(&apap,&achp,NULL);


#if 0
		fprintf(outputfile,"\n<br><img src=\"%s/documentend.gif\"><br>\n",patterndir());

		if (tail)
			{
			fprintf(outputfile,"<hr><p>\nDocument converted from word 8 by \n<a href=\"http://www.csn.ul.ie/~caolan/docs/MSWordView.html\">MSWordView</a> (%s)<br>\n",wv_version);
			fprintf(outputfile,"MSWordView written by <a href=\"mailto:Caolan.McNamara@ul.ie\">Caolan McNamara</a>\n</body>\n");
			}
		if (core)
			fprintf(outputfile,"\n</html>\n");
#endif
		do_output_end(doc_style,core,tail);
		}
	cleanupglobals();
	
	if (ret == 10) /*known reason as to why conversion did not occur*/
		ret=0;
	return(ret);
	}



#if defined (HAVE_POSIX_SIGNALS)
void signal_handle (int sig, SigHandler * handler)
    {
    struct sigaction act, oact;

    act.sa_handler = handler;
    act.sa_flags = 0 | SA_NOCLDSTOP | SA_RESTART;
    sigemptyset (&act.sa_mask);
    sigemptyset (&oact.sa_mask);
    sigaction (sig, &act, &oact);
    }
#endif
