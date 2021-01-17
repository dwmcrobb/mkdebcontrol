//===========================================================================
// @(#) $DwmPath$
// @(#) $Id$
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
//!  \file DwmDebPkgVersion.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::PkgVersion class declaration
//---------------------------------------------------------------------------

#ifndef _DWMDEBPKGVERSION_HH_
#define _DWMDEBPKGVERSION_HH_

#include <iostream>
#include <string>

#include "DwmDebVersionString.hh"

namespace Dwm {

  namespace Deb {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class PkgVersion
    {
    public:
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      PkgVersion()
          : _epoch(0), _version(), _release()
      {}

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      PkgVersion(const PkgVersion & dpv) = default;
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      PkgVersion(const std::string & version,
                 const std::string & release = "")
          : _epoch(0), _version(version), _release(release)
      {}

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      PkgVersion(int epoch, const std::string & version,
                 const std::string & release = "")
          : _epoch(epoch), _version(version), _release(release)
      {}

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool FromString(const std::string & s);
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      int Epoch() const                    { return _epoch; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      int Epoch(int epoch);
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const VersionString & Version() const  { return _version; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const VersionString & Version(const VersionString & version);
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Release() const  { return _release; }

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      const std::string & Release(const std::string & release);

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator < (const PkgVersion & dpv) const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator > (const PkgVersion & dpv) const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator == (const PkgVersion & dpv) const;

      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      bool operator != (const PkgVersion & dpv) const;
    
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      friend std::ostream & operator << (std::ostream & os,
                                         const PkgVersion & version);
    
    private:
      int            _epoch;
      VersionString  _version;
      std::string    _release;
    
    };

  }  // namespace Deb
  
}  // namespace Dwm

#endif  // _DWMDEBPKGVERSION_HH_
