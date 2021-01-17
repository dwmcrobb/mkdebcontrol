%code requires
{
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
  //!  \file DwmDebControlParser.y
  //!  \author Daniel W. McRobb
  //!  \brief Debian control file parser
  //-------------------------------------------------------------------------

  #include <iostream>
  #include <string>
  #include <utility>
  #include <set>

  #include "DwmDebControl.hh"
  #include "DwmDebPkgDepend.hh"
  #include "DwmDebPkgVersion.hh"
    
  using std::pair, std::string, std::set;
}

%{
  #include <cstdio>
  #include <cstdlib>
    
  extern "C" {
    extern void dwmdebctrlerror(const char *arg, ...);
    extern FILE *dwmdebctrlin;
  }

  #include <mutex>
  #include <string>

  #include "DwmDebControl.hh"
  #include "DwmDebPkgDepend.hh"

  using namespace std;
  
  string              g_debPath;
  Dwm::Deb::Control  *g_debctrl = nullptr;
  static mutex      g_mtx;
  
%}

%define api.prefix {dwmdebctrl}

%union {
  string                    *stringVal;
  pair<string,string>       *stringPairVal;
  Dwm::Deb::PkgDepend       *dwmDebPkgDependVal;
  Dwm::Deb::PkgVersion      *dwmDebPkgVersionVal;
  set<Dwm::Deb::PkgDepend>  *dwmDebPkgDependSetVal;
}

%code provides
{
  // Tell Flex the expected prototype of yylex.
  #define YY_DECL                             \
    int dwmdebctrllex ()

  // Declare the scanner.
  YY_DECL;
}

%token DEPENDS PREDEPENDS
%token<stringVal> FIELDNAME STRING UNKNOWNFIELDNAME

%type<stringVal>             Values VersionOperator
%type<stringPairVal>         Field
%type<dwmDebPkgVersionVal>   PackageVersion
%type<dwmDebPkgDependVal>    DependPackage
%type<dwmDebPkgDependSetVal> Depends PreDepends DependPackages

%%

Fields: Field
{
  g_debctrl->Add(*$1);
  //  std::cout << $1->first << " " << $1->second  << '\n';
  delete $1;
}
| Depends
{
  g_debctrl->AddDepends(*$1);
  delete $1;
}
| PreDepends
{
  g_debctrl->AddDepends(*$1);
  delete $1;
}
| Fields Field
{
  g_debctrl->Add(*$2);
  //  std::cout << $2->first << " " << $2->second << '\n';
  delete $2;
}
| Fields Depends
{
  g_debctrl->AddDepends(*$2);
  delete $2;
}
| Fields PreDepends
{
  g_debctrl->AddPreDepends(*$2);
  delete $2;
};

Field: FIELDNAME Values
{
  $$ = new std::pair<std::string,std::string>(*$1,*$2);
  delete $1;
  delete $2;
}
| UNKNOWNFIELDNAME Values
{
  $$ = new std::pair<std::string,std::string>(*$1,*$2);
  delete $1;
  delete $2;
};

Values: STRING
{
  $$ = new std::string();
  (*$$) = *$1;
  delete $1;
}
| Values STRING
{
  (*$$) += *$2;
  delete $2;
};

Depends: DEPENDS DependPackages
{
  $$ = $2;
};

PreDepends: PREDEPENDS DependPackages
{
  $$ = $2;
};

DependPackages: DependPackage
{
  $$ = new std::set<Dwm::Deb::PkgDepend>();
  $$->insert(*$1);
  delete $1;
}
| DependPackages ',' DependPackage
{
  $$->insert(*$3);
  delete $3;
};

DependPackage: STRING
{
  $$ = new Dwm::Deb::PkgDepend(*$1);
  delete $1;
}
| STRING '(' VersionOperator PackageVersion ')'
{
    $$ = new Dwm::Deb::PkgDepend(*$1, *$3, *$4);
    delete $1;
    delete $3;
    delete $4;
};

VersionOperator: '>' '>'
{
  $$ = new std::string(">>");
}
| '<' '<'
{
  $$ = new std::string("<<");
}
| '>' '='
{
  $$ = new std::string(">=");
}
| '<' '='
{
  $$ = new std::string("<=");
}
| '='
{
  $$ = new std::string("=");
};

PackageVersion: STRING ':' STRING
{
  $$ = new Dwm::Deb::PkgVersion(atoi($1->c_str()), *$3);
  delete $1;
  delete $3;
}
| STRING
{
  $$ = new Dwm::Deb::PkgVersion(*$1);
  delete $1;
};

%%

namespace Dwm {

  namespace Deb {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Control::Control()
        : _entries(), _predepends(), _depends()
    {}
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Control::Parse(const string & path)
    {
      lock_guard<mutex>  lck(g_mtx);
      
      bool  rc = false;
      dwmdebctrlin = fopen(path.c_str(), "r");
      if (dwmdebctrlin) {
        g_debPath = path;
        g_debctrl = this;
        rc = (0 == dwmdebctrlparse());
        fclose(dwmdebctrlin);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Control::HasRequiredEntries() const
    {
      static const vector<string>  requiredFields = {
        "Package:",
        "Version:",
        "Architecture:",
        "Maintainer:",
        "Description:"
      };
      bool  rc = true;
      for (const auto & rf : requiredFields) {
        if (_entries.find(rf) == _entries.end()) {
          rc = false;
          break;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::Add(const std::pair<std::string,std::string> & entry)
    {
      _entries[entry.first] = entry.second;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::RemoveDepend(const PkgDepend & dep)
    {
      _depends.erase(dep);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::RemovePreDepend(const PkgDepend & dep)
    {
      _predepends.erase(dep);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::AddDepend(const PkgDepend & dep)
    {
      _depends.insert(dep);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::AddPreDepend(const PkgDepend & dep)
    {
      _predepends.insert(dep);
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::AddDepends(const std::set<PkgDepend> & depends)
    {
      for (const auto & d : depends) {
        _depends.insert(d);
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Control::AddPreDepends(const std::set<PkgDepend> & depends)
    {
      for (const auto & d : depends) {
        _predepends.insert(d);
      }
      return;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & operator << (std::ostream & os,
                                const Control & debctrl)
    {
      for (const auto & e : debctrl._entries) {
        os << e.first << ' ' << e.second << '\n';
      }
      if (! debctrl._predepends.empty()) {
        os << "Pre-Depends: ";
        string sep;
        for (const auto & predep : debctrl._predepends) {
          os << sep << predep;
          sep = ", ";
        }
        os << '\n';
      }

      if (! debctrl._depends.empty()) {
        os << "Depends: ";
        string sep;
        for (const auto & dep : debctrl._depends) {
          os << sep << dep;
          sep = ", ";
        }
        os << '\n';
      }
      return os;
    }

  }  // namespace Deb
  
}  // namespace Dwm
