#ifndef __MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__
#define __MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__

#include "base/sat_counter.hh"
#include "base/types.hh"
#include "mem/cache/prefetch/associative_set.hh"
#include "mem/cache/prefetch/queued.hh"
#include "mem/cache/tags/indexing_policies/set_associative.hh"
#include "mem/packet.hh"
#include "params/TDTPrefetcherHashedSetAssociative.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(ReplacementPolicy, replacement_policy);
namespace replacement_policy
{
    class Base;
}

struct TDTPrefetcherParams;

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

class TDTPrefetcherHashedSetAssociative : public SetAssociative
{
    protected:
        uint32_t extractSet(const Addr addr) const override;
        Addr extractTag(const Addr addr) const override;

    public:
        TDTPrefetcherHashedSetAssociative(
            const TDTPrefetcherHashedSetAssociativeParams &p)
        : SetAssociative(p)
        {}

        ~TDTPrefetcherHashedSetAssociative() = default;
};

class TDTPrefetcher : public Queued
{

  protected:

    const struct PCTableInfo
    {
        const int assoc;
        const int numEntries;

        BaseIndexingPolicy* const indexingPolicy;
        replacement_policy::Base* const replacementPolicy;

        PCTableInfo(int assoc, int num_entries,
            BaseIndexingPolicy* indexing_policy,
            replacement_policy::Base* repl_policy)
          : assoc(assoc), numEntries(num_entries),
            indexingPolicy(indexing_policy), replacementPolicy(repl_policy)
        {

        }
    } pcTableInfo;

    struct TDTEntry : public TaggedEntry
    {
        TDTEntry();

        Addr lastAddr = 0;

        void invalidate() override;

    };

    typedef AssociativeSet<TDTEntry> PCTable;
    std::unordered_map<int, PCTable> pcTables;

    PCTable* findTable(int context);

    PCTable* allocateNewContext(int context);

  public:
    TDTPrefetcher(const TDTPrefetcherParams &p);

    void calculatePrefetch(const PrefetchInfo &pf1,
                           std::vector<AddrPriority> &addresses) override;

};

} //namespace prefetch
} //namespace gem5

#endif //_MEM_CACHE_PREFETCH_TDT_PREFETCHER_HH__
