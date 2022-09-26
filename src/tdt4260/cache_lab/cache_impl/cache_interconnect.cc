
#include "tdt4260/cache_lab/cache_impl/cache_interconnect.hh"

namespace gem5
{

CacheInterconnect::CacheInterconnect(bool cpuSide, bool memSide,
    statistics::Group *parent, const char *name)
    : isCpuSide(cpuSide), isMemSide(memSide),
      stats(parent, name)
{
    assert(!(memSide && cpuSide));
}

CacheInterconnect::CacheInterconnectStats::CacheInterconnectStats(
    statistics::Group *parent, const char *name)
    : statistics::Group(parent, name),
    ADD_STAT(reqsReceived, statistics::units::Count::get(),
            "Total numbers of reqs received from cpu side"),
    ADD_STAT(respReceived, statistics::units::Count::get(),
            "Total number of resps received from mem side") {}

void
CacheInterconnect::recvReq(Addr req, int size)
{
    ++stats.reqsReceived;
    if (isMemSide) {
        memory->recvReq(req, size);
    } else {
        memSideCache->recvReq(req, size);
    }
}

void
CacheInterconnect::recvResp(Addr resp)
{
    ++stats.respReceived;
    if (!isCpuSide)
        cpuSideCache->recvResp(resp);
}

void
CacheInterconnect::setCpuSideCache(Interface *cpuCache)
{
    assert(!isCpuSide);
    cpuSideCache = cpuCache;
}

void
CacheInterconnect::setMemSideCache(Interface *memCache)
{
    assert(!isMemSide);
    memSideCache = memCache;
}

void
CacheInterconnect::setMemory(Interface *mem)
{
    assert(isMemSide);
    memory = mem;
}

}