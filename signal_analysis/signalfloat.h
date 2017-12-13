#pragma once

#include "signal.h"

class CSignalFloat : public CSignal
{
public:
	CSignalFloat(QString strName, unsigned char width, double factor = 1);
	QVector<double> m_vData;
private:
	void HandleData();
	double m_factor;
};