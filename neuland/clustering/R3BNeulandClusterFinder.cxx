#include "R3BNeulandClusterFinder.h"
#include "FairLogger.h"
#include <algorithm>
#include <iostream>
#include <vector>

R3BNeulandClusterFinder::R3BNeulandClusterFinder(const Double_t dx,
                                                 const Double_t dy,
                                                 const Double_t dz,
                                                 const Double_t dt,
                                                 const TString input,
                                                 const TString output)
    : FairTask("R3BNeulandClusterFinder")
    , fDigis(input)
    , fClusters(output)
{
    fClusteringEngine.SetClusteringCondition([=](const R3BNeulandHit& a, const R3BNeulandHit& b) {
        return std::abs(a.GetPosition().X() - b.GetPosition().X()) < dx &&
               std::abs(a.GetPosition().Y() - b.GetPosition().Y()) < dy &&
               std::abs(a.GetPosition().Z() - b.GetPosition().Z()) < dz && std::abs(a.GetT() - b.GetT()) < dt;
    });
}

InitStatus R3BNeulandClusterFinder::Init()
{
    fDigis.Init();
    fClusters.Init();
    return kSUCCESS;
}

void R3BNeulandClusterFinder::Exec(Option_t*)
{
    fClusters.Reset();

    auto digis = fDigis.RetrieveObjects();
    const auto nDigis = digis.size();

    // Group them using the clustering condition set above: vector of digis -> vector of vector of digis
    auto clusteredDigis = fClusteringEngine.Clusterize(digis);
    const auto nClusters = clusteredDigis.size();

    LOG(DEBUG) << "R3BNeulandClusterFinder - nDigis nCluster:" << nDigis << " " << nClusters;

    for (auto& cluster : clusteredDigis)
    {
        fClusters.Insert(R3BNeulandCluster{ std::move(cluster) });
    }
}

ClassImp(R3BNeulandClusterFinder);
