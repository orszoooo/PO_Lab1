﻿#pragma once

class ObiektSISO {
public:
	virtual double symuluj(double u) = 0;
	virtual ~ObiektSISO() {};//wirtualny destruktor umożliwiający polimorfizm
};