#ifndef __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
#define __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__

#include "base/types.hh"
#include "mem/cache/prefetch/queued.hh"
#include <deque>
#include <algorithm>

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
		struct Members {
			const int SCORE_MAX = 31;
			const int ROUND_MAX = 100;
			const int BAD_SCORE = 10;
			const int RR_SIZE = 256;
			const int NUMBER_OF_OFFSETS = 3;

			bool prefetcherEnabled = false;
			int bestOffset = 0;
			int subround = 0;
			int round = 0;

			std::vector<std::pair<int, int>> offsetScorePair {{1, 0}, {2, 0}, {3, 0}};
			std::deque<int> recentRequests;
		} M;

  public:
    BestOffsetPrefetcher(const BestOffsetPrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses);

};

} //namespace prefetch
} //namespace gem5

#endif //__MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
