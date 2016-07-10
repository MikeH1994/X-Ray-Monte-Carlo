#ifndef _H_RAND_H_
#define _H_RAND_H_


class RandomGenerator {
	//pg 354
	//https://books.google.co.uk/books?id=-OHpBwAAQBAJ&pg=PA354&lpg=PA354&dq=mlcg+avoiding+overflow&source=bl&ots=jPeTqMm2gu&sig=OHvID6qxD5e9qF26vqeXvZqFkJU&hl=en&sa=X&ved=0ahUKEwiTn6uJ_KnKAhXIXB4KHfyVCfgQ6AEIHzAA#v=onepage&q=mlcg%20avoiding%20overflow&f=false
	//http://www.eternallyconfuzzled.com/tuts/algorithms/jsw_tut_rand.aspx
public:
	RandomGenerator() {
		this->m = 2147483647;
		this->a = 16807;
		this->q = m / a;
		this->r = m%a;
		this->initialSeed = 4;
		this->ni = initialSeed;
	}
	RandomGenerator(int _initialSeed) :initialSeed(_initialSeed), ni(initialSeed) {
		this->m = 2147483647;
		this->a = 16807;
		this->q = m / a;
		this->r = m%a;
	}
	double getRand() {
		this->ni = a*(ni%q) - r*(ni / q);
		if (ni <= 0) {
			ni += m;
		}
		return ni / (double)m;
	}
	int getInitialSeed() {
		return this->initialSeed;
	}
private:
	int initialSeed;
	int ni;
	int m;
	int a;
	int q;
	int r;
};


#endif
