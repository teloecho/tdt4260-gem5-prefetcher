#include "tdt4260/cache_lab/cache_impl/simple_mem.hh"

namespace gem5
{

SimpleMem::SimpleMem(int blockSize, statistics::Group *parent)
    : blockSize(blockSize), stats(parent) {}

SimpleMem::SimpleMemStats::SimpleMemStats(statistics::Group *parent)
    : statistics::Group(parent, "simple_memory"),
    ADD_STAT(reqsReceived, statistics::units::Count::get(),
            "Total number of reqs received from memSide"),
    ADD_STAT(respsIssued, statistics::units::Count::get(),
            "Total number of resps returned to cpuSide") {}

void
SimpleMem::recvReq(Addr req, int size)
{
    ++stats.reqsReceived;
    int numLines = ceil(((double) size) / ((double) blockSize));
    for (int i = 0; i < numLines; i++) {
        ++stats.respsIssued;
        cpuSideInterconnect->recvResp(req + (i*blockSize));
    }
}

void
SimpleMem::recvResp(Addr resp)
{
    panic("Should never receive a response");
}

void
SimpleMem::setCpuSideInterconnect(Interface *cpuSide)
{
    cpuSideInterconnect = cpuSide;
}

}