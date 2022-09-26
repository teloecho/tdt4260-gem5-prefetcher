#ifndef __TDT4260_SIMPLE_MEM_HH__
#define __TDT4260_SIMPLE_MEM_HH__

#include "base/statistics.hh"
#include "interface.hh"

namespace gem5
{

class CacheHierarchy;
class CacheInterconnect;

class SimpleMem : public Interface
{
    public:
        SimpleMem(int blockSize, statistics::Group *parent);
    private:
        int blockSize = 0;

        Interface *cpuSideInterconnect = nullptr;

    public:
        virtual void recvReq(Addr req, int size);
        virtual void recvResp(Addr req);

        void setCpuSideInterconnect(Interface *cpuSide);

    protected:
        struct SimpleMemStats : public statistics::Group
        {
            SimpleMemStats(statistics::Group *parent);

            statistics::Scalar reqsReceived;
            statistics::Scalar respsIssued;
        } stats;
};

}

#endif
