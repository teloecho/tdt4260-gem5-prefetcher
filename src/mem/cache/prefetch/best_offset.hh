#ifndef __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
#define __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__

#include "base/types.hh"
#include "mem/cache/prefetch/queued.hh"

namespace gem5
{

// maybe not required:
GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{
    class Base;
}
// end: maybe not required

struct BestOffsetPrefetcherParams;

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

class BestOffsetPrefetcher : public Queued
{

  protected:

  public:
    BestOffsetPrefetcher(const BestOffsetPrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses) override;

};

} //namespace prefetch
} //namespace gem5

#endif //__MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
