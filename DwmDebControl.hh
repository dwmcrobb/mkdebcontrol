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
//!  \file DwmDebControl.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::Deb::Control class declaration
//---------------------------------------------------------------------------

#ifndef _DWMDEBCONTROL_HH_
#define _DWMDEBCONTROL_HH_

#include <map>
#include <string>
#include <set>

#include "DwmDebPkgDepend.hh"

namespace Dwm {

  namespace Deb {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    class Control
    {
    public:
      Control();
    
      bool Parse(const std::string & path);

      bool HasRequiredEntries() const;
    
      const std::map<std::string,std::string> & Entries() const
      { return _entries; }

      const std::set<PkgDepend> & Depends() const
      { return _depends; }
      
      const std::set<PkgDepend> & PreDepends() const
      { return _predepends;
      }
      
      void Add(const std::pair<std::string,std::string> & entry);

      void RemoveDepend(const PkgDepend & dep);

      void RemovePreDepend(const PkgDepend & dep);

      void AddDepend(const PkgDepend & dep);

      void AddPreDepend(const PkgDepend & dep);
      
      void AddDepends(const std::set<PkgDepend> & depends);

      void AddPreDepends(const std::set<PkgDepend> & depends);
    
      friend std::ostream & operator << (std::ostream & os,
                                         const Control & debctrl);

    private:
      std::map<std::string,std::string>  _entries;
      std::set<PkgDepend>                _predepends;
      std::set<PkgDepend>                _depends;
    };

  }  // namespace Deb
  
}  // namespace Dwm

#endif  // _DWMDEBCONTROL_HH_
