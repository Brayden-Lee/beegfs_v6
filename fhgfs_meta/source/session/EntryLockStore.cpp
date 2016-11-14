#include <common/threading/SafeMutexLock.h>

#include "EntryLockStore.h"

ParentNameLockData* EntryLockStore::lock(const std::string& parentID, const std::string& name)
{
   ParentNameLockData& lock = parentNameLocks.getLockFor(
      std::pair<const std::string&, const std::string&>(parentID, name) );
   lock.getLock().lock();
   return &lock;
}

FileIDLockData* EntryLockStore::lock(const std::string& fileID)
{
   FileIDLockData& lock = fileLocks.getLockFor(fileID);
   lock.getLock().lock();
   return &lock;
}

DirIDLockData* EntryLockStore::lock(const std::string& dirID, const bool writeLock)
{
   DirIDLockData& lock = dirLocks.getLockFor(dirID);
   if(writeLock)
      lock.getLock().writeLock();
   else
      lock.getLock().readLock();

   return &lock;
}

void EntryLockStore::unlock(ParentNameLockData* parentNameLockData)
{
   parentNameLockData->getLock().unlock();
   parentNameLocks.putLock(*parentNameLockData);
}

void EntryLockStore::unlock(FileIDLockData* fileIDLockData)
{
   fileIDLockData->getLock().unlock();
   fileLocks.putLock(*fileIDLockData);
}

void EntryLockStore::unlock(DirIDLockData* dirIDLockData)
{
   dirIDLockData->getLock().unlock();
   dirLocks.putLock(*dirIDLockData);
}
