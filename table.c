#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "bintree.h"


/*
allow figures within 3 units of each other to
be considered the same
*/
int shortCompLT(void *a,void *b)
    {
    S16 *a2,*b2;
    a2 = (S16 *)a;
    b2 = (S16 *)b;
    return( *a2 < (*b2)+3);
    }

int shortCompEQ(void *a,void *b)
    {
    S16 *a2,*b2;
    a2 = (S16 *)a;
    b2 = (S16 *)b;
    if( (*a2 - *b2) < 3)
		return(1);
	return(0);
    }

void wvGetRowTap(wvParseStruct *ps,PAP *dpap,U32 para_intervals,BTE *btePapx,U32 *posPapx)	
	{
	PAPX_FKP para_fkp;
	U32 para_fcFirst, para_fcLim=0xffffffff;
	PAP apap;
	U32 i,j=0;

	wvCopyPAP(&apap,dpap);

	wvInitPAPX_FKP(&para_fkp);

	i=ftell(ps->mainfd);
	do
		{
		wvReleasePAPX_FKP(&para_fkp);		
		wvGetSimpleParaBounds(wvQuerySupported(&ps->fib,NULL),&para_fkp,&para_fcFirst,&para_fcLim,i, btePapx, posPapx, para_intervals, ps->mainfd);
		wvAssembleSimplePAP(wvQuerySupported(&ps->fib,NULL),&apap, para_fcLim, &para_fkp, &ps->stsh);
		wvTrace(("para from %x to %x\n",para_fcFirst,para_fcLim));
		i=para_fcLim;
		}
	while(apap.fTtp == 0);

	wvTrace(("fTtp is %d\n",apap.fTtp));

	wvReleasePAPX_FKP(&para_fkp);
	wvCopyTAP(&(dpap->ptap),&apap.ptap);
	}

void wvGetFullTableInit(wvParseStruct *ps,U32 para_intervals,BTE *btePapx,U32 *posPapx)	
	{
	PAPX_FKP para_fkp;
	U32 para_fcFirst, para_fcLim=0xffffffff;
	PAP apap;
	U32 i,j=0;
	TAP *test=NULL;

	if (ps->intable) return;

	wvInitPAPX_FKP(&para_fkp);

	i = ftell(ps->mainfd);
	do
		{
		wvReleasePAPX_FKP(&para_fkp);		
		wvGetSimpleParaBounds(wvQuerySupported(&ps->fib,NULL),&para_fkp,&para_fcFirst,&para_fcLim,i, btePapx, posPapx, para_intervals, ps->mainfd);
		wvAssembleSimplePAP(wvQuerySupported(&ps->fib,NULL),&apap, para_fcLim, &para_fkp, &ps->stsh);
		wvTrace(("para from %x to %x\n",para_fcFirst,para_fcLim));
		i=para_fcLim;

		/* ignore the row end markers */
		if (apap.ptap.itcMac)
			{
			test = (TAP *)realloc(test,sizeof(TAP)*(j+1));
			wvCopyTAP(&(test[j]),&apap.ptap);
			j++;
			}

		}
	while(apap.fInTable);

	wvReleasePAPX_FKP(&para_fkp);

	wvSetTableInfo(ps,test,j);
	ps->intable=1;
	wvFree(test);
	}


/*
-------------------------
|          |            |
-------------------------
|   | | |    |     |    |
-------------------------
|                       |
-------------------------
|     |         |       |
-------------------------

==>

|   | | |  | |  |  |    |

As in this example we create a list of cell begin
positions which is a superset of all begin
positions in all rows, once we have this list we
restart at the top of the table and figure out
how many spans each cell has to achieve to match
back up to its original boundaries.

We will have to match boundaries that are with in
3 units of eachother to be the same boundary as
that occurs frequently in word tables, (gagh!)
*/
void wvSetTableInfo(wvParseStruct *ps,TAP *ptap,int no)
	{
	BintreeInfo tree;
	Node *testn,*testp;
	int i,j,k;

	InitBintree(&tree,shortCompLT,shortCompEQ);

	for (i=0;i<no;i++)
		for (j=0;j<ptap[i].itcMac+1;j++)
			{
			InsertNode(&tree,(void *) &(ptap[i].rgdxaCenter[j]) );
			}

	testn = NextNode(&tree,NULL);
	i=0;
    while (testn != NULL)
        {
		ps->cellbounds[i++] = *((S16 *)testn->Data);
        testp = NextNode(&tree,testn);
        DeleteNode(&tree,testn);
        testn = testp;
        }


	if (ps->vmerges)
		wvFree(ps->vmerges);
	ps->vmerges = (S16 **)malloc(sizeof(S16 *) * no);
	for(i=0;i<no;i++)
		{
		ps->vmerges[i] = (S16 *)malloc(sizeof(S16) * ptap[i].itcMac);
		for (j=0;j<ptap[i].itcMac;j++)
			ps->vmerges[i][j]=1;
		}

	for (i=no-1;i>0;i--)
		{
		for (j=0;j<ptap[i].itcMac;j++)
			{
			wvTrace(("Vertical merge is %d\n",ptap[i].rgtc[j].fVertMerge));
			if (ptap[i].rgtc[j].fVertMerge)
				{
				wvTrace(("Vertical merge found, row %d, cell %d\n",i,j));
				/* 
				find a cell above me with the same boundaries
					if it is also merged increment it, and set myself to 0
					else leave me alone
				*/
				for(k=0;k<ptap[i-1].itcMac;k++)	/* the row above */
					{
					wvTrace(("cell begins are %d %d\n",ptap[i-1].rgdxaCenter[k],ptap[i].rgdxaCenter[j]));
					wvTrace(("cell ends are %d %d\n",ptap[i-1].rgdxaCenter[k+1],ptap[i].rgdxaCenter[j+1]));
				
					if ( (shortCompEQ((void *)&(ptap[i-1].rgdxaCenter[k]) ,(void *)&(ptap[i].rgdxaCenter[j])))
						&&
						(shortCompEQ((void *)&(ptap[i-1].rgdxaCenter[k+1]) ,(void *)&(ptap[i].rgdxaCenter[j+1]))) )
						{
						wvTrace(("found a cell above me, yippee\n"));
						if (ptap[i-1].rgtc[k].fVertMerge)
							{
							ps->vmerges[i-1][k]+=ps->vmerges[i][j];
							ps->vmerges[i][j]=0;
							}
						}

					}
				}
			}
		}


	for(i=0;i<no;i++)
		for (j=0;j<ptap[i].itcMac;j++)
			wvTrace(("rowspan numbers are %d\n",ps->vmerges[i][j]));
	}

