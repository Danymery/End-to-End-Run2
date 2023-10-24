#include "RecHit2.h"
// Fill EB rec hits ////////////////////////////////
// Store event rechits in a vector of length equal
// to number of crystals in EB (ieta:170 x iphi:360)

TProfile2D *hEB_energia;
TProfile2D *hEB_tiempo;
std::vector<float> vEB_energia_;
std::vector<float> vEB_tiempo_;

void RecHit2::branchesEB ( TTree* tree, edm::Service<TFileService> &fs ) {

  // Branches for images
  mtree->Branch("EB_energy", &vEB_energia_);
  mtree->Branch("EB_time",   &vEB_tiempo_);

  // Histograms for monitoring
  hEB_energia = fs->make<TProfile2D>("EB_energy", "E(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX  , EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*EB_IETA_MAX,-EB_IETA_MAX,   EB_IETA_MAX );
  hEB_tiempo = fs->make<TProfile2D>("EB_time", "t(i#phi,i#eta);i#phi;i#eta",
      EB_IPHI_MAX  , EB_IPHI_MIN-1, EB_IPHI_MAX,
      2*EB_IETA_MAX,-EB_IETA_MAX,   EB_IETA_MAX );

} 

void RecHit2::fillEB(const edm::Event& iEvent, const edm::EventSetup& iSetup ) {


 	int iphi_, ieta_, idx_; // rows:ieta, cols:iphi
  	float energy_;

        vEB_energia_.assign( EBDetId::kSizeForDenseIndexing, 0. );

        vEB_tiempo_.assign( EBDetId::kSizeForDenseIndexing, 0. );

        edm::Handle<EBRecHitCollection>EBRecHitsH_;
        iEvent.getByToken(EBRecHit_ , EBRecHitsH_);

// Fill EB rechits

      for ( EcalRecHitCollection::const_iterator iRHit = EBRecHitsH_->begin(); iRHit != EBRecHitsH_->end(); ++iRHit ) {

        energy_ = iRHit->energy();

        if ( energy_ <= 0 ) continue;

        // Get detector id and convert to histogram-friendly coordinates
        EBDetId ebId( iRHit->id() );
        iphi_ = ebId.iphi() - 1;
        ieta_ = ebId.ieta() > 0 ? ebId.ieta()-1 : ebId.ieta();

        // Fill histograms for monitoring
        hEB_energia->Fill( iphi_,ieta_,energy_ );
        hEB_tiempo->Fill( iphi_,ieta_,iRHit->time() );

        // Get Hashed Index: provides convenient
        // index mapping from [ieta][iphi] -> [idx]

         idx_ = ebId.hashedIndex(); // (ieta_+EB_IETA_MAX)*EB_IPHI_MAX + iphi_

        // Fill vectors for images
        vEB_energia_[idx_] = energy_;
        vEB_tiempo_[idx_] = iRHit->time();
        }


}
