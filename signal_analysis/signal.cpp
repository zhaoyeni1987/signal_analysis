#include "signal.h"

CSignal::CSignal(QString strName) : m_strName(strName)
{
	m_signalWidth = 0;
	m_dataSize = 0;
	memset(m_aszBuffer, 0, sizeof(8));
}

void CSignal::SetType(EN_SIGNAL_TYPE enSignalType)
{
	m_enSignalType = enSignalType;
}

void CSignal::SetByteWidth(unsigned char width)
{
	m_signalWidth = width;
}

void CSignal::InputData(unsigned char* &pData)
{
	memcpy(m_aszBuffer, pData, m_signalWidth);

	pData += m_signalWidth;

	HandleData();

	m_dataSize++;
}

QString CSignal::GetName()
{
	return m_strName;
}

EN_SIGNAL_TYPE CSignal::GetType()
{
	return m_enSignalType;
}

unsigned char CSignal::GetLength()
{
	return m_signalWidth;
}

unsigned int CSignal::GetDataSize()
{
	return m_dataSize;
}