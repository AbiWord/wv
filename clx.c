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

void wvBuildCLXForSimple6(CLX *clx,FIB *fib)
	{
	wvInitCLX(clx);
    clx->nopcd=1;;

	clx->pcd = (PCD *) malloc(clx->nopcd * sizeof(PCD));
	clx->pos = (U32 *) malloc( (clx->nopcd+1) * sizeof(U32));
	
    clx->pos[0] = 0;
    clx->pos[1] = fib->ccpText;

	wvInitPCD(&(clx->pcd[0]));			
	clx->pcd[0].fc = fib->fcMin;

	/* reverse the special encoding thing they do for word97 */
    clx->pcd[0].fc *= 2;
	clx->pcd[0].fc |= 0x40000000UL;

	clx->pcd[0].prm.fComplex = 0;
	clx->pcd[0].prm.para.var1.isprm = 0;
	/*
	these set the one that *I* use correctly, but may break for other wvware
	users, though i doubt it, im just marking a possible firepoint for the
	future
	*/
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
void wvGetCLX(int version,CLX *clx,U32 offset,U32 len,FILE *fd)
	{
	U8 clxt;
	U16 cb;
	U32 lcb,i,j=0;

	fseek(fd,offset,SEEK_SET);
	wvTrace("clx offset is %x, len is %d\n",offset,len);

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
			if (version == 0)
				{
				lcb = read_32ubit(fd);
				j+=4;
				}
			else
				{
				wvTrace("Here so far\n");
#if 0
				lcb = read_16ubit(fd);		/* word 6 only has two bytes here */
				j+=2;
#endif

				lcb = read_32ubit(fd);		/* word 6 specs appeared to have lied ! */
				j+=4;
				}
			wvGetPCD_PLCF(&clx->pcd,&clx->pos,&clx->nopcd,ftell(fd),lcb,fd);
			j+=lcb;

			if (version)
				for (i=0;i<clx->nopcd;i++)
					{
					clx->pcd[i].fc *= 2;
					clx->pcd[i].fc |= 0x40000000UL;
					}
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



int wvQuerySamePiece(U32 fcTest,CLX *clx,U32 piece)
	{
	/*
	wvTrace("Same Piece, %x %x %x\n",fcTest,wvNormFC(clx->pcd[piece].fc,NULL),wvNormFC(clx->pcd[piece+1].fc,NULL));
    if ( (fcTest >= wvNormFC(clx->pcd[piece].fc,NULL)) && (fcTest < wvNormFC(clx->pcd[piece+1].fc,NULL)) )
	*/
    if ( (fcTest >= wvNormFC(clx->pcd[piece].fc,NULL)) && (fcTest < wvGetEndFCPiece(piece,clx)) )
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
	wvTrace("cp was not in any piece ! \n",currentcp);
	return(0xffffffffL);
	}

U32 wvGetPieceFromFC(U32 currentfc,CLX *clx)
	{
	U32 i=0;
	while(i<clx->nopcd)
		{
		if ( (currentfc >= clx->pos[i]) && (currentfc < clx->pos[i+1]) )
			return(i);
		i++;
		}
	wvTrace("fc was not in any piece ! \n",currentfc);
	return(0xffffffffL);
	}

U32 wvGetEndFCPiece(U32 piece,CLX *clx)
	{
	int flag;
	U32 fc;
	U32 offset = clx->pos[piece+1] - clx->pos[piece];
	wvTrace("offset is %x\n",offset);
	fc = wvNormFC(clx->pcd[piece].fc,&flag);
	wvTrace("fc is %x, flag %d\n",fc,flag);
	if (flag) fc+=offset;
	else fc+=offset*2;
	wvTrace("fc is finally %x\n",fc);
	return(fc);
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
	int flag;

	while(i<clx->nopcd)
        {
        if ( (currentcp >= clx->pos[i]) && (currentcp < clx->pos[i+1]) )
            {
            currentfc = wvNormFC(clx->pcd[i].fc,&flag);
			if (flag) currentfc+=(currentcp-clx->pos[i]);
			else currentfc+=((currentcp-clx->pos[i])*2);
            break;
            }
        i++;
        }

	if (currentfc == 0xffffffffL)
		{
		i--;
		currentfc = wvNormFC(clx->pcd[i].fc,&flag);
		if (flag) currentfc+=(currentcp-clx->pos[i]);
		else currentfc+=((currentcp-clx->pos[i])*2);
		}

	return(currentfc);
	}
