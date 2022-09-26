#ifndef __TDT4260_SIMPLE_CACHE_HH__
#define __TDT4260_SIMPLE_CACHE_HH__

#include <vector>

#include "base/statistics.hh"
#include "interface.hh"

namespace gem5
{

class SimpleCache : public Interface
{

    public:
        SimpleCache(int size, int blockSize, int associativity,
            statistics::Group *parent, const char *name);
    private:

        struct Entry
        {
            Entry() {
                tag = MaxAddr;
                lastUsed = 0;
            };

            Addr tag;
            int lastUsed;
        };

        int size = 0;
        int blockSize = 0;

        int numEntries = 0;
        int associativity = 0;
        int numSets = 0;

        bool isL1;
        bool isL2;
        bool isL3;

        int ctr = 0;

        const char *cacheName;

        std::vector<std::vector<Entry *>> entries;

        Interface *cpuSide = nullptr;
        Interface *memSide = nullptr;

    public:
        virtual void recvReq(Addr req, int size);
        virtual void recvResp(Addr resp);

        void setLevel(int level) {
            if (level == 1) isL1 = true;
            if (level == 2) isL2 = true;
            if (level == 3) isL3 = true;
        }

        void setCpuSide(Interface *cpu) {
            cpuSide = cpu;
        }

        void setMemSide(Interface *mem) {
            memSide = mem;
        }


    private:
        int calculateTag(Addr req);
        int calculateIndex(Addr req);
        bool hasLine(int index, int tag);
        int lineWay(int index, int tag);
        int oldestWay(int index);

        void sendReq(Addr req, int size);
        void sendResp(Addr resp);
    
    protected:
        struct SimpleCacheStats : public statistics::Group
        {
            SimpleCacheStats(statistics::Group *parent,
                             const char *name);

            statistics::Scalar reqsReceived;
            statistics::Scalar reqsServiced;
            statistics::Scalar respsReceived;
        } stats;
};

}

#endif
