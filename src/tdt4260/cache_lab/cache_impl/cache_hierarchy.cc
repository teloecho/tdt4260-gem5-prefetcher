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
      cpuInterconnect(true, false, this, "L0_IC"),
      L1Interconnect(false, true, this, "L1_IC"),
      memory(params.blockSize, this)
{
    cpuInterconnect.setMemSideCache(&L1Cache);
    L1Interconnect.setCpuSideCache(&L1Cache);
    L1Interconnect.setMemory(&memory);

    L1Cache.setCpuSide(&cpuInterconnect);
    L1Cache.setMemSide(&L1Interconnect);
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
