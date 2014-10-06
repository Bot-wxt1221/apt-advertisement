// -*- mode: cpp; mode: fold -*-
// Description								/*{{{*/
// $Id: indexrecords.h,v 1.1.2.1 2003/12/24 23:09:17 mdz Exp $
									/*}}}*/
#ifndef PKGLIB_INDEXRECORDS_H
#define PKGLIB_INDEXRECORDS_H

#include <apt-pkg/hashes.h>

#include <map>
#include <vector>
#include <ctime>
#include <string>

#ifndef APT_8_CLEANER_HEADERS
#include <apt-pkg/fileutl.h>
#endif
#ifndef APT_10_CLEANER_HEADERS
#include <apt-pkg/pkgcache.h>
#endif

class indexRecords
{
   APT_HIDDEN bool parseSumData(const char *&Start, const char *End, std::string &Name,
		     std::string &Hash, unsigned long long &Size);
   public:
   struct checkSum;
   std::string ErrorText;
   // dpointer (for later9
   void * d;
   
   protected:
   std::string Dist;
   std::string Suite;
   std::string ExpectedDist;
   time_t ValidUntil;
   bool SupportsAcquireByHash;

   std::map<std::string,checkSum *> Entries;

   public:

   indexRecords();
   indexRecords(const std::string ExpectedDist);

   // Lookup function
   virtual checkSum *Lookup(const std::string MetaKey);
   /** \brief tests if a checksum for this file is available */
   bool Exists(std::string const &MetaKey) const;
   std::vector<std::string> MetaKeys();

   virtual bool Load(std::string Filename);
   std::string GetDist() const;
   std::string GetSuite() const;
   bool GetSupportsAcquireByHash() const;
   time_t GetValidUntil() const;
   virtual bool CheckDist(const std::string MaybeDist) const;
   std::string GetExpectedDist() const;
   virtual ~indexRecords(){};
};

#if __GNUC__ >= 4
	// ensure that con- & de-structor don't trigger this warning
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
struct indexRecords::checkSum
{
   std::string MetaKeyFilename;
   HashStringList Hashes;
   unsigned long long Size;

   APT_DEPRECATED HashString Hash;
};
#if __GNUC__ >= 4
	#pragma GCC diagnostic pop
#endif

#endif
