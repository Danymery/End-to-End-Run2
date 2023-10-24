#include "RecHit2.h"

RecHit2::RecHit2(const edm::ParameterSet& iConfig){
photonToken_ 	= consumes<reco::PhotonCollection>(iConfig.getParameter<edm::InputTag>("fotones"));
EBRecHit_ 	= consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("CollectionEBRecHit"));	
EERecHit_ 	= consumes<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("CollectionEERecHit"));
HBHERecHitT_ 	= consumes<HBHERecHitCollection>(iConfig.getParameter<edm::InputTag>("CollectionHBHERecHit"));

  // Initialize file writer
  // NOTE: initializing dynamic-memory histograms outside of TFileService
  // will cause memory leaks
  
  usesResource("TFileService");
  edm::Service<TFileService> fs;
  histogram = fs->make<TH1F>("h_sel", "isSelected;isSelected;Events", 2, 0., 2.);

  //////////// TTree //////////

  // These will be use to create the actual images
   mtree = fs->make<TTree>("mtree", "RecHit tree");

   branchesEvtSel(mtree, fs );
   branchesEB (mtree, fs);
   branchesEE(mtree,fs);
   branchesHBHE (mtree, fs );
}

RecHit2::~RecHit2()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}
 

void
RecHit2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

runEvtSel( iEvent, iSetup );
fillEB( iEvent, iSetup );
fillEE( iEvent, iSetup );
fillHBHE( iEvent, iSetup );
}

void
RecHit2::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
RecHit2::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RecHit2::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

}

//define this as a plug-in
DEFINE_FWK_MODULE(RecHit2);
