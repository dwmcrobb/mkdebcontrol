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
//!  \file DwmDebPkgDepend.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::PkgDepend class declaration
//---------------------------------------------------------------------------

#ifndef _DWMDEBPKGDEPEND_HH_
#define _DWMDEBPKGDEPEND_HH_

#include "DwmDebPkgVersion.hh"

namespace Dwm {

  namespace Deb {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class PkgDepend
    {
    public:
      PkgDepend() = default;

      PkgDepend(const std::string & pkg)
          : _pkg(pkg), _operator(), _version()
      {}
    
      PkgDepend(const std::string & pkg, const std::string & op,
                   const PkgVersion & version)
          : _pkg(pkg), _operator(op), _version(version)
      {}

      const std::string & Package() const
      { return _pkg; }

      const std::string & Package(const std::string & package);
    
      const std::string & Operator() const
      { return _operator; }

      const std::string & Operator(const std::string & op);
    
      const PkgVersion & Version() const
      { return _version; }

      const PkgVersion & Version(const PkgVersion & dpv);
    
      static PkgVersion InstalledVersion(const std::string & pkg);

      bool operator < (const PkgDepend & dpd) const;

      bool operator == (const PkgDepend & dpd) const;
    
      friend std::ostream & operator << (std::ostream & os,
                                         const PkgDepend & dep);
    
    private:
      std::string  _pkg;
      std::string  _operator;
      PkgVersion   _version;
    };

  }  // namespace Deb
  
}  // namespace Dwm

#endif  // _DWMDEBPKGDEPEND_HH_
