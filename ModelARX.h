#pragma once
#include "ObiektSISO.h"

#include <vector>
#include <deque>

class ModelARX : ObiektSISO {
private:
	unsigned int s_dA, s_dB, s_k; //rozmiary buforów k - opóŸnienie transportowe
	double s_odchStd;		//s_ oznacza sk³adow¹ klasy
	std::vector<double> s_wspolWielA, s_wspolWielB;
	std::deque<double> s_sygWe, s_sygWy, s_sygOpK;

	double symZaklocenie(double u);
	double odpModelu(std::vector<double> wielomian, std::deque<double> kolejkaSyg);
public:
	//Settery wspó³czynników wielomianów A i B
	void setWspolWielA(std::vector<double> noweWspol);
	void setWspolWielB(std::vector<double> noweWspol);
	
	void setOpoznienieT(unsigned int wartZadana);
	void setOdchStd(double noweOdchStd);
	void obslugaWej(double& u);
	void obslugaWyj(double& y_i);
	double symuluj(double u);

	friend class Testy_ModelARX;

	ModelARX(std::vector<double> wielA, std::vector<double> wielB, unsigned int opoznienie = 1, double odchylenie = 0.0);
	~ModelARX();
};


class Testy_ModelARX {
private:
	static void test_ModelARX_brakPobudzenia();
	static void test_ModelARX_skokJednostkowy_1();
	static void test_ModelARX_skokJednostkowy_2();
	static void test_ModelARX_skokJednostkowy_3();
	static void raportBleduSekwencji(std::vector<double>& spodz, std::vector<double>& fakt);
	static bool porownanieSekwencji(std::vector<double>& spodz, std::vector<double>& fakt);
public:
	static void uruchom();
};