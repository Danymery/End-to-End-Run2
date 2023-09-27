#include "RecHit2.h"

// Run event selection ////////////////////////////////

TH1F *masa_0;
TH1F *h_photonPt; 
TH1F *h_photonE;
TH1F *h_photonEta;
TH1F *h_photonR9;
TH1F *h_photonSieie;

unsigned long long eventoId_;
unsigned int corridaId_;
unsigned int luminosidadId_;
float masa0_;
float diPhotonE_;
float diPhotonPt_;
std::vector<float> vFC_entradas_;

float m0corte = 90.;

// Initialize branches _____________________________________________________//
void RecHit2::branchesEvtSel ( TTree* tree, edm::Service<TFileService> &fs ) {

  masa_0     = fs->make<TH1F>("Masa"    , "m0;m0;Events"         ,  50, m0corte, m0corte+150.);
  h_photonPt = fs->make<TH1F>("Momento Transversal" , "p_{T};p_{T};Particles", 100,  0., 500.);
  h_photonE   = fs->make<TH1F>("Energia"  , "E;E;Particles"        , 100,  0., 800.);
  h_photonEta = fs->make<TH1F>("Eta", "#eta;#eta;Particles"  , 100, -5., 5.);
  h_photonR9  = fs->make<TH1F>("R9" , "R9;R9;Particles"  , 50, 0., 1.);
  h_photonSieie  = fs->make<TH1F>("Sieie" , "Sieie;Sieie;Particles"  , 50, 0., 0.1);

  mtree->Branch("eventoId",        &eventoId_);
  mtree->Branch("corridaId",          &corridaId_);
  mtree->Branch("luminosidadId",         &luminosidadId_);
  mtree->Branch("masa0",             &masa0_);
  
  mtree->Branch("FC_entradas",      &vFC_entradas_);
  mtree->Branch("diPhotonE",         &diPhotonE_);
  mtree->Branch("diPhotonPt",        &diPhotonPt_);


} 
bool RecHit2::runEvtSel ( const edm::Event& iEvent, const edm::EventSetup& iSetup ){

edm::Handle<reco::PhotonCollection> fotones;
iEvent.getByToken(photonToken_, fotones );
math::PtEtaPhiELorentzVectorD vDiPho;
std::vector<int> vPhoIdxs;
int nPhoTrg = 0;
float leadPhoPt = 0.;
int leadPho = 0.;

if(fotones->size()==2){
	for ( unsigned int iP = 0; iP < fotones->size(); iP++ ) {
	reco::PhotonRef iPho(fotones, iP );
	if ( std::abs(iPho->pt()) < 18. ) continue;
	if ( iPho->r9() < 0.5 ) continue;
    	if ( iPho->hadTowOverEm() > 0.07 ) continue;
    	if ( iPho->full5x5_sigmaIetaIeta() > 0.0105 ) continue;
    	if ( iPho->hasPixelSeed() == true ) continue;
	if ( std::abs(iPho->eta()) > 1.44 && std::abs(iPho->eta()) < 1.57 ) continue;	
	std::cout << " >> pT:" << iPho->pt() << " eta:" << iPho->eta() << " phi: " << iPho->phi() << " E:" << iPho->energy() << std::endl;
	nPhoTrg++;
	
	if ( std::abs(iPho->pt()) > leadPhoPt ){ 
	leadPhoPt = std::abs(iPho->pt()); 
    	leadPho = iP;}
	
	vDiPho += iPho->p4();
	vPhoIdxs.push_back( iP );}
}

 masa0_ = vDiPho.mass();
 if ( masa0_ < m0corte ) return false;

// Get photon order
int ptOrder[2] = {0, 1};
if ( leadPho == 1 ) {
    ptOrder[0] = 1;
    ptOrder[1] = 0;
  }



masa_0->Fill( masa0_ );
diPhotonE_  = 0.;
diPhotonPt_ = 0.;
float dphi[2] = {0., 0.};
vFC_entradas_.clear();
for ( int iP = 0; iP < nPhoTrg; iP++ ) {
    reco::PhotonRef iPho(fotones , vPhoIdxs[ptOrder[iP]] );
    h_photonPt->Fill( iPho->pt() ); 
    h_photonE->Fill( iPho->energy() );
    h_photonEta->Fill( iPho->eta() ); 
    h_photonR9->Fill( iPho->r9() ); 
    h_photonSieie->Fill( iPho->full5x5_sigmaIetaIeta() ); 
    
    diPhotonE_  += std::abs( iPho->energy() );
    diPhotonPt_ += std::abs( iPho->pt() );
    vFC_entradas_.push_back( iPho->pt()/masa0_);
    vFC_entradas_.push_back( iPho->eta() );
    dphi[iP] = iPho->phi();
}
vFC_entradas_.push_back( TMath::Cos(reco::deltaPhi(dphi[0], dphi[1])) );

return true;
}
