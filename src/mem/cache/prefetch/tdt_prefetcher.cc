#include "mem/cache/prefetch/tdt_prefetcher.hh"

#include "debug/HWPrefetch.hh"
#include "mem/cache/prefetch/associative_set_impl.hh"
#include "mem/cache/replacement_policies/base.hh"
#include "params/TDTPrefetcher.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{

TDTPrefetcher::TDTEntry::TDTEntry()
    : TaggedEntry()
{
    invalidate();
}

void
TDTPrefetcher::TDTEntry::invalidate()
{
    TaggedEntry::invalidate();
}

TDTPrefetcher::TDTPrefetcher(const TDTPrefetcherParams &params)
    : Queued(params),
      pcTableInfo(params.table_assoc, params.table_entries,
                  params.table_indexing_policy,
                  params.table_replacement_policy)
    {}

TDTPrefetcher::PCTable*
TDTPrefetcher::findTable(int context)
{
    auto it = pcTables.find(context);
    if (it != pcTables.end())
        return &it->second;

    return allocateNewContext(context);
}

TDTPrefetcher::PCTable*
TDTPrefetcher::allocateNewContext(int context)
{
    auto insertion_result = pcTables.insert(
    std::make_pair(context,
        PCTable(pcTableInfo.assoc, pcTableInfo.numEntries,
        pcTableInfo.indexingPolicy, pcTableInfo.replacementPolicy,
        TDTEntry())));

    return &(insertion_result.first->second);
}

void
TDTPrefetcher::notifyFill(const PacketPtr &ptr)
{
    //A cache line has been filled in
    printf("cache line fill \n");
}

void
TDTPrefetcher::notifyPrefetchFill(const PacketPtr &ptr)
{
    //A cache line that was prefetched has been filled in
    printf("cache line prefetch fill \n");
}

void
TDTPrefetcher::calculatePrefetch(const PrefetchInfo &pfi,
                                 std::vector<AddrPriority> &addresses)
{
    if (!pfi.hasPC()) {
        DPRINTF(HWPrefetch, "Ignoring request with no PC.\n");
        return;
    }

    // access_addr is the memory address (of the cache line) requested
    Addr access_addr = pfi.getAddr();
    // access pc is the pc of the inst that requests the cache line
    Addr access_pc = pfi.getPC();

    // context can be ignored
    int context = 0;

    // Currently implemented prefetching algorithm: Next line prefetching
    // TODO: Implement something better!
    addresses.push_back(AddrPriority(access_addr + blkSize, 0));

    // Can safely be ignored
    // Get matching storage of entries
    // Context is 0 due to single-threaded application
    PCTable* pcTable = findTable(context);

    // Get matching entry for your given PC from the PC Table
    TDTEntry *entry = pcTable->findEntry(access_pc, false);

    // Check if you have entry
    if (entry != nullptr) {
        // There is an entry for this PC
        // You might want to update information for this entry
    } else {
        // No entry for this PC
        // You might want to make an entry for this PC
    }

    // The following show you how to add an entry to PCTable for a PC
    // All slots are by default taken, you must replace a previous slot with new data
    // Find replacement victim for your new data, update information
    TDTEntry* victim = pcTable->findVictim(access_pc);
    victim->lastAddr = access_addr;
    pcTable->insertEntry(access_pc, false, victim);

}

uint32_t
TDTPrefetcherHashedSetAssociative::extractSet(const Addr pc) const
{
    const Addr hash1 = pc >> 1;
    const Addr hash2 = hash1 >> tagShift;
    return (hash1 ^ hash2) & setMask;
}

Addr
TDTPrefetcherHashedSetAssociative::extractTag(const Addr addr) const
{
    return addr;
}

}
}
