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
//!  \file DwmDebPkgVersion.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::PkgVersion class implementation
//---------------------------------------------------------------------------

#include <cstdlib>
#include <regex>

#include "DwmDebPkgVersion.hh"

namespace Dwm {

  namespace Deb {

    using namespace std;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgVersion::FromString(const std::string & s)
    {
      _epoch = 0;
      _version.clear();
      _release.clear();

      bool   rc = false;
    
      static regex  full("([0-9]+)[:]([^-]+)\\-(.+)",
                         regex::ECMAScript|regex::optimize);
      smatch        sm;
      if (regex_match(s, sm, full)) {
        if (sm.size() == 4) {
          _epoch = atoi(sm[1].str().c_str());
          _version = sm[2].str();
          _release = sm[3].str();
          rc = true;
        }
      }
      else {
        static regex  epochvers("([0-9]+)[:]([^-]+)",
                                regex::ECMAScript|regex::optimize);
        if (regex_match(s, sm, epochvers)) {
          if (sm.size() == 3) {
            _epoch = atoi(sm[1].str().c_str());
            _version = sm[2].str();
            rc = true;
          }
        }
        else {
          static regex  versrelease("([^-]+)\\-(.+)",
                                    regex::ECMAScript|regex::optimize);
          if (regex_match(s, sm, versrelease)) {
            if (sm.size() == 3) {
              _version = sm[1].str();
              _release = sm[2].str();
              rc = true;
            }
          }
          else {
            if (! s.empty()) {
              _version = s;
              rc = true;
            }
          }
        }
      }
      return rc;
    }
  

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int PkgVersion::Epoch(int epoch)
    {
      _epoch = epoch;
      return _epoch;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const VersionString & PkgVersion::Version(const VersionString & version)
    {
      _version = version;
      return _version;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    const std::string & PkgVersion::Release(const std::string & release)
    {
      _release = release;
      return _release;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgVersion::operator > (const PkgVersion & dpv) const
    {
      bool  rc = false;
      if (_epoch > dpv._epoch) {
        rc = true;
      }
      else if (_epoch == dpv._epoch) {
        if (_version > dpv._version) {
          rc = true;
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgVersion::operator < (const PkgVersion & dpv) const
    {
      bool  rc = false;
      if (_epoch < dpv._epoch) {
        rc = true;
      }
      else if (_epoch == dpv._epoch) {
        if (_version < dpv._version) {
          rc = true;
        }
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgVersion::operator == (const PkgVersion & dpv) const
    {
      return ((_epoch == dpv._epoch)
              && (_version == dpv._version)
              && (_release == dpv._release));
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool PkgVersion::operator != (const PkgVersion & dpv) const
    {
      return ((_epoch != dpv._epoch)
              || (_version != dpv._version)
              || (_release != dpv._release));
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::ostream & operator << (std::ostream & os,
                                const PkgVersion & version)
    {
      if (version._epoch) {
        os << version._epoch << ':';
      }
      os << version._version;
      if (! version._release.empty()) {
        os << "-" << version._release;
      }
      return os;
    }

  }  // namespace Deb
  
}  // namespace Dwm
