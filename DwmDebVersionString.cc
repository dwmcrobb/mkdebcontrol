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
//!  \file DwmDebVersionString.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::VersionString and Dwm::Deb::VersionPart class
//!    implementations
//---------------------------------------------------------------------------

#include <regex>

#include "DwmDebVersionString.hh"

namespace Dwm {

  namespace Deb {

    using namespace std;
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionPart::VersionPart(const string & part)
    {
      size_t  i = 0;
      while (i < part.size()) {
        string  intstr;
        for ( ; (i < part.size()) && isdigit(part[i]); ++i) {
          intstr += part[i];
        }
        if (! intstr.empty()) {
          _data.push_back(stoul(intstr));
        }
        string  sstr;
        for ( ; (i < part.size()) && (! isdigit(part[i])); ++i) {
          sstr += part[i];
        }
        if (! sstr.empty()) {
          _data.push_back(sstr);
        }
      }
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionPart & VersionPart::operator = (const std::string & vp)
    {
      _data.clear();
      size_t  i = 0;
      while (i < vp.size()) {
        string  intstr;
        for ( ; (i < vp.size()) && isdigit(vp[i]); ++i) {
          intstr += vp[i];
        }
        if (! intstr.empty()) {
          _data.push_back(stoul(intstr));
        }
        string  sstr;
        for ( ; (i < vp.size()) && (! isdigit(vp[i])); ++i) {
          sstr += vp[i];
        }
        if (! sstr.empty()) {
          _data.push_back(sstr);
        }
      }
      return *this;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionPart::operator < (const VersionPart & vpp) const
    {
      return (_data < vpp._data);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionPart::operator > (const VersionPart & vpp) const
    {
      return (_data > vpp._data);
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionPart::operator == (const VersionPart & vpp) const
    {
      return (_data == vpp._data);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionPart::operator != (const VersionPart & vpp) const
    {
      return (_data != vpp._data);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionPart::operator std::string () const
    {
      string  rc;
      for (const auto & p : _data) {
        if (p.index()) {
          rc += get<1>(p);
        }
        else {
          rc += to_string(get<0>(p));
        }
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ostream & operator << (ostream & os, const VersionPart & vpp)
    {
      for (const auto & p : vpp._data) {
        if (p.index()) {
          os << get<1>(p);
        }
        else {
          os << get<0>(p);
        }
      }
      return os;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void VersionPart::clear()
    {
      _data.clear();
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionString::VersionString()
        : _version()
    {}
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionString::VersionString(const string & s)
    {
      static const regex  rgx("[^.]+", regex::ECMAScript|regex::optimize);
      smatch  sm;
      string  tmp = s;
      while (regex_search(tmp, sm, rgx)) {
        _version.push_back(VersionPart(sm.str()));
        tmp = sm.suffix();
      }
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionString & VersionString::operator = (const std::string & v)
    {
      _version.clear();
      static const regex  rgx("[^.]+", regex::ECMAScript|regex::optimize);
      smatch  sm;
      string  tmp = v;
      while (regex_search(tmp, sm, rgx)) {
        _version.push_back(VersionPart(sm.str()));
        tmp = sm.suffix();
      }
      return *this;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionString::operator < (const VersionString & vs) const
    {
      return (_version < vs._version);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionString::operator > (const VersionString & vs) const
    {
      return (_version > vs._version);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionString::operator == (const VersionString & vs) const
    {
      return (_version == vs._version);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool VersionString::operator != (const VersionString & vs) const
    {
      return (_version != vs._version);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    VersionString::operator std::string () const
    {
      string  rc;
      string  sep;
      for (const auto & p : _version) {
        rc += sep + (string)p;
        sep = ".";
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    ostream & operator << (ostream & os, const VersionString & vs)
    {
      string  sep;
      for (const auto & p : vs._version) {
        os << sep << p;
        sep = ".";
      }
      return os;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void VersionString::clear()
    {
      _version.clear();
    }
  

  }  // namespace Deb
  
}  // namespace Dwm
