#include "ModelARX.h"

double ModelARX::symZaklocenie(double u) {
	return 0.0;
}

void ModelARX::setWspolWielA(std::vector<double> noweWspol) {
	dA = static_cast<unsigned int>(noweWspol.size());
	wspolWielA = noweWspol;
}

void ModelARX::setWspolWielB(std::vector<double> noweWspol) {
	dB = static_cast<unsigned int>(noweWspol.size());
	wspolWielB = noweWspol;
}

void ModelARX::setOpoznienieT(unsigned int wartZadana) {
	if (wartZadana < 0) {
		std::cerr << "Warto�� op�nienia musi by� dodatnia! Zostanie u�yta domy�lna warto�� 1." << std::endl;
		k = 1;
	}
	else {
		k = wartZadana;
	}
}

void ModelARX::setOdchStd(double noweOdchStd) {
	if (noweOdchStd < 0.0) {
		std::cerr << "Warto�� op�nienia musi by� dodatnia! Zostanie u�yta domy�lna warto�� 0.0!" << std::endl;
		odchStd = 0.0;
	}
	else {
		odchStd = noweOdchStd;
	}
}

double ModelARX::symuluj(double u) {
	double y_i = 0;
	//Obs�uga op�nienia wej�cia
	if (sygOpK.size() < k+1) { //?
		sygOpK.push_front(u); //wprowadzenie op�nienia 
	}
	else {
		sygOpK.push_front(u); //dodanie nowego sygna�u na koniec kolejki

		if (sygWe.size() < dB) {
			sygWe.push_front(sygOpK.front()); //przepisanie sygna�u z pocz�tku kolejki do kolejki sygna��w wej�ciowych
		}
		else {
			sygWe.push_front(sygOpK.front()); //przepisanie sygna�u z pocz�tku kolejki do kolejki sygna��w wej�ciowych
			sygWe.pop_back();
		}
		sygOpK.pop_back(); //usuni�cie najstarszego elementu w kolejce op�nienia

		//Obliczanie odpowiedzi modelu
		double tmp_b = 0;

		for (int i = 0; i < sygWe.size(); i++) {
			tmp_b += wspolWielB.at(i) * sygWe.at(i);
		}

		double tmp_a = 0;

		for (int i = 0; i < sygWy.size(); i++) {
			tmp_a += wspolWielA.at(i) * sygWy.at(i);
		}

		//Wyj�cie modelu
		y_i = tmp_b - tmp_a + symZaklocenie(0.0);

		if (sygWy.size() < dA) {
			sygWy.push_front(y_i);
		}
		else {
			sygWy.push_front(y_i);
			sygWy.pop_back();
		}

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