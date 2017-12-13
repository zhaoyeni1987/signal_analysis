#pragma once

#include "signal.h"

class CSignalUInt : public CSignal
{
public:
	CSignalUInt(QString strName, unsigned char width);
	QVector<unsigned int> m_vData;
	QVector<double> m_vDataf;
private:
	void HandleData();

	
};