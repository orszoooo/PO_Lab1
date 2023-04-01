#pragma once

class ObiektSISO {
public:
	virtual double symuluj(double u) { return 0.0; }
	virtual ~ObiektSISO() {}//wirtualny destruktor umożliwiający polimorfizm
};