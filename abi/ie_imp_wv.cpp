/* AbiWord
 * Copyright (C) 1998 AbiSource, Inc.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ut_types.h"
#include "ut_assert.h"
#include "ut_debugmsg.h"
#include "ut_string.h"
#include "ie_imp_wv.h"
#include "ie_types.h"
#include "pd_Document.h"
#include "ut_bytebuf.h"

#define X_ReturnIfFail(exp,ies)     do { UT_Bool b = (exp); if (!b) return (ies); } while (0)
#define X_ReturnNoMemIfError(exp)   X_ReturnIfFail(exp,IES_NoMemory)

#define X_CheckError(v)         do {  if (!(v))                             \
                                      {  m_iestatus = IES_Error;            \
                                         return; } } while (0)


int CharProc(wvParseStruct *ps,U16 eachchar,U8 chartype);
int ElementProc(wvParseStruct *ps,wvTag tag, void *props);
int DocProc(wvParseStruct *ps,wvTag tag);



/*****************************************************************/

IEStatus IE_Imp_wv::importFile(const char * szFilename)
{
	wvStream *fp = NULL;
	char buf[4096];
	fprintf(stderr,"got to import file\n");

	fp = fopen(szFilename, "rb");
	if (!fp)
	{
		UT_DEBUGMSG(("Could not open file %s\n",szFilename));
		m_iestatus = IES_FileNotFound;
	}
	fprintf(stderr,"wv importer\n");

	wvParseStruct ps;
	if (wvInitParser(&ps,fp))
		{
		UT_DEBUGMSG(("Could not open file %s\n",szFilename));
		m_iestatus = IES_BogusDocument;
		return m_iestatus;
		}
	
	fprintf(stderr,"just here\n");
	ps.userData = this;
	wvSetElementHandler(ElementProc);
	wvSetCharHandler(CharProc);
	wvSetDocumentHandler(DocProc);

	wvText(&ps);

	m_iestatus = IES_OK;

	return m_iestatus;
}

int CharProc(wvParseStruct *ps,U16 eachchar,U8 chartype)
	{
	IE_Imp_wv* pDocReader = (IE_Imp_wv *) ps->userData;
	UT_UCSChar *pbuf=&eachchar;
	fprintf(stderr,"word 97 char is %c, type is %d\n",eachchar,chartype);
	return(pDocReader->_charData(&eachchar, 1));
	}

int DocProc(wvParseStruct *ps,wvTag tag)
	{
	IE_Imp_wv* pDocReader = (IE_Imp_wv *) ps->userData;
	return(pDocReader->_docProc(ps, tag));
	}

int ElementProc(wvParseStruct *ps,wvTag tag,void *props)
	{
	IE_Imp_wv* pDocReader = (IE_Imp_wv *) ps->userData;
	return(pDocReader->_eleProc(ps, tag, props));
	fprintf(stderr,"ele begins\n");
	return(0);
	}

int IE_Imp_wv::_charData(U16 *charstr, int len)
	{
	UT_UCSChar buf[1];
	buf[0] = charstr[0];
	X_CheckError(m_pDocument->appendSpan(buf,1));
	return(0);
	}

int IE_Imp_wv::_docProc(wvParseStruct *ps,wvTag tag)
	{
	switch(tag)
		{
		case DOCBEGIN:
			X_ReturnNoMemIfError(m_pDocument->appendStrux(PTX_Section, NULL));
			break;
		case DOCEND:	
			/*abiword doesn't need this*/
			break;
		}
	return(0);
	}

int IE_Imp_wv::_eleProc(wvParseStruct *ps,wvTag tag, void *props)
	{
	XML_Char propBuffer[1024];
	XML_Char* pProps = "PROPS";
	const XML_Char* propsArray[3];

	propBuffer[0] = 0;
	fprintf(stderr," started\n");
	PAP *apap;
	CHP *achp;
	   
	switch(tag)
		{
		case PARABEGIN:
                        apap = (PAP*)props;
		        strcat(propBuffer, "text-align:");
			switch(apap->jc)
				{
				case 0:
					strcat(propBuffer, "left");
					break;
				case 1:
					strcat(propBuffer, "center");
					break;
				case 2:
					strcat(propBuffer, "right");
					break;
				case 3:
					strcat(propBuffer, "justify");
					break;
				case 4:			
				/* this type of justification is of unknown purpose and is 
				undocumented , but it shows up in asian documents so someone
				should be able to tell me what it is someday C. */
					strcat(propBuffer, "justify");
					break;
				}
			//strcat(propBuffer, "; ");
			propsArray[0] = pProps;
			propsArray[1] = propBuffer;
			propsArray[2] = NULL;
			fprintf(stderr,"the propBuffer is %s\n",propBuffer);
			X_ReturnNoMemIfError(m_pDocument->appendStrux(PTX_Block, propsArray));
			/*
			X_ReturnNoMemIfError(m_pDocument->appendStrux(PTX_Block,NULL));
			*/
			break;
		case CHARPROPBEGIN:
		   achp = (CHP*)props;
		   if (achp->fBold) { 
		      strcat(propBuffer, "font-weight:bold;");
		   } else {
		      strcat(propBuffer, "font-weight:normal;");
		   }
		   if (achp->fItalic) {
		      strcat(propBuffer, "font-style:italic;");
		   } else {
		      strcat(propBuffer, "font-style:normal;");
		   }
		   propsArray[0] = pProps;
		   // remove trailing ;
		   propBuffer[strlen(propBuffer)-1] = 0;
		   propsArray[1] = propBuffer;
		   propsArray[2] = NULL;
		   fprintf(stderr,"the propBuffer is %s\n",propBuffer);
		   X_ReturnNoMemIfError(m_pDocument->appendFmt(propsArray));
		   break;
		case PARAEND:	/* not needed */
		case CHARPROPEND: /* not needed */
		default:
		   break;
		}
	fprintf(stderr,"ended\n");
	return(0);
	}


/*****************************************************************/

IE_Imp_wv::~IE_Imp_wv()
{
}

IE_Imp_wv::IE_Imp_wv(PD_Document * pDocument)
	: IE_Imp(pDocument)
{
	fprintf(stderr,"constructed wv\n");
	m_iestatus = IES_OK;
}

/*****************************************************************/
/*****************************************************************/

UT_Bool IE_Imp_wv::RecognizeSuffix(const char * szSuffix)
{
	return (UT_stricmp(szSuffix,".doc") == 0);
}

IEStatus IE_Imp_wv::StaticConstructor(PD_Document * pDocument,
											 IE_Imp ** ppie)
{
	IE_Imp_wv * p = new IE_Imp_wv(pDocument);
	*ppie = p;
	return IES_OK;
}

UT_Bool	IE_Imp_wv::GetDlgLabels(const char ** pszDesc,
									   const char ** pszSuffixList,
									   IEFileType * ft)
{
	*pszDesc = "wv (.doc)";
	*pszSuffixList = "*.doc";
	*ft = IEFT_wv;
	return UT_TRUE;
}

UT_Bool IE_Imp_wv::SupportsFileType(IEFileType ft)
{
	return (IEFT_wv == ft);
}
