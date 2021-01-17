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
//!  \file DwmDebVersionString.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::VersionString and Dwm::Deb::VersionPart class
//!    declarations
//---------------------------------------------------------------------------

#ifndef _DWMDEBVERSIONSTRING_HH_
#define _DWMDEBVERSIONSTRING_HH_

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace Dwm {

  namespace Deb {

    //------------------------------------------------------------------------
    //!  Encapsulates a single part of a version string.  We use a std::vector
    //!  of std::variant to hold the value.  If the value is all digits, the
    //!  vector will have one entry, holding an unsigned long.  This is the
    //!  most sane versioning scheme.  If the value is all non-digits, the
    //!  vector will have one entry holding a string.  In all other cases,
    //!  we'll store a vector that's composed by walking the input string
    //!  and creating a vector entry for each consecutive chain of digits and
    //!  each consecutive chain of non-digits.
    //------------------------------------------------------------------------
    class VersionPart
    {
    public:
      VersionPart(const std::string & part);
      VersionPart & operator = (const std::string & vp);
      bool operator < (const VersionPart & vpp) const;
      bool operator > (const VersionPart & vpp) const;
      bool operator == (const VersionPart & vpp) const;
      bool operator != (const VersionPart & vpp) const;
      operator std::string () const;
      friend std::ostream & operator << (std::ostream & os,
                                         const VersionPart & vpp);
      void clear();
    
    private:
      std::vector<std::variant<unsigned long,std::string>>  _data;
    };

    //------------------------------------------------------------------------
    //!  Encapsulates a software version string, such as '13.4.2'.  This is
    //!  an attempt to deal with various version numbering schemes and allow
    //!  comparisons between them.  It won't work on every scheme; there is
    //!  no universally used versioning scheme and some directly conflict with
    //!  others in terms of machine parsing and comparisons.
    //!
    //!  The accepted schemes have one or more strings separated by dots.
    //!  This class stores each of the strings as a VersionPart in a 
    //!  std::vector.  Most of the real work occurs in VersionPart.
    //------------------------------------------------------------------------
    class VersionString
    {
    public:
      VersionString();
      VersionString(const std::string & s);
      VersionString & operator = (const std::string & v);
      bool operator < (const VersionString & vs) const;
      bool operator > (const VersionString & vs) const;
      bool operator == (const VersionString & vs) const;
      bool operator != (const VersionString & vs) const;
      operator std::string () const;
      friend std::ostream & operator << (std::ostream & os,
                                         const VersionString & vs);
      void clear();
    
    private:
      std::vector<VersionPart>  _version;
    };

    
  }  // namespace Deb

}  // namespace Dwm

#endif  // _DWMDEBVERSIONSTRING_HH_
