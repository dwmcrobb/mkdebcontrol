//===========================================================================
// @(#) $DwmPath: dwm/libDwm/trunk/apps/mkdebcontrol/mkdebcontrol.cc 11861 $
// @(#) $Id: mkdebcontrol.cc 11861 2021-01-17 06:25:29Z dwm $
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
//!  \file mkdebcontrol.cc
//!  \author Daniel W. McRobb
//!  \brief ingest a deb-control(5) file, update dependencies and
//!    requested fields, then emit the updated deb-control(5) on stdout
//---------------------------------------------------------------------------

extern "C" {
  #include <fts.h>
  #include <signal.h>
  #include <strings.h>
  #include <unistd.h>
}

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <regex>
#include <set>

#include "DwmDebArguments.hh"
#include "DwmDebControl.hh"

using namespace std;

namespace fs = std::filesystem;

typedef   Dwm::Deb::Arguments<Dwm::Deb::Argument<'a',string>,
                              Dwm::Deb::Argument<'d',string>,
                              Dwm::Deb::Argument<'m',string>,
                              Dwm::Deb::Argument<'n',string>,
                              Dwm::Deb::Argument<'r',string,true>,
                              Dwm::Deb::Argument<'s',string,true>,
                              Dwm::Deb::Argument<'v',string>,
                              Dwm::Deb::Argument<'w',string>>  MyArgType;

static MyArgType  g_args;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void InitArgs()
{
  g_args.SetValueName<'a'>("architecture");
  g_args.SetHelp<'a'>("Set the architecture");
  g_args.SetValueName<'d'>("description");
  g_args.SetHelp<'d'>("Set the description");
  g_args.SetValueName<'m'>("maintainer");
  g_args.SetHelp<'m'>("Set the maintainer");
  g_args.SetValueName<'n'>("name");
  g_args.SetHelp<'n'>("Set the package name");
  g_args.SetValueName<'r'>("debControlFile");
  g_args.SetHelp<'r'>("Read the given debControlFile and ingest its settings");
  g_args.SetValueName<'s'>("directory");
  g_args.SetHelp<'s'>("Staging directory where files to be packaged are"
                      " located.  Binaries and shared libraries are examined"
                      " in this directory (and its subdirectories,"
                      " recursively) to determine dependencies.");
  g_args.SetValueName<'v'>("version");
  g_args.SetHelp<'v'>("Set the package version");
  g_args.SetValueName<'w'>("URL");
  g_args.SetHelp<'w'>("Set the software's official web site");
  return;
}

