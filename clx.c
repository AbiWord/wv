#include <stdlib.h>
#include <stdio.h>
#include "wv.h"

void wvReleaseCLX(CLX *clx)
	{
	U16 i;
	for(i=0;i<clx->grpprl_count;i++)
		wvFree(clx->grpprl[i]);
	wvFree(clx->grpprl);
	wvFree(clx->cbGrpprl);
	wvReleasePCD_PLCF(clx->pcd,clx->pos);
	}

/*
The complex part of a file (CLX) is composed of a number of variable-sized
blocks of data. Recorded first are any grpprls that may be referenced by the
plcfpcd (if the plcfpcd has no grpprl references, no grpprls will be
recorded) followed by the plcfpcd. Each block in the complex part is
prefaced by a clxt (clx type), which is a 1-byte code, either 1 (meaning the
block contains a grpprl) or 2 (meaning this is the plcfpcd). A clxtGrpprl
(1) is followed by a 2-byte cb which is the count of bytes of the grpprl. A
clxtPlcfpcd (2) is followed by a 4-byte lcb which is the count of bytes of
the piece table. A full saved file will have no clxtGrpprl's.
*/
void wvGetCLX(CLX *clx,U32 offset,U32 len,FILE *fd)
	{
	U8 clxt;
	U16 cb;
	U32 lcb,i,j=0;

	fseek(fd,offset,SEEK_SET);
	wvTrace("clx offset is %d, len is %d\n",offset,len);

	wvInitCLX(clx);

	while (j<len)
		{
		clxt = getc(fd);
		j++;
		if (clxt == 1)
			{
			cb = read_16ubit(fd);
			j+=2;
			clx->grpprl_count++;
			clx->cbGrpprl = (U16 *)realloc(clx->cbGrpprl,sizeof(U16)*clx->grpprl_count);
			clx->cbGrpprl[clx->grpprl_count-1] = cb;
			clx->grpprl = (U8 **)realloc(clx->grpprl,sizeof(U8 *)*(clx->grpprl_count));
			clx->grpprl[clx->grpprl_count-1] = (U8 *)malloc(cb);
			for(i=0;i<cb;i++)
				 clx->grpprl[clx->grpprl_count-1][i] = getc(fd);
			j+=i;
			}
		else if (clxt == 2)
			{
			lcb = read_32ubit(fd);
			j+=4;
			wvGetPCD_PLCF(&clx->pcd,&clx->pos,&clx->nopcd,ftell(fd),lcb,fd);
			j+=lcb;
			}
		else
			{
			wvError("clxt is not 1 or 2, it is %d\n",clxt);
			return;
			}
		}
	}


void wvInitCLX(CLX *item)
	{
    item->pcd=NULL;
	item->pos=NULL;
	item->nopcd=0;

    item->grpprl_count=0;
    item->cbGrpprl=NULL;
    item->grpprl=NULL;
    }


int wvGetPieceBoundsFC(U32 *begin,U32 *end,CLX *clx,U32 piececount)
	{
    int type;
    if ( (piececount+1) > clx->nopcd)
		{
		wvError("piececount is > nopcd, i.e.%d > %d\n", piececount+1,clx->nopcd);
        return(-1);
		}
    *begin = wvNormFC(clx->pcd[piececount].fc,&type);

    *end = *begin+(clx->pos[piececount+1]-clx->pos[piececount]);

    return(type);
    }

int wvGetPieceBoundsCP(U32 *begin,U32 *end,CLX *clx,U32 piececount)
	{
    if ( (piececount+1) > clx->nopcd)
        return(-1);
    *begin = clx->pos[piececount];
    *end = clx->pos[piececount+1];
    return(0);
    }


U16 wvAutoCharset(CLX *clx)
	{
	U16 i=0;
	int flag;
	U16 ret=ISO_5589_15;

	while(i<clx->nopcd)
        {
		wvNormFC(clx->pcd[i].fc,&flag);
		if (flag==0)
			{
			ret=UTF8;
			break;
			}
        i++;
        }

	return(ret);
	}

/*
1) search for the piece containing the character in the piece table.

2) Then calculate the FC in the file that stores the character from the piece
    table information.
*/
U32 wvConvertCPToFC(U32 currentcp,CLX *clx)
	{
	U32 currentfc=0xffffffffL;
	U32 i=0;

	while(i<clx->nopcd)
        {
//        fprintf(stderr,"%x %x %x\n",currentcp,clx->pos[i],clx->pos[i+1]);
        if ( (currentcp >= clx->pos[i]) && (currentcp < clx->pos[i+1]) )
            {
            currentfc = wvNormFC(clx->pcd[i].fc,NULL) + (currentcp-clx->pos[i]);
            break;
            }
        i++;
        }
	return(currentfc);
	}

int wvQuerySamePiece(U32 fcTest,CLX *clx,U32 piece)
	{
	/*
    if ( (fcTest > wvNormFC(clx->pcd[piece].fc,NULL)) && (fcTest < wvNormFC(clx->pcd[piece+1].fc,NULL)) )
	*/

    if ( (fcTest >= wvNormFC(clx->pcd[piece].fc,NULL)) && (fcTest < wvNormFC(clx->pcd[piece+1].fc,NULL)) )
		return(1);
    return(0);
	}


U32 wvGetPieceFromCP(U32 currentcp,CLX *clx)
	{
	U32 i=0;
	while(i<clx->nopcd)
		{
		if ( (currentcp >= clx->pos[i]) && (currentcp < clx->pos[i+1]) )
			return(i);
		i++;
		}
	wvError("cp was not in any piece ! \n",currentcp);
	return(-1);
	}
