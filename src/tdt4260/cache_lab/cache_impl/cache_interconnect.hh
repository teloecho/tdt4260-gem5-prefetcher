#ifndef __TDT4260_CACHE_INTERCONNECT_HH__
#define __TDT4260_CACHE_INTERCONNECT_HH__

#include "base/statistics.hh"
#include "interface.hh"

namespace gem5
{

class SimObject;

class CacheInterconnect : public Interface
{
    public:
        CacheInterconnect(bool isCpuSide, bool isMemSide,
            statistics::Group *parent, const char *name);

    private:
        bool isCpuSide;
        bool isMemSide;

        Interface *cpuSideCache = nullptr;
        Interface *memSideCache = nullptr;
        Interface *memory = nullptr;

    public:
        virtual void recvReq(Addr req, int size);
        virtual void recvResp(Addr resp);

        void setCpuSideCache(Interface *cpuCache);
        void setMemSideCache(Interface *memCache);
        void setMemory(Interface *mem);

    protected:
        struct CacheInterconnectStats : public statistics::Group
        {
            CacheInterconnectStats(statistics::Group *parent,
                                   const char *name);

            statistics::Scalar reqsReceived;
            statistics::Scalar respReceived;
        } stats;

};

}

#endif
