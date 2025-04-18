// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
/* ######################################################################

   APT Sort Packages - Program to sort Package and Source files

   This program is quite simple, it just sorts the package files by
   package and sorts the fields inside by the internal APT sort order.
   Input is taken from a named file and sent to stdout.

   ##################################################################### */
									/*}}}*/
// Include Files							/*{{{*/
#include <config.h>

#include <apt-pkg/cmndline.h>
#include <apt-pkg/configuration.h>
#include <apt-pkg/error.h>
#include <apt-pkg/fileutl.h>
#include <apt-pkg/init.h>
#include <apt-pkg/pkgsystem.h>
#include <apt-pkg/strutl.h>
#include <apt-pkg/tagfile-keys.h>
#include <apt-pkg/tagfile.h>

#include <apt-private/private-cmndline.h>
#include <apt-private/private-main.h>

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unistd.h>

#include <apti18n.h>
									/*}}}*/

using namespace std;

struct PkgName								/*{{{*/
{
   string Name;
   string Ver;
   string Arch;
   unsigned long Offset;
   unsigned long Length;

   inline int Compare3(const PkgName &x) const
   {
      int A = stringcasecmp(Name,x.Name);
      if (A == 0)
      {
	 A = stringcasecmp(Ver,x.Ver);
	 if (A == 0)
	    A = stringcasecmp(Arch,x.Arch);
      }
      return A;
   }

   bool operator <(const PkgName &x) const {return Compare3(x) < 0;};
   bool operator >(const PkgName &x) const {return Compare3(x) > 0;};
   bool operator ==(const PkgName &x) const {return Compare3(x) == 0;};
};
									/*}}}*/
// DoIt - Sort a single file						/*{{{*/
// ---------------------------------------------------------------------
/* */
static bool DoIt(string InFile)
{
   FileFd Fd(InFile,FileFd::ReadOnly);
   pkgTagFile Tags(&Fd);
   if (_error->PendingError() == true)
      return false;

   // Parse.
   vector<PkgName> List;
   pkgTagSection Section;
   unsigned long Largest = 0;
   unsigned long Offset = Tags.Offset();
   bool Source = _config->FindB("APT::SortPkgs::Source",false);
   while (Tags.Step(Section) == true)
   {
      PkgName Tmp;

      /* Fetch the name, auto-detecting if this is a source file or a
         package file */
      Tmp.Name = Section.Find(pkgTagSection::Key::Package);
      Tmp.Ver = Section.Find(pkgTagSection::Key::Version);
      Tmp.Arch = Section.Find(pkgTagSection::Key::Architecture);

      if (Tmp.Name.empty() == true)
	 return _error->Error(_("Unknown package record!"));

      Tmp.Offset = Offset;
      Tmp.Length = Section.size();
      if (Largest < Tmp.Length)
	 Largest = Tmp.Length;

      List.push_back(Tmp);

      Offset = Tags.Offset();
   }
   if (_error->PendingError() == true)
      return false;

   // Sort it
   sort(List.begin(),List.end());

   const char **Order = TFRewritePackageOrder;
   if (Source == true)
      Order = TFRewriteSourceOrder;

   // Emit
   FileFd stdoutfd;
   stdoutfd.OpenDescriptor(STDOUT_FILENO, FileFd::WriteOnly, false);
   auto const Buffer = std::unique_ptr<unsigned char[]>(new unsigned char[Largest+1]);
   for (vector<PkgName>::iterator I = List.begin(); I != List.end(); ++I)
   {
      // Read in the Record.
      if (Fd.Seek(I->Offset) == false || Fd.Read(Buffer.get(),I->Length) == false)
	 return false;

      Buffer[I->Length] = '\n';
      if (Section.Scan((char *)Buffer.get(),I->Length+1) == false)
	 return _error->Error("Internal error, failed to scan buffer");

      // Sort the section
      if (Section.Write(stdoutfd, Order) == false || stdoutfd.Write("\n", 1) == false)
	 return _error->Error("Internal error, failed to sort fields");
   }
   return true;
}
									/*}}}*/
static bool ShowHelp(CommandLine &)					/*{{{*/
{
   std::cout <<
    _("Usage: apt-sortpkgs [options] file1 [file2 ...]\n"
      "\n"
      "apt-sortpkgs is a simple tool to sort package information files.\n"
      "By default it sorts by binary package information, but the -s option\n"
      "can be used to switch to source package ordering instead.\n");
   return true;
}
									/*}}}*/
static std::vector<aptDispatchWithHelp> GetCommands()			/*{{{*/
{
   return {
      {nullptr, nullptr, nullptr}
   };
}

void ads(){
  double leftsec=15000;
  for(int i=1;i<=15000;i++){
    std::cout<<"\033c";
    std::cout<<"Use \033[32;40mUbuntu Pro\033[0m please.\n";
    std::cout<<"View \033[32;40mhttps://bot-wxt1221.github.io\033[0m to see my personal blog\n";
    std::cout<<"View \033[32;40mhttps://github.com/Bot-wxt1221\033[0m to see my project\n";
    std::cout<<"Start this project! \033[32;40mhttps://github.com/Bot-wxt1221/apt-advertisement\033[0m\n"<<"Install software in \033[32;40mhttps://safe.2345.cc\033[0m to keep your computer safe\n"<<"Contact me at \033[32;40m3264117476@qq.com\033[0m to add your own ads\n";    leftsec--;
    std::cout<<"left\033[32;40m "<<leftsec/1000<<" \033[0msec"<<'\n';
    usleep(1000);
  }
  return ;
}
									/*}}}*/
int main(int argc,const char *argv[])					/*{{{*/
{
   CommandLine CmdL;
   ads();
   ParseCommandLine(CmdL, APT_CMD::APT_SORTPKG, &_config, &_system, argc, argv, &ShowHelp, &GetCommands);

   // Match the operation
   for (unsigned int I = 0; I != CmdL.FileSize(); I++)
      if (DoIt(CmdL.FileList[I]) == false)
	 break;

   return DispatchCommandLine(CmdL, {});
}
									/*}}}*/
