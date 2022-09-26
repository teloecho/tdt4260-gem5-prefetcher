#include "tdt4260/cache_lab/cache_impl/cache_hierarchy.hh"

#include "base/trace.hh"
#include "debug/TDTSimpleCache.hh"

namespace gem5
{

CacheHierarchy::CacheHierarchy(const BaseCacheHierarchyParams &params)
    : SimObject(params), stats(this),
      L1Cache(params.l1CacheSize,
              params.blockSize,
              params.l1CacheAssociativity,
              this, "L1_Cache"),
//      L2Cache(params.l2CacheSize,
//              params.blockSize,
//              params.l2CacheAssociativity,
//              this, "L2_Cache"),
//      L3Cache(params.l3CacheSize,
//              params.blockSize,
//              params.l3CacheAssociativity,
//              this, "L3_Cache"),
      cpuInterconnect(true, false, this, "L0_IC"),
      L1Interconnect(false, true, this, "L1_IC"),
//      L2Interconnect(false, false, this, "L2_IC"),
//      L3Interconnect(false, true, this, "L3_IC"),
      memory(params.blockSize, this)
{
    cpuInterconnect.setMemSideCache(&L1Cache);
    L1Interconnect.setCpuSideCache(&L1Cache);
//    L1Interconnect.setMemSideCache(&L2Cache);
//    L2Interconnect.setCpuSideCache(&L2Cache);
//    L2Interconnect.setMemSideCache(&L3Cache);
//    L3Interconnect.setCpuSideCache(&L3Cache);
    L1Interconnect.setMemory(&memory);

    L1Cache.setCpuSide(&cpuInterconnect);
    L1Cache.setMemSide(&L1Interconnect);
//    L2Cache.setCpuSide(&L1Interconnect);
//    L2Cache.setMemSide(&L2Interconnect);
//    L3Cache.setCpuSide(&L2Interconnect);
//    L3Cache.setMemSide(&L3Interconnect);
    memory.setCpuSideInterconnect(&L1Interconnect);
}

CacheHierarchy::
CacheHierarchyStats::CacheHierarchyStats(CacheHierarchy *cacheHierarchy)
    : statistics::Group(cacheHierarchy),
    ADD_STAT(reqsReceived, statistics::units::Count::get(),
            "Total number of reqs received from CPU") {}

void
CacheHierarchy::recvReq(Addr req, int size)
{
    DPRINTF(TDTSimpleCache, "Received req %#x with size %d\n",
            req, size);
    cpuInterconnect.recvReq(req, size);
}

}
