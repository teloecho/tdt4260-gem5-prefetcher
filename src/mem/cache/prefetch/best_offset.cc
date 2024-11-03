/*
 * 2024 oystebw, teloecho
 */

/**
 * @file
 * Our implementation of a Best Offset Prefetcher following the paper of
 * Pierre Michaud https://inria.hal.science/hal-01254863v1.
 */


#include "mem/cache/prefetch/best_offset.hh"

//#include "mem/cache/replacement_policies/base.hh"
#include "params/BestOffsetPrefetcher.hh"

namespace gem5
{

GEM5_DEPRECATED_NAMESPACE(Prefetcher, prefetch);
namespace prefetch
{


BestOffsetPrefetcher::BestOffsetPrefetcher(const BestOffsetPrefetcherParams &params)
    : Queued(params), SCOREMAX(params.scoreMax), ROUNDMAX(params.roundMax), BADSCORE(params.badScore),
    RR_SIZE(params.rrSize) {}


void
BestOffsetPrefetcher::calculatePrefetch(const PrefetchInfo &pfi,
                                        std::vector<AddrPriority> &addresses){
                                       
    Addr accessAddr = pfi.getAddr();
    if (!pfi.hasPC()) {
        return;
    }
    Addr tag = (accessAddr >> log2blockSize);

    Addr tagToBeTested = tag - this->offsetScorePair[this->subround].first; // (X - d_i)
    // if this resides in the RR, we increment the corresponding score
    for(auto& recent: this->recentRequests){
        if(recent == tagToBeTested){
            this->offsetScorePair[this->subround].second++;
        }
    }

    Addr bestOffset = this->bestOffset << log2blockSize;

    // at the end of a subround, we do the following
    this->subround++;
    if(this->subround != this->NUMBER_OF_OFFSETS){ // we don't conclude in the middle of a round
        if(this->prefetcherEnabled && (accessAddr >> 12) == ((accessAddr + bestOffset) >> 12)){ // ensure that we don't cross page boundries
            addresses.push_back(AddrPriority(accessAddr +  bestOffset, 0));
        }
        return;
    }

    // we have finished a round and do some book-keeping
    this->subround = 0;
    this->round++;
    if(this->round < SCOREMAX){ // not possible to reach SCOREMAX yet
        if(this->prefetcherEnabled && (accessAddr >> 12) == ((accessAddr + bestOffset) >> 12)){
            addresses.push_back(AddrPriority(accessAddr +  bestOffset, 0));           
        }
        return;
    }

    // but here we might get a new bestOffset
    int bestOffset_temp = 0;
    int bestScore_temp = 0;
    // we find the offset that has scored the highest so far
    for(auto& pair : this->offsetScorePair){
        if(pair.second > bestScore_temp){
            bestScore_temp = pair.second;
            bestOffset_temp = pair.first;
        }
    }
    // and check if the conditions for terminating this training phase are in place
    if(this->round == ROUNDMAX || bestScore_temp >= SCOREMAX){
        this->bestOffset = bestOffset_temp;
        this->subround = 0;
        this->round = 0;
        for(auto& pair : this->offsetScorePair){
            pair.second = 0;
        }
        if(this->bestOffset > BADSCORE){
            this->prefetcherEnabled = true;
        }
        else{
            this->prefetcherEnabled = false;
        }
    }
    if(this->prefetcherEnabled && (accessAddr >> 12) == ((accessAddr + bestOffset) >> 12)){
        addresses.push_back(AddrPriority(accessAddr +  bestOffset, 0));       
    }
}

void BestOffsetPrefetcher::notifyFill(const PacketPtr &pkt){
    if(!this->prefetcherEnabled){
        Addr tag = (pkt->getAddr() >> log2blockSize);
        // we only care about fills from the L3 that is the result of prefetching
        // making sure that we only store the maximum capacity of RR
        if(this->recentRequests.size() == RR_SIZE){
            this->recentRequests.pop_back();
        }
        this->recentRequests.push_front(tag);
    }
}

void BestOffsetPrefetcher::notifyPrefetchFill(const PacketPtr &pkt){

    Addr tag = (pkt->getAddr() >> log2blockSize);
    // we only care about fills from the L3 that is the result of prefetching
    
    int tagToBeStored = tag - this->bestOffset; // (Y - D) in figure
    // making sure that we only store the maximum capacity of RR
    if(this->recentRequests.size() == RR_SIZE){
        this->recentRequests.pop_back();
    }
    this->recentRequests.push_front(tagToBeStored);
}

} // namespace prefetch
} // namespace gem5
