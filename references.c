#include <stdlib.h>
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "wv.h"

#if 0
extern wvStream *erroroutput;
extern wvStream *outputfile;
extern DOP dop;
extern long int cp;
extern int insuper;
extern int footnotehack;


void decode_f_reference(textportions *portions)
    {
    U32 i;
    error(erroroutput,"footnotes\n");
    /*
    search in the first thing for the cp of this
    reference, then use this index into the second
    one to determine what text to use ?
    */
    i=0;
    while(i<portions->fndref_no)
        {
        if (portions->fndRef[i] == cp)
            {
            error(erroroutput,"found correct ref pos at %x, index was %d\n",portions->fndRef[i],i);
            if (portions->fndFRD[i].frd > 0)
                {
                if (!insuper)
                    {
                    fprintf(outputfile,"<sup>");
                    insuper=2;
                    }
                error(erroroutput,"is autonumbered <%d>\n",portions->fndFRD[i].frd);
                portions->auto_foot++;
                fprintf(outputfile,"<a href=\"#foot%d\">",i);
				decode_list_nfc(portions->fndFRD[i].frd,dop.new_nfcFtnRef);
                fprintf(outputfile,"</a>");
                }
            else
                {
                error(erroroutput,"is custom <%d>\n",portions->fndFRD[i].frd);
                /*dont handling custom footnotes the same way*/
                fprintf(outputfile,"<a href=\"#foot%d\">",i);
                footnotehack=1;
                }
            if (insuper==2)
                fprintf(outputfile,"</sup>");
            break;
            }
        error(erroroutput,"conpared given pos %x and ref list %x",cp,portions->fndRef[i]);
        i++;
        }

    /*
    now to hold off to the end of the page, before listing the references
    off
    */
    if ( (i < portions->fndref_no ) && (i < 256) )
        portions->list_footnotes[portions->list_foot_no++] = i;
    else if (i >= 256)
        fprintf(erroroutput,"oops silly programmer :-) lost a footnote\n");
    }


void decode_e_reference(textportions *portions)
    {
    int i;
    error(erroroutput,"endnotes\n");
    /*
    search in the first thing for the cp of this
    reference, then use this index into the second
    one to determine what text to use ?
    */
    i=0;
    while(i<portions->endref_no)
        {
        if (portions->endRef[i] == cp)
            {
            error(erroroutput,"found correct ref pos at %x, index was %d\n",portions->endRef[i],i);
            if (portions->endFRD[i].frd > 0)
                {
                if (!insuper)
                    {
                    fprintf(outputfile,"<sup>");
                    insuper=2;
                    }
                error(erroroutput,"is autonumbered <%d>, i is %d, true is %d\n",portions->endFRD[i].frd,i,portions->endTrueFRD[i]);
				portions->endTrueFRD[i] = portions->auto_end++;
                fprintf(outputfile,"<a href=\"#end%d\">",i);
				decode_list_nfc(portions->endTrueFRD[i],dop.new_nfcEdnRef);
                fprintf(outputfile,"</a>");
                }
            else
                {
                error(erroroutput,"is custom <%d>\n",portions->endFRD[i].frd);
                /*dont handling custom footnotes the same way*/
                fprintf(outputfile,"<a href=\"#end%d\">",i);
                footnotehack=1;
                }
            if (insuper==2)
                fprintf(outputfile,"</sup>");
            break;
            }
        error(erroroutput,"conpared given pos %x and ref list %x",cp,portions->endRef[i]);
        i++;
        }

    /*
    now to hold off to the end of the page, before listing the references
    off
    */
    if ( (i < portions->endref_no ) && (i < 256) )
        portions->list_endnotes[portions->list_end_no++] = i;
    else if (i >= 256)
        fprintf(erroroutput,"oops silly programmer :-) lost a endnote\n");
    }

#endif
