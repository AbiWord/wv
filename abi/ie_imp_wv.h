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


#ifndef IE_IMP_WV_H
#define IE_IMP_WV_H

#include <stdlib.h>
#include <stdio.h>
#include "wv.h"
#include "xmlparse.h"
#include "ut_vector.h"
#include "ut_stack.h"
#include "ie_imp.h"
#include "ut_bytebuf.h"
class PD_Document;

// The importer/reader for Microsoft Word 97

class IE_Imp_wv : public IE_Imp
{
public:
	IE_Imp_wv(PD_Document * pDocument);
	~IE_Imp_wv();

	IEStatus			importFile(const char * szFilename);
	static UT_Bool		RecognizeSuffix(const char * szSuffix);
	static IEStatus		StaticConstructor(PD_Document * pDocument,
										  IE_Imp ** ppie);
	static UT_Bool		GetDlgLabels(const char ** pszDesc,
									 const char ** pszSuffixList,
									 IEFileType * ft);
	static UT_Bool 		SupportsFileType(IEFileType ft);
	int					_charData(U16 *, int);
	int 				_docProc(wvParseStruct *ps,wvTag tag);
	int 				_eleProc(wvParseStruct *ps,wvTag tag,void *props);
protected:
	IEStatus			m_iestatus;
};

#endif /* IE_IMP_WV_H */
