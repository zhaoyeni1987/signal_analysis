#include "signalfloat.h"

CSignalFloat::CSignalFloat(QString strName, unsigned char width, double factor) : CSignal(strName), m_factor(factor)
{
	SetType(EN_FLOAT);

	m_signalWidth = width;

	max = 0;
	min = 0;
}

void CSignalFloat::HandleData()
{
	double value = 0;

	if (m_signalWidth == 1)
	{
		value = ((char)m_aszBuffer[0])*m_factor;
	}
	else if (m_signalWidth == 2)
	{
		unsigned short* pTemp = (unsigned short*)m_aszBuffer;
		value = ((short)(*pTemp))*m_factor;
	}
	else if (m_signalWidth == 4)
	{
		unsigned int* pTemp = (unsigned int*)m_aszBuffer;
		value = ((int)(*pTemp))*m_factor;
	}
	else if (m_signalWidth == 8)
	{
		double* pTemp = (double*)m_aszBuffer;
		value = *pTemp;
	}

	if (value >= max)
		max = value;

	if (value <= min)
		min = value;

	m_vData << value;
}