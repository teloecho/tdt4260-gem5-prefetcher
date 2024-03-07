#include "mem/cache/prefetch/best_offset.hh"

//#include "mem/cache/replacement_policies/base.hh"
#include "params/BestOffsetPrefetcher.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{


BestOffsetPrefetcher::BestOffsetPrefetcher(const BestOffsetPrefetcherParams &params)
    : Queued(params)/*,
      someVariable(params.some_variable1,
                   params.other_variable2)*/
    {}


void
BestOffsetPrefetcher::calculatePrefetch(const PrefetchInfo &pfi,
                                        std::vector<AddrPriority> &addresses){
    Addr access_addr = pfi.getAddr();
    if (!pfi.hasPC()) {
        return;
    }

    uint64_t blockAddress = (access_addr >> std::log2(Base::blkSize));
    if (hasBeenPrefetched(access_addr, pfi.isSecure())) {
        // append blockAddress minus bestOffset (Y - D) to recentRequests
    }
    else if () {
        // append blockAddress (X) to recentRequests
    }


    if(M.prefetcherEnabled){
        addresses.push_back(AddrPriority(access_addr + Base::blkSize * M.bestOffset, 0));
    }

    M.subround++;
    if(M.subround == M.NUMBER_OF_OFFSETS){
        M.subround = 0;
        M.round++;
    }
}

}
}
