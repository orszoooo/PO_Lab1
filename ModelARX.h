#pragma once
#include <vector>
#include <deque>
#include <iostream>
#include "ObiektSISO.h"

class ModelARX : ObiektSISO {
private:
	unsigned int dA, dB, k; //rozmiary buforów k - opóŸnienie transportowe
	std::vector<double> wspolWielA, wspolWielB;
	double odchStd;
	std::deque<double> sygWe, sygWy, sygOpK;

	double symZaklocenie(double u);

public:
	//Settery wspó³czynników wielomianów A i B
	void setWspolWielA(std::vector<double> noweWspol);
	void setWspolWielB(std::vector<double> noweWspol);
	void setOpoznienieT(unsigned int wartZadana);
	void setOdchStd(double noweOdchStd);
	double symuluj(double u);

	ModelARX(std::vector<double> wielA, std::vector<double> wielB, unsigned int opoznienie = 1, double odchylenie = 0.0);
	~ModelARX();
};
