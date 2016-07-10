#include "RandomGenerator.h"
#include "DataFromFile.h"
#include "Photon.h"
#include "PhotonHandler.h"
#include <cmath>



int main() {
  //TFile file("comptonScattering.root", "recreate");
	DataFromFile *synchrotron = new DataFromFile("xraySynchrotron.dat");
	DataFromFile *attenuation = new DataFromFile("BeF2 Attenuation.dat");
	const int nBins = 100;
  const int nPhotons = 50000;
	const double maxE = synchrotron->getMaxX();
	double E = 0;
	double length = 1E-3;
	double electronDensity = PhotonHandler::getElectronDensity(47.01, 22, 1986);
 
  //TH1D histCompton("histCompton", "Compton histogram", nBins, 0, maxE);
  //TH1D histPh("histPh", "Synchrotron histogram", nBins, 0, maxE);
	PhotonHandler handler(synchrotron, attenuation, length, electronDensity);
	handler.run(10, false);
  //fillAttenuationHistogram(synchrotron,attenuation,histPh,nPhotons,length);

  //file.Write();
  //file.Close();
}
