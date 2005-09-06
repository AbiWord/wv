#ifndef WV_PLUSPLUS_H
#define WV_PLUSPLUS_H

extern "C" {
  #include "wv.h"
}

#include <string>

namespace wv
{
  class Library
    {
    public:
      static void Init ();
      static void Term ();      

    private:
      Library ();
    } ;

  class Exception
    {
    public:
      Exception ( const std::string & inMsg )
	: mMsg ( inMsg ) {}

      virtual ~Exception () {}

      std::string GetMessage () const { return mMsg; }

    private:
      Exception ();

      std::string mMsg;
    } ;

  class InvalidDocumentException : public Exception
    {
    public:
      InvalidDocumentException ( const std::string & inMsg )
	: Exception ( inMsg ) {}

      virtual ~InvalidDocumentException () {}

    private:
      InvalidDocumentException ();
    };

  class PasswordException : public Exception
    {
    public:
      PasswordException ( const std::string & inMsg )
	: Exception ( inMsg ) {}

      virtual ~PasswordException () {}

    private:
      PasswordException ();
    } ;

  class Parser
    {
    public:
      Parser ( const std::string & inPasswd );
      Parser ();

      virtual ~Parser ();

      void Process ( const std::string & inFileName )
	throw ();   

    protected:

      /*!
       * Return the current valid parse struct or null
       */
      inline wvParseStruct * GetParseStruct ()
	{
	  return mPs;
	}

      /*!
       * Append this character to the current character buffer
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual void AppendChar ( U16 ch )
	{
	}

      /*!
       * Flush the current character buffer
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual void Flush ()
	{
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool SpecialCharProc (U16 eachchar, CHP *achp)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool BeginDoc ()
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool EndDoc ()
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool BeginSection (SEP * asep, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool EndSection (SEP * asep, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool BeginParagraph (PAP * apap, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool EndParagraph (PAP * apap, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool BeginSpan (CHP * achp, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool EndSpan (CHP * achp, bool dirty)
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool BeginComment ()
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool EndComment ()
	{
	  return true;
	}

      /*!
       *
       * DO NOTHING IMPL!! OVERRIDE!!
       */
      virtual bool FieldProc (U16 eachChar, U8 charType, U16 lid)
	{
	  return true;
	}

    private:
      Parser ( const Parser & other ); // no impl
      Parser & operator= ( const Parser & other ); // no impl

      /*!
       * Will append the current character via AppendChar or invoke
       * FieldProc as applicable
       */
      bool CharProc (U16 eachChar, U8 charType, U16 lid);

      // wv callbacks: the private static methods
      static int CallbackCharProc ( wvParseStruct * ps, U16 eachChar, 
				    U8 charType, U16 lid );
      static int CallbackSpecCharProc ( wvParseStruct * ps, U16 eachChar, 
					CHP * achp );
      static int CallbackEleProc ( wvParseStruct * ps, wvTag tag, void * props,
				   int dirty );
      static int CallbackDocProc ( wvParseStruct * ps, wvTag tag );

      // for document parsing stuff
      wvParseStruct * mPs;
      std::string mPassword;
    } ;
}

#endif // WV_PLUSPLUS_H
