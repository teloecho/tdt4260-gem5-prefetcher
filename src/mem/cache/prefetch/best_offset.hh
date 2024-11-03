/*
 * 2024 oystebw, teloecho
 */

/**
 * @file
 * Header file for our implementation of a Best Offset Prefetcher following
 * the paper of Pierre Michaud https://inria.hal.science/hal-01254863v1.
 * However the recent requests table (recentRequests) is simplified as deqeue,
 * allowing duplicated entries, which diverges from the papers implementation
 * as cache-like structure, "direct mapped, accessed through a hash function".
 */

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
  private:
    /**
     * When an offset reaches this score at the end of a round, it will be the
     * next bestOffset and the learning phase starts over again.
     */
    const int SCOREMAX;

    /**
     * After this number of rounds and if no score reached SCOREMAX, the
     * learning phase finishes and the highest scoring offset is the next best
     * offset, if it is also higher than BADSCORE.
     */
    const int ROUNDMAX;

    /**
     * If a learning phase finishes and no score is above the BADSCORE
     * threshold, prefetching is switched off.
     */
    const int BADSCORE;

    /**
     * The maximum size of the RR (recent requests) table. If recentRequests
     * reached this size, for each new element at the front one old element at
     * the end is removed.
     */
    const int RR_SIZE;

    /** Right shift an address by that value to get the block address (tag). */
    const int log2blockSize = int(std::log2(Base::blkSize));

    /**
     * Limit the offsets to the first NUMBER_OF_OFFSETS offsets in
     * offsetScorePair.
     */
    const int NUMBER_OF_OFFSETS = 25;

    /**
     * If this flag is false, no prefetch requests are issued, the learning
     * phases however continue. If an offset scorse higher than BADSCORE at
     * the end of a learning phase, a new bestOffset is found and prefetching
     * is enabled again.
     */
    bool prefetcherEnabled = false;

    /**
     * The current best offset which is used to issue prefetch requests.
     */
    int bestOffset = 1;

    /**
     * Round counter. A learning phase consists of several rounds. It is
     * limited by ROUNDMAX.
     */
    int round = 0;

    /**
     * During a round, each offset shall be tested once. This counter is
     * limited by NUMBER_OF_OFFSETS and serves as an index into
     * offsetScorePair vector.
     */
    int subround = 0;

    /**
     * The
     */
    std::vector<std::pair<int, int>> offsetScorePair {
        {1, 0},  {2, 0},  {3, 0},  {4, 0},  {5, 0},  {6, 0},  {8, 0},  {9, 0},
        {10, 0}, {12, 0}, {15, 0}, {16, 0}, {18, 0}, {20, 0}, {24, 0}, {25, 0},
        {27, 0}, {30, 0}, {32, 0}, {36, 0}, {40, 0}, {45, 0}, {48, 0}, {50, 0},
        {54, 0}, {60, 0}
    };

    /**
     * The RR (recent requests) table. It is filled with the block addresses
     * (tags) of prefetched blocks as soon as they arrive from L3.
     */
    std::deque<int> recentRequests;

  public:
    BestOffsetPrefetcher(const BestOffsetPrefetcherParams &p);

    /**
     * Calculate which addresses to prefetch according to the best offset
     * prefetch algorithm.
     */
    void calculatePrefetch(const PrefetchInfo &pfi,
                           std::vector<AddrPriority> &addresses);
    
    /**
     * Is invoked on any L2 cache fill. Used to learn offsets while the
     * prefetcher is disabled.
     */
    void notifyFill(const PacketPtr &ptr) override;

    /**
     * Is invoked only on L2 fills if the block is a prefetched block. Used to
     * learn offsets while the prefetcher is enabled.
     */
    void notifyPrefetchFill(const PacketPtr &ptr) override;
};

} //namespace prefetch
} //namespace gem5

#endif //__MEM_CACHE_PREFETCH_BEST_OFFSET_HH__