extern "C" {
  //--------------------------------------------------------------------------
  //!  
  //--------------------------------------------------------------------------
  void SigPipeHandler(int signum)
  {
    cerr << "got SIGPIPE\n";
  }
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void HandleSigPipe()
{
  struct sigaction  action;
  memset(&action,0,sizeof(action));
  action.sa_handler = SigPipeHandler;
  action.sa_flags   = 0;
  if (sigaction(SIGPIPE, &action, nullptr) < 0) {
    cerr << "Failed to install SIGPIPE handler\n";
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void SigPipeDefault()
{
  struct sigaction  action;
  memset(&action,0,sizeof(action));
  action.sa_handler = SIG_DFL;
  if (sigaction(SIGPIPE, &action, nullptr) < 0) {
    cerr << "Failed to set default SIGPIPE handling\n";
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static string ToLower(string s)
{
  transform(s.begin(), s.end(), s.begin(),
            [] (unsigned char c) { return tolower(c); });
  return s;
}
              
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void GetSharedLibs(const string & filename, set<string> & libs)
{
  string  lddcmd("objdump -p " + filename);
  lddcmd += " 2>/dev/null";
  HandleSigPipe();
  FILE    *lddpipe = popen(lddcmd.c_str(), "r");
  if (lddpipe) {
    regex   rgx("^[ \\t]+NEEDED[ \\t]+([^ \\t\\n]+)",
                regex::ECMAScript|regex::optimize);
    smatch  sm;
    char    line[4096] = { '\0' };
    while (fgets(line, 4096, lddpipe) != NULL) {
      string  s(line);
      if (regex_search(s, sm, rgx)) {
        if (sm.size() == 2) {
          string  lib(sm[1].str());
          libs.insert(lib);
        }
      }
      memset(line, 0, sizeof(line));
    }
    pclose(lddpipe);
  }
  SigPipeDefault();
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static string GetPackage(const string & shlib)
{
  string  rc;
  string  cmd("dpkg -S ");
  cmd += shlib + " | tail -1";
  HandleSigPipe();
  FILE  *cmdpipe = popen(cmd.c_str(), "r");
  if (cmdpipe) {
    regex rgx("([^:]+)[:].+", regex::ECMAScript|regex::optimize);
    smatch  sm;
    char    line[4096] = { '\0' };
    while (fgets(line, 4096, cmdpipe) != NULL) {
      string  s(line);
      if (regex_search(s, sm, rgx)) {
        if (sm.size() == 2) {
          rc = sm[1].str();
        }
      }
      memset(line, 0, sizeof(line));
    }
    pclose(cmdpipe);
  }
  SigPipeDefault();
  return rc;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void GetPackages(const set<string> & shlibs, set<string> & packages)
{
  for (const auto & shlib : shlibs) {
    string  pkg = GetPackage(shlib);
    if (! pkg.empty()) {
      packages.insert(pkg);
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void GetNeededPackages(const string & prog, set<string> & packages)
{
  set<string>  shlibs;
  GetSharedLibs(prog, shlibs);
  GetPackages(shlibs, packages);
  return;
}

#ifndef __linux__
//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void GetExecutables(const std::string & dirpath,
                           set<string> & paths)
{
  try {
    for (auto & p : fs::recursive_directory_iterator(dirpath)) {
      if (fs::is_regular_file(p.path())) {
        if ((fs::status(p.path()).permissions() & fs::perms::owner_exec)
            != fs::perms::none) {
          string  fp = p.path().string();
          paths.insert(fp);
        }
      }
    }
  }
  catch (...) {
    cerr << "Exception reading " << dirpath << '\n';
  }
}

#else

//----------------------------------------------------------------------------
//!  Sigh, std::filesystem is still broken in GNU libstdc++fs on Raspbian 10
//!  (and probably on other Linux platforms).  On Raspbian 10 on the
//!  Raspberry Pi 4, it stomps on the stack and results in a useless core
//!  dump as a result.  valgrind isn't any help because it loses track.
//!  gcc/g++ on Raspbian don't have stack protection options.  So...
//!
//!  xxx TODO: see if I can reproduce this in stdc++fs on Ubuntu 20.04.
//!
//!  Resort to using fts_open(, fts_read() and fts_close().  They're C
//!  interfaces, but they're old and work correctly on Linux.
//----------------------------------------------------------------------------
static void GetExecutables(const string & dirName, set<string> & paths)
{
  string             filename;
  char  *dirs[2] = { strdup(dirName.c_str()), 0 };
  FTS  *fts = fts_open(&dirs[0], FTS_PHYSICAL|FTS_NOCHDIR, 0);
  if (fts) {
    FTSENT  *ftsent;
    while ((ftsent = fts_read(fts))) {
      switch (ftsent->fts_info) {
        case FTS_F:
          {
            filename = ftsent->fts_path;
            fs::path  p(filename);
            if ((fs::status(p).permissions() & fs::perms::owner_exec)
                != fs::perms::none) {
              paths.insert(filename);
            }
          }
          break;
        default:
          break;
      }
    }
    fts_close(fts);
  }
  free(dirs[0]);
  return;
}
#endif

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void UpdateDepends(Dwm::Deb::Control & debctrl)
{
  if (! debctrl.Depends().empty()) {
    for (auto dep : debctrl.Depends()) {
      auto  installedVers = Dwm::Deb::PkgDepend::InstalledVersion(dep.Package());
      if (installedVers > dep.Version()) {
        debctrl.RemoveDepend(dep);
        dep.Version(installedVers);
        dep.Operator(">=");
        debctrl.AddDepend(dep);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
void UpdatePreDepends(Dwm::Deb::Control & debctrl)
{
  if (! debctrl.PreDepends().empty()) {
    for (auto dep : debctrl.PreDepends()) {
      auto  installedVers = Dwm::Deb::PkgDepend::InstalledVersion(dep.Package());
      if (installedVers > dep.Version()) {
        debctrl.RemovePreDepend(dep);
        dep.Version(installedVers);
        dep.Operator(">=");
        debctrl.AddPreDepend(dep);
      }
    }
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void GetAllNeededPackages(int argc, char *argv[],
                                 set<string> & neededPackages)
{
  set<string>  executables;
  cerr << "scanning " << g_args.Get<'s'>() << '\n';
  GetExecutables(g_args.Get<'s'>(), executables);
  
  for (int i = 0; i < argc; ++i) {
    cerr << "scanning " << argv[i] << '\n';
    GetExecutables(argv[i], executables);
  }
  set<string>  sharedLibs;
  for (const auto & prog : executables) {
    GetSharedLibs(prog, sharedLibs);
  }
  GetPackages(sharedLibs, neededPackages);
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
static void ApplyCommandLineSettings(Dwm::Deb::Control & debctl)
{
  if (! g_args.Get<'a'>().empty()) {
    debctl.Add({"Architecture:", g_args.Get<'a'>()});
  }
  if (! g_args.Get<'d'>().empty()) {
    debctl.Add({"Description:", g_args.Get<'d'>()});
  }
  if (! g_args.Get<'m'>().empty()) {
    debctl.Add({"Maintainer:", g_args.Get<'m'>()});
  }
  if (! g_args.Get<'n'>().empty()) {
    debctl.Add({"Package:", g_args.Get<'n'>()});
  }
  if (! g_args.Get<'v'>().empty()) {
    debctl.Add({"Version:", g_args.Get<'v'>()});
  }
  if (! g_args.Get<'w'>().empty()) {
    debctl.Add({"Homepage:", g_args.Get<'w'>()});
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  using namespace Dwm;

  InitArgs();
  int arg = g_args.Parse(argc, argv);
  if (arg < 0) {
    cerr << g_args.Usage(argv[0], "[dependency_scan_path(s)...]");
    exit(1);
  }
  
  Deb::Control  debctrl;
  if (debctrl.Parse(g_args.Get<'r'>())) {
    ApplyCommandLineSettings(debctrl);
    if (! debctrl.HasRequiredEntries()) {
      cerr << g_args.Get<'r'>() << " is missing some required fields!\n";
      exit(1);
    }
    auto  pkgit = debctrl.Entries().find("Package:");
    set<string>  neededPackages;
    GetAllNeededPackages(argc - arg, &(argv[arg]), neededPackages);
    for (const auto & np : neededPackages) {
      //  Don't include our own package
      if (ToLower(np) != ToLower(pkgit->second)) {
        debctrl.AddPreDepend(np);
        debctrl.AddDepend(np);
      }
    }
    UpdatePreDepends(debctrl);
    UpdateDepends(debctrl);

    //  Add previous versions of our package as a conflict
    auto    versit = debctrl.Entries().find("Version:");
    string  conflict = ToLower(pkgit->second)
      + " (<< " + versit->second + ")";
    debctrl.Add({"Conflicts:", conflict});
    cout << debctrl;
  }
  else {
    cerr << "Failed to parse '" << g_args.Get<'r'>() << "'\n";
  }
  
  
}
