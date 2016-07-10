#ifndef _H_PHOTONHANDLER_H_
#define _H_PHOTONHANDLER_H_
#include "RandomGenerator.h"
#include "DataFromFile.h"
#include "Photon.h"
#include <cmath>


enum RESULT {
	TRANSMITTED, ABSORBED, SCATTERED
};

class PhotonHandler {
private:
	RandomGenerator *rand;
	DataFromFile *synchrotron;
	DataFromFile *attenuation;
	bool calculateAttenuation;
	bool print;
	int atomicNumber;
	double electronDensity;
	double atomicDensity;
	double length;
	double pi;
	double r0;
	double m_e;
	double c;
	
	void setConstants() {
		this->print = false;
		this->pi = 3.14159265;
		this->r0 = 2.8179E-15;
		this->m_e = 9.11E-31;
		this->c = 3E8;
	}
	void performComptonScattering(Photon &photon, double lambdaCom) {
		double E = photon.getE();
		double sigmaCompton = 1. / lambdaCom / this->electronDensity;
		double cosTheta;
		double gamma = E / (m_e*c*c);
		double gammaPrime;
		double EPrime;
		double dSigma_dTheta;
		double P;
		while (true) {
			cosTheta = -1 + 2 * rand->getRand();
			EPrime = E / (1 + E / (m_e*c*c)*(1 - cosTheta));
			gammaPrime = EPrime / (m_e*c*c);
			dSigma_dTheta = pi*r0*r0 * gammaPrime*gammaPrime / (gamma*gamma)*(gamma / gammaPrime + gammaPrime / gamma + cosTheta*cosTheta - 1);

			P = dSigma_dTheta / sigmaCompton;
			if (rand->getRand() < P) {
				break;
			}
		}
		photon.setE(EPrime);
		photon.setTheta(acos(cosTheta));
	}
	RESULT processPhoton(Photon &photon) {
		RESULT nextStep;
		while (true) {
			nextStep = getNextStep(photon);
			if (nextStep == ABSORBED) {
				return ABSORBED;
			}
			if (nextStep == TRANSMITTED) {
				return TRANSMITTED;
			}
		}
		return ABSORBED;
	}
	RESULT getNextStep(Photon &photon) {
		double E = photon.getE();
		double lambdaCom = getComptonAttenuation(E);
		double lambdaPh = getPhotoElectricAttenuation(E);
		double lambdaTot = 1. / (1. / lambdaPh + 1. / lambdaCom);
		double distanceTravelled = -lambdaTot*log(rand->getRand())*cos(photon.getTheta());
		photon.setX(photon.getX() + distanceTravelled);
		
		std::cout << "lC " << lambdaCom << ", lPh " << lambdaPh << std::endl;

		if (photon.getX() > this->length) {
			return TRANSMITTED;
		}

		double r = rand->getRand();
	
		if (r < lambdaTot / lambdaPh) {
			//process that occurs is photoelectric absorbtion
			return ABSORBED;
		}
		else {
			performComptonScattering(photon,lambdaCom);
			return SCATTERED;
		}
	}
	
public:
	PhotonHandler(DataFromFile *synchrotron, DataFromFile *attenuation, double length, double electronDensity) {
		this->rand = new RandomGenerator(12345);
		this->synchrotron = synchrotron;
		this->attenuation = attenuation;
		this->length = length;
		this->electronDensity = electronDensity;
		setConstants();
	}
	PhotonHandler(DataFromFile *synchrotron, double length, double electronDensity,double atomicDensity,int atomicNumber) {
		this->calculateAttenuation = true;
		this->rand = new RandomGenerator(12345);
		this->synchrotron = synchrotron;
		this->length = length;
		this->electronDensity = electronDensity;
		this->atomicDensity = atomicDensity;
		this->atomicNumber = atomicNumber;
		setConstants();
	}
	~PhotonHandler() {
		 delete rand;
	}
	void run(int nPhotons, bool returnNPhotons) {
		// if true, keep running until n Photons are returned
		if (!returnNPhotons){
			for (int i = 0; i < nPhotons; i++) {
				Photon photon(synchrotron->getPhoton());
				if (processPhoton(photon) == TRANSMITTED) {
					//hist.Fill(photon.getE_init());
				}
			}
		}
		else {
			int photonCounter = 0;
			while (photonCounter <= nPhotons) {
				Photon photon(synchrotron->getPhoton());
				if (processPhoton(photon) == TRANSMITTED) {
					//hist.Fill(photon.getE_init());
				}
			}
		}
	}
	double getComptonAttenuation(double E) {
		double gamma = E / (m_e*c*c);
		double Z = 1 + 2 * gamma;
		double sigma = 2 * pi*r0*r0* ((1 + gamma) / (gamma*gamma)*(2 * (1 + gamma) / Z - 1. / gamma*log(Z))
			+ 1. / 2. / gamma*log(Z) - (1 + 3 * gamma) / Z / Z);
		return 1. / (sigma*this->electronDensity);
	}
	double getPhotoElectricAttenuation(double E) {
		//convert from microns to metres and return
		if (!calculateAttenuation) {
			// if the attenuation is being retrieved from a DataFromFile
			return attenuation->val(convertJToEv(E))*1E-6;
		}
		else {
			double sigmaP;

			double z5 = pow(atomicNumber, 5);
			double alpha4 = pow(1. / 137., 4);
			sigmaP = 4 * alpha4 * sqrt(2) * z5 * 8. / 3.*pi*r0*r0*pow(m_e*c*c / E, 3.5);
			return 1. / (sigmaP*atomicDensity);
		}
	}

	static double getElectronDensity(double molecularMass, int molecularAtomicNumber, double density) {
		// density should be entered in SI units
		// calculate molecules per unit volume
		double electronsPerKg = 1000 / molecularMass * molecularAtomicNumber * 6.022E23;
		return density*electronsPerKg;
	}
	static double getAtomicDensity(double molecularMass, double density) {
		double atomsPerKg = 1000 / molecularMass * 6.022E23;
		return density*atomsPerKg;
	}
	static double converteVToJ(double E) {
		return E * 1.6E-19;
	}
	static double convertJToEv(double E) {
		return E / 1.6E-19;
	}
};

#endif
