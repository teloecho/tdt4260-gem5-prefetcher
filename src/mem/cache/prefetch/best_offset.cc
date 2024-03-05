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
                                        std::vector<AddrPriority> &addresses)
{
    Addr access_addr = pfi.getAddr();
    addresses.push_back(AddrPriority(access_addr + Base::blkSize, 0));
}

}
}
