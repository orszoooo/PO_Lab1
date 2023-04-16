#include "ModelARX.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <numeric>

double ModelARX::symZaklocenie(double u) {
	double zaklocenie = 0.0;
	if (s_odchStd > 0.0) { //Bez instrukcji warunkowej program nie kompiluje sie dla odchStd == 0.0. Standard zabrania podawania odchylenia standardowego 0.0 do std::normal_distribution
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<double> zaklocenieNrm(0.0, s_odchStd);
		zaklocenie = zaklocenieNrm(gen);
	}
	
	return zaklocenie;
}

double ModelARX::odpModelu(std::vector<double> wielomian, std::deque<double> kolejkaSyg) { //przetwarzanie wielomianów 
	return std::inner_product(kolejkaSyg.begin(), kolejkaSyg.end(), wielomian.begin(), 0.0);
}

void ModelARX::setWspolWielA(std::vector<double> noweWspol) {
	s_dA = static_cast<unsigned int>(noweWspol.size());
	s_wspolWielA = noweWspol;
}

void ModelARX::setWspolWielB(std::vector<double> noweWspol) {
	s_dB = static_cast<unsigned int>(noweWspol.size());
	s_wspolWielB = noweWspol;
}

void ModelARX::setOpoznienieT(unsigned int wartZadana) {
	if (wartZadana < 0) {
		std::cerr << "Wartoœæ opóŸnienia musi byæ dodatnia! Zostanie u¿yta domyœlna wartoœæ 1." << std::endl;
		s_k = 1;
	}
	else {
		s_k = wartZadana;
	}
}

void ModelARX::setOdchStd(double noweOdchStd) {
	if (noweOdchStd < 0.0) {
		std::cerr << "Wartoœæ opóŸnienia musi byæ dodatnia! Zostanie u¿yta domyœlna wartoœæ 0.0!" << std::endl;
		s_odchStd = 0.0;
	}
	else {
		s_odchStd = noweOdchStd;
	}
}

void ModelARX::obslugaWej(double& u)
{
	//Obs³uga opóŸnienia wejœcia
	if (s_sygOpK.size() <= s_k) {
		s_sygOpK.push_front(u); //wprowadzenie opóŸnienia 
	}
	else {
		s_sygOpK.push_front(u); //dodanie nowego sygna³u na koniec kolejki

		if (s_sygWe.size() < s_dB) {
			s_sygWe.push_front(s_sygOpK.front()); //przepisanie sygna³u z pocz¹tku kolejki do kolejki sygna³ów wejœciowych
		}
		else {
			s_sygWe.push_front(s_sygOpK.front()); //przepisanie sygna³u z pocz¹tku kolejki do kolejki sygna³ów wejœciowych
			s_sygWe.pop_back();
		}
		s_sygOpK.pop_back(); //usuniêcie najstarszego elementu w kolejce opóŸnienia
	}
}

void ModelARX::obslugaWyj(double& y_i)
{
	//Dodanie wyniku do kolejki sygna³ów wyjœciowych
	if (s_sygWy.size() < s_dA) {
		s_sygWy.push_front(y_i);
	}
	else {
		s_sygWy.push_front(y_i);
		s_sygWy.pop_back();
	}
}

double ModelARX::symuluj(double u) {
	double y_i = 0.0;

	//Obs³uga wejœcia
	obslugaWej(u);

	//Wyjœcie modelu
	y_i = odpModelu(s_wspolWielB, s_sygWe) - odpModelu(s_wspolWielA, s_sygWy) + symZaklocenie(0.0);

	//Obsluga wyjœcia
	obslugaWyj(y_i);

	return y_i;
}

ModelARX::ModelARX(std::vector<double> wielA, std::vector<double> wielB, unsigned int opoznienie, double odchylenie) {
	setWspolWielA(wielA);
	setWspolWielB(wielB);
	setOpoznienieT(opoznienie);
	setOdchStd(odchylenie);
}

ModelARX::~ModelARX() {

}

//============================TESTY===================================//

void Testy_ModelARX::test_ModelARX_brakPobudzenia() {
	//Sygnatura testu:
	std::cerr << "ModelARX (-0.4 | 0.6 | 1 | 0 ) -> test zerowego pobudzenia: ";
	try
	{
		// Przygotowanie danych:
		ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 1, 0);
		constexpr size_t LICZ_ITER = 30;
		std::vector<double> sygWe(LICZ_ITER);      // pobudzenie modelu (tu same 0)
		std::vector<double> spodzSygWy(LICZ_ITER); // spodziewana sekwencja wy (tu same 0)
		std::vector<double> faktSygWy(LICZ_ITER);  // faktyczna sekwencja wy

		// Symulacja modelu:

		for (int i = 0; i < LICZ_ITER; i++)
			faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

		// Walidacja poprawnoœci i raport:
		if (porownanieSekwencji(spodzSygWy, faktSygWy))
			std::cerr << "OK!\n";
		else
		{
			std::cerr << "FAIL!\n";
			raportBleduSekwencji(spodzSygWy, faktSygWy);
		}
	}
	catch (...)
	{
		std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
	}
}

