#include "ModelARX.h"
#include <iostream>
#include <iomanip>
#include <random>

double ModelARX::symZaklocenie(double u) {
	double zaklocenie = 0.0;
	if (s_odchStd > 0.0) { //Bez instrukcji warunkowej program nie kompiluje sie dla odchStd == 0.0
		std::random_device rd;
		std::mt19937 gen(rd());
		std::normal_distribution<double> zaklNrm(0.0, s_odchStd);
		zaklocenie = zaklNrm(gen);
	}
	
	return zaklocenie;
}

double ModelARX::odpModelu(std::vector<double> wielomian, std::deque<double> kolejkaSyg) { //przetwarzanie wielomian�w 
	double odp = 0.0;

	for (int i = 0; i < kolejkaSyg.size(); i++) {
		odp += wielomian.at(i) * kolejkaSyg.at(i);
	}

	return odp;
}

void ModelARX::setWspolWielA(std::vector<double> noweWspol) {
	s_dA = static_cast<unsigned int>(noweWspol.size());
	wspolWielA = noweWspol;
}

void ModelARX::setWspolWielB(std::vector<double> noweWspol) {
	s_dB = static_cast<unsigned int>(noweWspol.size());
	wspolWielB = noweWspol;
}

void ModelARX::setOpoznienieT(unsigned int wartZadana) {
	if (wartZadana < 0) {
		std::cerr << "Warto�� op�nienia musi by� dodatnia! Zostanie u�yta domy�lna warto�� 1." << std::endl;
		s_k = 1;
	}
	else {
		s_k = wartZadana;
	}
}

void ModelARX::setOdchStd(double noweOdchStd) {
	if (noweOdchStd < 0.0) {
		std::cerr << "Warto�� op�nienia musi by� dodatnia! Zostanie u�yta domy�lna warto�� 0.0!" << std::endl;
		s_odchStd = 0.0;
	}
	else {
		s_odchStd = noweOdchStd;
	}
}

double ModelARX::symuluj(double u) {
	double y_i = 0.0;
	//double tmp_a = 0.0;
	//double tmp_b = 0.0;

	//Obs�uga op�nienia wej�cia
	if (sygOpK.size() <= s_k) { 
		sygOpK.push_front(u); //wprowadzenie op�nienia 
	}
	else {
		sygOpK.push_front(u); //dodanie nowego sygna�u na koniec kolejki

		if (sygWe.size() < s_dB) {
			sygWe.push_front(sygOpK.front()); //przepisanie sygna�u z pocz�tku kolejki do kolejki sygna��w wej�ciowych
		}
		else {
			sygWe.push_front(sygOpK.front()); //przepisanie sygna�u z pocz�tku kolejki do kolejki sygna��w wej�ciowych
			sygWe.pop_back();
		}
		sygOpK.pop_back(); //usuni�cie najstarszego elementu w kolejce op�nienia

		////Obliczanie odpowiedzi modelu(Przeniesionno do odpModelu)
		//for (int i = 0; i < sygWe.size(); i++) {
		//	tmp_b += wspolWielB.at(i) * sygWe.at(i);
		//}

		//for (int i = 0; i < sygWy.size(); i++) {
		//	tmp_a += wspolWielA.at(i) * sygWy.at(i);
		//}
	}

	//Wyj�cie modelu
	y_i = odpModelu(wspolWielB, sygWe) - odpModelu(wspolWielA, sygWy) + symZaklocenie(0.0);

	if (sygWy.size() < s_dA) {
		sygWy.push_front(y_i);
	}
	else {
		sygWy.push_front(y_i);
		sygWy.pop_back();
	}

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

//////// TESTY

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

		// Walidacja poprawno�ci i raport:
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

		// Walidacja poprawno�ci i raport:
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

		// Walidacja poprawno�ci i raport:
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

		// Weryfikacja poprawno�ci i raport:
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
	constexpr double TOL = 1e-3;	// tolerancja dla por�wna� zmiennoprzecinkowych
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