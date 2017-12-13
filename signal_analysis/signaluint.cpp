#include "signaluint.h"

CSignalUInt::CSignalUInt(QString strName, unsigned char width) : CSignal(strName)
{
	SetType(EN_UINT);

	m_signalWidth = width;
}

void CSignalUInt::HandleData()
{
	unsigned int value = 0;

	if (m_signalWidth == 1)
	{
		value = m_aszBuffer[0];
	}
	else if (m_signalWidth == 2)
	{
		unsigned short* pTemp = (unsigned short*)m_aszBuffer;
		value = *pTemp;
	}
	else if (m_signalWidth == 4)
	{
		unsigned int* pTemp = (unsigned int*)m_aszBuffer;
		value = *pTemp;
	}

	double fvalue = (double)value;

	if (fvalue >= max)
		max = fvalue;

	if (fvalue <= min)
		min = fvalue;

	m_vData << value;
	m_vDataf << fvalue;
}