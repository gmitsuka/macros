using namespace std;

void
EEMCInit()
{
}

void EEMC_Cells(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();

  PHG4ForwardCalCellReco *hc = new PHG4ForwardCalCellReco("EEMCCellReco");
  hc->Detector("EEMC");
  se->registerSubsystem(hc);

  return;
}

void
EEMCSetup(PHG4Reco* g4Reco, const int absorberactive = 0)
{

  gSystem->Load("libg4detectors.so");

  /** Use dedicated EEMC module */
  PHG4CrystalCalorimeterSubsystem *eemc = new PHG4CrystalCalorimeterSubsystem("EEMC");

  /* path to central copy of calibrations repositry */
  ostringstream mapping_eemc;

  /* Use non-projective geometry */
  mapping_eemc << getenv("CALIBRATIONROOT") << "/CrystalCalorimeter/mapping/towerMap_EEMC_v004.txt";
  cout << mapping_eemc.str() << endl;

  eemc->SetTowerMappingFile( mapping_eemc.str() );
  eemc->OverlapCheck(overlapcheck);

  if (absorberactive)  eemc->SetAbsorberActive();

  /* register Ecal module */
  g4Reco->registerSubsystem( eemc );

}

void EEMC_Towers(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();

  ostringstream mapping_eemc;
  mapping_eemc << getenv("CALIBRATIONROOT") <<
    "/CrystalCalorimeter/mapping/towerMap_EEMC_v004.txt";

  RawTowerBuilderByHitIndex* tower_EEMC = new RawTowerBuilderByHitIndex("TowerBuilder_EEMC");
  tower_EEMC->Detector("EEMC");
  tower_EEMC->set_sim_tower_node_prefix("SIM");
  tower_EEMC->GeometryTableFile( mapping_eemc.str() );

  se->registerSubsystem(tower_EEMC);


  /* Calorimeter digitization */

  // CMS lead tungstate barrel ECAL at 18 degree centrigrade: 4.5 photoelectrons per MeV
  const double EEMC_photoelectron_per_GeV = 4500;

  RawTowerDigitizer *TowerDigitizer_EEMC = new RawTowerDigitizer("EEMCRawTowerDigitizer");
  TowerDigitizer_EEMC->Detector("EEMC");
  TowerDigitizer_EEMC->Verbosity(verbosity);
  TowerDigitizer_EEMC->set_raw_tower_node_prefix("RAW");
  TowerDigitizer_EEMC->set_digi_algorithm(RawTowerDigitizer::kSimple_photon_digitalization);
  TowerDigitizer_EEMC->set_pedstal_central_ADC(0);
  TowerDigitizer_EEMC->set_pedstal_width_ADC(8);// eRD1 test beam setting
  TowerDigitizer_EEMC->set_photonelec_ADC(1);//not simulating ADC discretization error
  TowerDigitizer_EEMC->set_photonelec_yield_visible_GeV( EEMC_photoelectron_per_GeV );
  TowerDigitizer_EEMC->set_zero_suppression_ADC(16); // eRD1 test beam setting

  se->registerSubsystem( TowerDigitizer_EEMC );


  /* Calorimeter calibration */

  RawTowerCalibration *TowerCalibration_EEMC = new RawTowerCalibration("EEMCRawTowerCalibration");
  TowerCalibration_EEMC->Detector("EEMC");
  TowerCalibration_EEMC->Verbosity(verbosity);
  TowerCalibration_EEMC->set_calib_algorithm(RawTowerCalibration::kSimple_linear_calibration);
  TowerCalibration_EEMC->set_calib_const_GeV_ADC( 1. / EEMC_photoelectron_per_GeV );
  TowerCalibration_EEMC->set_pedstal_ADC( 0 );

  se->registerSubsystem( TowerCalibration_EEMC );

}

void EEMC_Clusters(int verbosity = 0) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  Fun4AllServer *se = Fun4AllServer::instance();

  RawClusterBuilderFwd* ClusterBuilder = new RawClusterBuilderFwd("EEMCRawClusterBuilderFwd");
  ClusterBuilder->Detector("EEMC");
  ClusterBuilder->Verbosity(verbosity);
  se->registerSubsystem( ClusterBuilder );

  return;
}
