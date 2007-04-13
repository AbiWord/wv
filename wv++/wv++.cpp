/*!
 * By Dom Lachowicz <doml@appligent.com>
 */

#include "wv++.h"

namespace wv
{
  /*!
   * This chunk of code initializes and terminates the wv library
   *
   */

  void Library::Init ()
  {
    wvInit ();
  }

  void Library::Term ()
  {
    wvShutdown ();
  }

}

namespace wv
{
  Parser::Parser ( const std::string & inPasswd )
    : mPs(0), mPassword ( inPasswd )
  {
  }

  Parser::Parser ()
    : mPs(0)
  {
  }

  Parser::~Parser ()
  {
  }

  void Parser::Process ( const std::string & inFileName )
    throw ()   
  {
    wvParseStruct ps;
    int ret = wvInitParser (&ps, inFileName.c_str());
    
    // initialized the parser but it was password protected
    if (ret & 0x8000)
      {
	if ( ( ret & 0x7fff ) == WORD8 )
	  {
	    if ( 0 == mPassword.length () )
	      {
		throw PasswordException ( "Document needs passwd" );
	      }
	    else
	      {
		wvSetPassword ( mPassword.c_str(), &ps );
		if ( wvDecrypt97 ( &ps ) )
		  {
		    throw PasswordException ( "bad passwd" );
		  }
	      }
	  }
	else if (((ret & 0x7fff) == WORD7) || ((ret & 0x7fff) == WORD6))
	  {
	    ret = 0;
	    if ( 0 == mPassword.length() )
	      {
		      throw PasswordException ( "Doc needs passwd" );
	      }
	    else
	      {
		wvSetPassword ( mPassword.c_str(), &ps );
		if (wvDecrypt95 ( &ps ) )
		  {
		    throw PasswordException ( "bogus passwd" );
		  }
	      }
	  }		  
      }
    else if (ret != 0)
      {
	throw InvalidDocumentException ("This document is either not a MSWord document or is of an unsupported version");
      }
    
    // register ourself as the user-data
    ps.userData = this;
    ps.filename = const_cast<char *>(inFileName.c_str());
    
    // register callbacks
    wvSetElementHandler ( &ps, CallbackEleProc );
    wvSetCharHandler ( &ps, CallbackCharProc );
    wvSetSpecialCharHandler ( &ps, CallbackSpecCharProc );
    wvSetDocumentHandler ( &ps, CallbackDocProc );
    
    // run the processing loop
    mPs = &ps;
    wvText (&ps);
    mPs = 0;
    
    wvOLEFree (&ps);
  }
 
  bool Parser::CharProc (U16 eachChar, U8 charType, U16 lid)
  {
    // convert incoming character to unicode
    if ( charType )
      eachChar = wvHandleCodePage ( eachChar, lid );
    
    //
    // now do any necessary manual character conversions
    // and anything else needed for field-handling
    //
    switch ( eachChar )
      {
      case 19: // field begin
	this->Flush ();
	mPs->fieldstate++;
	mPs->fieldmiddle = 0;
	this->FieldProc (eachChar, charType, lid); 
	return true;
	
      case 20: // field separator
	this->FieldProc (eachChar, charType, lid);
	mPs->fieldmiddle = 1;
	return true;
	
      case 21: // field end
	mPs->fieldstate--;
	mPs->fieldmiddle = 0;
	this->FieldProc (eachChar, charType, lid);
	return true;
	
      default:
	break;
      }
    
    // TODO: i'm not sure if this is needed any more
    if (mPs->fieldstate)
      {
	if(this->FieldProc (eachChar, charType, lid))
	  return true;
      }
    
    //
    // append the character to our character buffer
    //
    this->AppendChar (eachChar);
    return true;
  }

  int Parser::CallbackCharProc ( wvParseStruct * ps, U16 eachChar, 
				 U8 charType, U16 lid )
  {
    Parser * pParser = static_cast <Parser *> ( ps->userData );
    return pParser->CharProc ( eachChar, charType, lid ) ? 0 : 1;
  }
      
  int Parser::CallbackSpecCharProc ( wvParseStruct * ps, U16 eachChar, 
				     CHP * achp )
  {
    Parser * pParser = static_cast <Parser *> ( ps->userData );
    return pParser->SpecialCharProc (eachChar, achp) ? 0 : 1;
  }

  int Parser::CallbackEleProc ( wvParseStruct * ps, wvTag tag, void * props, 
				int dirty )
  {
    Parser * pParser = static_cast <Parser *> ( ps->userData );
    switch (tag)
      {
      case SECTIONBEGIN:
	return pParser->BeginSection (static_cast<SEP*>(props), dirty) ? 0 : 1;
      case SECTIONEND:
	return pParser->EndSection(static_cast<SEP*>(props), dirty) ? 0 : 1;
      case PARABEGIN:
	return pParser->BeginParagraph(static_cast<PAP*>(props), dirty) ? 0 : 1;
      case PARAEND:
	return pParser->EndParagraph(static_cast<PAP*>(props), dirty) ? 0 : 1;
      case CHARPROPBEGIN:
	return pParser->BeginSpan(static_cast<CHP*>(props), dirty) ? 0 : 1;
      case CHARPROPEND:
	return pParser->EndSpan(static_cast<CHP*>(props), dirty) ? 0 : 1;
      case COMMENTBEGIN:
	return pParser->BeginComment() ? 0 : 1;
      case COMMENTEND:
	return pParser->EndComment() ? 0 : 1;
      default:
	return 1;
      }
  }
  
  int Parser::CallbackDocProc ( wvParseStruct * ps, wvTag tag )
  {
    Parser * pParser = static_cast <Parser *> ( ps->userData );
    
    switch (tag)
      {
      case DOCBEGIN:
	return pParser->BeginDoc () ? 0 : 1;
      case DOCEND:
	return pParser->EndDoc () ? 0 : 1;
      default:
	return 1;
      }
  }

}
