#ifndef __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
#define __MEM_CACHE_PREFETCH_BEST_OFFSET_HH__

#include "base/types.hh"
#include "mem/cache/prefetch/queued.hh"
#include "debug/HWPrefetch.hh"
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
			const int NUMBER_OF_OFFSETS = 27;

			bool prefetcherEnabled = true;
			int bestOffset = 1;
			int subround = 0;
			int round = 0;

			// from the paper
			std::vector<std::pair<int, int>> offsetScorePair {
				{1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {8, 0},  {9, 0},
				{10, 0}, {12, 0}, {15, 0}, {16, 0}, {18, 0}, {20, 0}, {24, 0}, {25, 0},
				{27, 0}, {30, 0}, {32, 0}, {36, 0}, {40, 0}, {45, 0}, {48, 0}, {50, 0},
				{54, 0}, {60, 0}, {64, 0}};
			std::deque<int> recentRequests;
		} M;

  public:
    BestOffsetPrefetcher(const BestOffsetPrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses);
	
	void notifyFill(const PacketPtr &pkt);

};

} //namespace prefetch
} //namespace gem5

#endif //__MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
