#ifndef _H_PHOTON_H_
#define _H_PHOTON_H_

class Photon {
private:
	double E;
  double E_init;
	double x;
	double theta;
public:
	Photon(double E) {
		this->E = E;
    this->E_init = E;
		this->x = 0;
		this->theta = 0;
	}
	Photon() {
		this->E = 0;
    this->E_init = 0;
		this->x = 0;
		this->theta = 0;
	}
	double getE() {
		return this->E;
	}
 double getE_init(){
    return this->E_init;
 }
	double getX() {
		return this->x;
	}
	double getTheta() {
		return this->theta;
	}
	void setE(double E) {
		this->E = E;
	}
	void setX(double x) {
		this->x = x;
	}
	void setTheta(double theta) {
		this->theta = theta;
	}
};

#endif
