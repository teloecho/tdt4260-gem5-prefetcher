#ifndef __TDT4260_CACHE_HIERARCHY_HH__
#define __TDT4260_CACHE_HIERARCHY_HH__

#include "params/BaseCacheHierarchy.hh"
#include "sim/sim_object.hh"
#include "tdt4260/cache_lab/cache_impl/cache_interconnect.hh"
#include "tdt4260/cache_lab/cache_impl/simple_cache.hh"
#include "tdt4260/cache_lab/cache_impl/simple_mem.hh"

namespace gem5

{

struct BaseCacheHierarchyParams;

class CacheInterconnect;

class CacheHierarchy : public SimObject
{

    public:
        CacheHierarchy(const BaseCacheHierarchyParams &params);

        void recvReq(Addr req, int size);

    private:
        SimpleCache L1Cache;

        CacheInterconnect cpuInterconnect;
        CacheInterconnect L1Interconnect;

        SimpleMem memory;

    public:
        struct CacheHierarchyStats : public statistics::Group
        {
            CacheHierarchyStats(CacheHierarchy *cacheHierarchy);

            statistics::Scalar reqsReceived;
        } stats;

};

}

#endif
