%{
  //=========================================================================
  // @(#) $DwmPath$
  //=========================================================================
  //  Copyright (c) Daniel W. McRobb 2021
  //  All rights reserved.
  //
  //  Redistribution and use in source and binary forms, with or without
  //  modification, are permitted provided that the following conditions
  //  are met:
  //
  //  1. Redistributions of source code must retain the above copyright
  //     notice, this list of conditions and the following disclaimer.
  //  2. Redistributions in binary form must reproduce the above copyright
  //     notice, this list of conditions and the following disclaimer in the
  //     documentation and/or other materials provided with the distribution.
  //  3. The names of the authors and copyright holders may not be used to
  //     endorse or promote products derived from this software without
  //     specific prior written permission.
  //
  //  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
  //  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
  //  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
  //  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
  //  DAMAGE.
  //
  //  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
  //  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
  //  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
  //  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
  //  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  //  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
  //  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
  //  TRADEMARK OR OTHER RIGHTS.
  //=========================================================================

  //-------------------------------------------------------------------------
  //!  \file DwmDebControlLexer.lex
  //!  \author Daniel W. McRobb
  //!  \brief Debian control file lexer
  //-------------------------------------------------------------------------


  #include <map>
  #include <string>

  #include "DwmDebControlParser.hh"

  extern std::string  g_debPath;
  
  extern "C" {
    #include <stdarg.h>

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void dwmdebctrlerror(const char *arg, ...)
    {
      va_list  ap;
      va_start(ap, arg);
      vfprintf(stderr, arg, ap);
      fprintf(stderr, ": '%s' at line %d of %s\n",
              yytext, yylineno, g_debPath.c_str());
      return;
    }
  }

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static const std::map<std::string,int>  g_fieldNames = {
    { "Architecture:",      FIELDNAME  },
    { "Breaks:",            FIELDNAME  },
    { "Build-Depends:",     FIELDNAME  },
    { "Conflicts:",         FIELDNAME  },
    { "Depends:",           DEPENDS    },
    { "Description:",       FIELDNAME  },
    { "Enhances:",          FIELDNAME  },
    { "Homepage:",          FIELDNAME  },
    { "Maintainer:",        FIELDNAME  },
    { "Package:",           FIELDNAME  },
    { "Pre-Depends:",       PREDEPENDS },
    { "Priority:",          FIELDNAME  },
    { "Recommends:",        FIELDNAME  },
    { "Replaces:",          FIELDNAME  },
    { "Section:",           FIELDNAME  },
    { "Standards-Version:", FIELDNAME  },
    { "Suggests:",          FIELDNAME  },
    { "Version:",           FIELDNAME  }
  };

  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  static bool IsKnownField(const std::string & s, int & field)
  {
    bool  rc = false;
    auto  iter = g_fieldNames.find(s);
    if (iter != g_fieldNames.end()) {
      field = iter->second;
      rc = true;
    }
    return rc;
  }

%}

%option noyywrap
%option prefix="dwmdebctrl"
%option yylineno

%x x_colon
%x x_dependList
%x x_predepends
%x x_value
   
%%

<INITIAL>^[^ \t\n:]+[:]           { int token;
                                    if (IsKnownField(yytext, token)) {
                                      if ((token == DEPENDS)
                                          || (token == PREDEPENDS)) {
                                        BEGIN(x_dependList);
                                      }
                                      else {
                                        dwmdebctrllval.stringVal =
                                          new std::string(yytext);
                                        BEGIN(x_value);
                                      }
                                      return token;
                                    }
                                    else {
                                      dwmdebctrllval.stringVal =
                                        new std::string(yytext);
                                      if (dwmdebctrllval.stringVal->back()
                                          == ':') {
                                        dwmdebctrllval.stringVal->pop_back();
                                      }
                                      BEGIN(x_value);
                                      return UNKNOWNFIELDNAME;
                                    }
                                  }
<x_dependList>[^ \t\n,()><=]+     { dwmdebctrllval.stringVal =
                                      new std::string(yytext);
                                    return STRING;
                                  }
<x_dependList>[,()><=]            { return yytext[0]; }
<x_dependList>[\n][ \t]+
<x_dependList>[\n]/[^ \t]+        { BEGIN(INITIAL); }
<x_dependList>[ \n]
<x_value>[^ \n][^\n]*/[\n]        { dwmdebctrllval.stringVal =
                                      new std::string(yytext);
                                    return STRING;
                                  }
<x_value>[\n][ \t]+               { dwmdebctrllval.stringVal =
                                      new std::string(yytext);
                                    return STRING;
                                  }
<x_value>[\n]/[^ \t]              { BEGIN(INITIAL); }
<x_value>[ \n]
<INITIAL>[ \n]

%%

                                  
