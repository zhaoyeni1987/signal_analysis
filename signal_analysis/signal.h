#pragma once

#include <QString>
#include <QVector>

typedef enum
{
	EN_INT = 0,
	EN_UINT,
	EN_FLOAT
}EN_SIGNAL_TYPE;

class CSignal
{
public:
	CSignal(QString strName);
	void SetType(EN_SIGNAL_TYPE enSignalType);
	void SetByteWidth(unsigned char width);
	void InputData( unsigned char* &pData);

	QString GetName();
	EN_SIGNAL_TYPE GetType();

	unsigned char GetLength();
	unsigned int GetDataSize();
	double max;
	double min;
protected:
	virtual void HandleData() {};

	unsigned char m_aszBuffer[8];
	QString m_strName;
	unsigned char m_signalWidth;
	EN_SIGNAL_TYPE m_enSignalType;

	unsigned int m_dataSize;
};
