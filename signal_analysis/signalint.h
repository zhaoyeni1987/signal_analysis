#pragma once

#include "signal.h"

class CSignalInt : public CSignal
{
public:
	CSignalInt(QString strName, unsigned char width);
	QVector<int> m_vData;
	QVector<double> m_vDataf;
private:
	void HandleData();

	
};