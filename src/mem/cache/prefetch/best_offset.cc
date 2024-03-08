#include "mem/cache/prefetch/best_offset.hh"

//#include "mem/cache/replacement_policies/base.hh"
#include "params/BestOffsetPrefetcher.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{


BestOffsetPrefetcher::BestOffsetPrefetcher(const BestOffsetPrefetcherParams &params)
    : Queued(params){}


void
BestOffsetPrefetcher::calculatePrefetch(const PrefetchInfo &pfi,
                                        std::vector<AddrPriority> &addresses){
                                       
    Addr accessAddr = pfi.getAddr();
    if (!pfi.hasPC()) {
        return;
    }
    Addr tag = (accessAddr >> log2blockSize);
    std::cout << "L1 wants tag: " << tag << '\n';

    std::cout << "New prefetch to L3 issued with tag: " << tag + M.bestOffset << '\n';

    Addr tagToBeTested = tag - M.offsetScorePair[M.subround].first; // (X - d_i)
    // if this resides in the RR, we increment the corresponding score
    for(auto& recent: M.recentRequests){
        if(recent == tagToBeTested){
            M.offsetScorePair[M.subround].second++;
        }
    }

    // at the end of a subround, we do the following
    M.subround++;
    if(M.subround != M.NUMBER_OF_OFFSETS){ // we don't conclude in the middle of a round
        if(M.prefetcherEnabled){
            addresses.push_back(AddrPriority(accessAddr +  (M.bestOffset << log2blockSize ), 0));
        }
        return;
    }

    // we have finished a round and do some book-keeping
    M.subround = 0;
    M.round++;
    if(M.round < M.SCORE_MAX){ // not possible to reach SCORE_MAX yet
        if(M.prefetcherEnabled){
            addresses.push_back(AddrPriority(accessAddr +  (M.bestOffset << log2blockSize), 0));
        }
        return;
    }

    // but here we might get a new bestOffset
    int bestOffset_temp = 0;
    int bestScore_temp = 0;
    // we find the offset that has scored the highest so far
    for(auto& pair : M.offsetScorePair){
        if(pair.second > bestScore_temp){
            bestScore_temp = pair.second;
            bestOffset_temp = pair.first;
        }
    }
    // and check if the conditions for terminating this training phase are in place
    if(M.round == M.ROUND_MAX || bestScore_temp >= M.SCORE_MAX){
        M.bestOffset = bestOffset_temp;
        M.subround = 0;
        M.round = 0;
        for(auto& pair : M.offsetScorePair){
            pair.second = 0;
        }
        /*
        if(M.bestOffset > M.BAD_SCORE){
            M.prefetcherEnabled = true;
        }
        else{
            M.prefetcherEnabled = false;
        }
        */
    }
    if(M.prefetcherEnabled){
        addresses.push_back(AddrPriority(accessAddr +  (M.bestOffset << log2blockSize ), 0));
    }
}

void BestOffsetPrefetcher::notifyFill(const PacketPtr &pkt){
    std::cout << "Fill from L3 with tag: " << (pkt->getAddr() >> log2blockSize ) << '\n';
}

void BestOffsetPrefetcher::notifyPrefetchFill(const PacketPtr &pkt){

    Addr tag = (pkt->getAddr() >> log2blockSize);
    // we only care about fills from the L3 that is the result of prefetching
    std:: cout << "Prefetch fill from L3 with tag: " << tag << '\n';
    
    int tagToBeStored = tag - M.bestOffset; // (Y - D) in figure
    // making sure that we only store the maximum capacity of RR
    if(M.recentRequests.size() == M.RR_SIZE){
        M.recentRequests.pop_back();
    }
    M.recentRequests.push_front(tagToBeStored);
}

} // namespace prefetch
} // namespace gem5