void Testy_ModelARX::test_ModelARX_skokJednostkowy_1() {
	//Sygnatura testu:
	std::cerr << "ModelARX (-0.4 | 0.6 | 1 | 0 ) -> test skoku jednostkowego nr 1: ";

	try
	{
		// Przygotowanie danych:
		ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 1, 0);
		constexpr size_t LICZ_ITER = 30;
		std::vector<double> sygWe(LICZ_ITER);      // pobudzenie modelu 
		std::vector<double> spodzSygWy(LICZ_ITER); // spodziewana sekwencja wy 
		std::vector<double> faktSygWy(LICZ_ITER);  // faktyczna sekwencja wy

		// Symulacja skoku jednostkowego w chwili 1. (!!i - daje 1 dla i != 0);
		for (int i = 0; i < LICZ_ITER; i++)
			sygWe[i] = !!i;
		spodzSygWy = { 0, 0, 0.6, 0.84, 0.936, 0.9744, 0.98976, 0.995904, 0.998362, 0.999345, 0.999738, 0.999895, 0.999958, 0.999983, 0.999993, 0.999997, 0.999999, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		// Symulacja modelu:
		for (int i = 0; i < LICZ_ITER; i++)
			faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

		// Walidacja poprawnoœci i raport:
		if (porownanieSekwencji(spodzSygWy, faktSygWy))
			std::cerr << "OK!\n";
		else
		{
			std::cerr << "FAIL!\n";
			raportBleduSekwencji(spodzSygWy, faktSygWy);
		}
	}
	catch (...)
	{
		std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
	}
}

void Testy_ModelARX::test_ModelARX_skokJednostkowy_2() {
	//Sygnatura testu:
	std::cerr << "ModelARX (-0.4 | 0.6 | 2 | 0 ) -> test skoku jednostkowego nr 2: ";

	try
	{
		// Przygotowanie danych:
		ModelARX instancjaTestowa({ -0.4 }, { 0.6 }, 2, 0);
		constexpr size_t LICZ_ITER = 30;
		std::vector<double> sygWe(LICZ_ITER);      // pobudzenie modelu, 
		std::vector<double> spodzSygWy(LICZ_ITER); // spodziewana sekwencja wy
		std::vector<double> faktSygWy(LICZ_ITER);  // faktyczna sekwencja wy

		// Symulacja skoku jednostkowego w chwili 1. (!!i - daje 1 dla i != 0);
		for (int i = 0; i < LICZ_ITER; i++)
			sygWe[i] = !!i;
		spodzSygWy = { 0, 0, 0, 0.6, 0.84, 0.936, 0.9744, 0.98976, 0.995904, 0.998362, 0.999345, 0.999738, 0.999895, 0.999958, 0.999983, 0.999993, 0.999997, 0.999999, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

		// Symulacja modelu:
		for (int i = 0; i < LICZ_ITER; i++)
			faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

		// Walidacja poprawnoœci i raport:
		if (porownanieSekwencji(spodzSygWy, faktSygWy))
			std::cerr << "OK!\n";
		else
		{
			std::cerr << "FAIL!\n";
			raportBleduSekwencji(spodzSygWy, faktSygWy);
		}
	}
	catch (...)
	{
		std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
	}
}

void Testy_ModelARX::test_ModelARX_skokJednostkowy_3() {
	//Sygnatura testu:
	std::cerr << "ModelARX (-0.4, 0.2 | 0.6, 0.3 | 2 | 0 ) -> test skoku jednostkowego nr 3: ";
	try
	{
		// Przygotowanie danych:
		ModelARX instancjaTestowa({ -0.4,0.2 }, { 0.6, 0.3 }, 2, 0);
		constexpr size_t LICZ_ITER = 30;
		std::vector<double> sygWe(LICZ_ITER);      // pobudzenie modelu, 
		std::vector<double> spodzSygWy(LICZ_ITER); // spodziewana sekwencja wy
		std::vector<double> faktSygWy(LICZ_ITER);  // faktyczna sekwencja wy

		// Symulacja skoku jednostkowego w chwili 1. (!!i - daje 1 dla i != 0);
		for (int i = 0; i < LICZ_ITER; i++)
			sygWe[i] = !!i;
		spodzSygWy = { 0, 0, 0, 0.6, 1.14, 1.236, 1.1664, 1.11936, 1.11446, 1.12191, 1.12587, 1.12597, 1.12521, 1.12489, 1.12491, 1.12499, 1.12501, 1.12501, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125, 1.125 };

		// Symulacja modelu:
		for (int i = 0; i < LICZ_ITER; i++)
			faktSygWy[i] = instancjaTestowa.symuluj(sygWe[i]);

		// Weryfikacja poprawnoœci i raport:
		if (porownanieSekwencji(spodzSygWy, faktSygWy))
			std::cerr << "OK!\n";
		else
		{
			std::cerr << "FAIL!\n";
			raportBleduSekwencji(spodzSygWy, faktSygWy);
		}
	}
	catch (...)
	{
		std::cerr << "INTERUPTED! (niespodziwany wyjatek)\n";
	}
}

void Testy_ModelARX::raportBleduSekwencji(std::vector<double>& spodz, std::vector<double>& fakt) {
	constexpr size_t PREC = 3;
	std::cerr << std::fixed << std::setprecision(PREC);
	std::cerr << "  Spodziewany:\t";
	for (auto& el : spodz)
		std::cerr << el << ", ";
	std::cerr << "\n  Faktyczny:\t";
	for (auto& el : fakt)
		std::cerr << el << ", ";
	std::cerr << std::endl << std::endl;
}

bool Testy_ModelARX::porownanieSekwencji(std::vector<double>& spodz, std::vector<double>& fakt) {
	constexpr double TOL = 1e-3;	// tolerancja dla porównañ zmiennoprzecinkowych
	bool result = fakt.size() == spodz.size();
	for (int i = 0; result && i < fakt.size(); i++)
		result = fabs(fakt[i] - spodz[i]) < TOL;
	return result;
}

void Testy_ModelARX::uruchom() {
	test_ModelARX_brakPobudzenia();
	test_ModelARX_skokJednostkowy_1();
	test_ModelARX_skokJednostkowy_2();
	test_ModelARX_skokJednostkowy_3();
}