//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2020
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
//===========================================================================

//---------------------------------------------------------------------------
//!  \file DwmDebPkgDepend.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::PkgDepend class implementation
//---------------------------------------------------------------------------

#include <cstdio>
#include <regex>

#include "DwmDebPkgDepend.hh"

namespace Dwm {

  namespace Deb {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & PkgDepend::Package(const string & package)
    {
      _pkg = package;
      return _pkg;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const string & PkgDepend::Operator(const string & op)
    {
      _operator = op;
      return _operator;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const PkgVersion & PkgDepend::Version(const PkgVersion & dpv)
    {
      _version = dpv;
      return _version;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    PkgVersion PkgDepend::InstalledVersion(const string & pkg)
    {
      PkgVersion  rc;
    
      string  cmd("dpkg -s ");
      cmd += pkg;
      FILE  *cmdpipe = popen(cmd.c_str(), "r");
      if (cmdpipe) {
        regex   rgx("^Version[:][ \\t]*([^ \\t\\n]+)",
                    regex::ECMAScript|regex::optimize);
        smatch  sm;
        char    line[4096];
        while (fgets(line, 4096, cmdpipe) != NULL) {
          string  s(line);
          if (regex_search(s, sm, rgx)) {
            if (sm.size() == 2) {
              rc.FromString(sm[1].str());
              break;
            }
          }
        }
        pclose(cmdpipe);
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgDepend::operator < (const PkgDepend & dpd) const
    {
      bool  rc = false;
      if (_pkg < dpd._pkg) {
        rc = true;
      }
      else if (_pkg == dpd._pkg) {
        if (_operator < dpd._operator) {
          rc = true;
        }
        else if (_operator == dpd._operator) {
          if (_version < dpd._version) {
            rc = true;
          }
        }
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgDepend::operator == (const PkgDepend & dpd) const
    {
      return ((_pkg == dpd._pkg)
              && (_operator == dpd._operator)
              && (_version == dpd._version));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ostream & operator << (ostream & os, const PkgDepend & dep)
    {
      os << dep._pkg;
      if (! dep._operator.empty()) {
        os << " (" << dep._operator << ' ' << dep._version << ')';
      }
      return os;
    }

  }  // namespace Deb
  
}  // namespace Dwm
