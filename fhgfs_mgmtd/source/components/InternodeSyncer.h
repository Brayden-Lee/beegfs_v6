#ifndef INTERNODESYNCER_H_
#define INTERNODESYNCER_H_

#include <common/app/log/LogContext.h>
#include <common/components/ComponentInitException.h>
#include <common/nodes/NodeStore.h>
#include <common/storage/StorageTargetInfo.h>
#include <common/threading/PThread.h>
#include <common/Common.h>
#include <components/componenthelpers/CapacityInfo.h>
#include <components/componenthelpers/DemotionFlags.h>
#include <nodes/NodeStoreServersEx.h>
#include <nodes/TargetCapacityReport.h>


class InternodeSyncer : public PThread
{
   public:
      InternodeSyncer() throw(ComponentInitException);
      ~InternodeSyncer();


   private:
      LogContext log;

      Mutex forcePoolsUpdateMutex;
      bool forcePoolsUpdate; // true to force update of capacity pools
      const unsigned targetOfflineTimeoutMS;

      RWLock targetCapacityReportMapLock;
      TargetCapacityReportMap targetCapacityReportMap;
      RWLock nodeCapacityReportMapLock;
      TargetCapacityReportMap nodeCapacityReportMap;

      virtual void run();
      void syncLoop();

      void updateMetaCapacityPools(bool updateForced);
      void updateStorageCapacityPools(bool updateForced);

      void updateNodeCapacityPools(NodeCapacityPools* pools, bool updateForced);
      void updateTargetCapacityPools(TargetMapper* targetMapper,
         TargetCapacityPools* pools, bool updateForced);
      void updateMetaBuddyCapacityPools();
      void updateTargetBuddyCapacityPools();

      bool assignNodesToPools(NodeCapacityPools* pools, const CapacityInfoList& capacityInfos);
      bool assignNodeToPool(NodeCapacityPools* pools, NumNodeID nodeID, CapacityPoolType pool,
         const std::string& reason);

      void getTargetMappingAsInfoList(TargetMapper* targetMapper,
         TargetCapacityInfoList& outTargetCapacityInfos);
      void getNodesAsInfoList(CapacityInfoList& outCapacityInfos);
      bool assignTargetsToPools(TargetCapacityPools* pools,
         const TargetCapacityInfoList& targetCapacityInfos);
      bool assignTargetToPool(TargetCapacityPools* pools, uint16_t targetID, NumNodeID nodeID,
         CapacityPoolType pool, const std::string& reason);

      template <typename P, typename L>
      void demoteIfNecessary(P* pools, L& capacityInfos, const DemotionFlags& demotionFlags);

      void logDemotionFlags(const DemotionFlags& demotionFlags, NodeType nodeType);

      void saveTargetMappings();

      void dropIdleConns();
      unsigned dropIdleConnsByStore(NodeStoreServersEx* nodes);

      void clearStaleCapacityReports(const NodeType nodeType);


   public:
      // getters & setters

      void setForcePoolsUpdate()
      {
         SafeMutexLock safeLock(&forcePoolsUpdateMutex);

         this->forcePoolsUpdate = true;

         safeLock.unlock();
      }

      // inliners

      bool getAndResetForcePoolsUpdate()
      {
         SafeMutexLock safeLock(&forcePoolsUpdateMutex);

         bool retVal = this->forcePoolsUpdate;

         this->forcePoolsUpdate = false;

         safeLock.unlock();

         return retVal;
      }

      /**
       * Take the elements of a StorageTargetInfoList and stores them in the local
       * target/nodeCapacityReportsMap as TargetCapacityReport objects.
       * @param nodeType the node type (storage / meta) for which the capacities are reported.
       */
      void storeCapacityReports(const StorageTargetInfoList& targetInfoList,
         const NodeType nodeType)
      {
         RWLock* reportsRWLock = (nodeType == NODETYPE_Meta)
            ? &nodeCapacityReportMapLock
            : &targetCapacityReportMapLock;

         TargetCapacityReportMap& capacityReportMap = (nodeType == NODETYPE_Meta)
            ? nodeCapacityReportMap
            : targetCapacityReportMap;

         Time timeNow; // Default-initialized to "now".

         SafeRWLock safeLock(reportsRWLock, SafeRWLock_WRITE); // L O C K

         for (StorageTargetInfoListCIter targetInfoIter = targetInfoList.begin();
              targetInfoIter != targetInfoList.end(); ++targetInfoIter)
         {
            const StorageTargetInfo& targetInfo = *targetInfoIter;

            TargetCapacityReport capacityReport = {
               targetInfo.getDiskSpaceTotal(),
               targetInfo.getDiskSpaceFree(),
               targetInfo.getInodesTotal(),
               targetInfo.getInodesFree()
            };

            const uint16_t targetID = targetInfo.getTargetID();

            capacityReportMap[targetID] = capacityReport;
         }

         safeLock.unlock(); // U N L O C K
      }
};

#endif /* INTERNODESYNCER_H_ */
