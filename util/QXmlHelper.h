#ifndef Q_XML_HELPER_H
#define Q_XML_HELPER_H

#include <QtCore>
#include <QtXml>

class QXmlHelper : public QObject {
	Q_OBJECT
public:
	static QDomElement VariantToXml(QVariant arg);
	static QVariant VariantFromXml(const QDomElement &elem);
	static QString VariantToResponse(QVariant arg);
	static QVariant ResponseToVariant(QString response);
private:
	static QString FaultToVariant(int faultCode, const QString & faultString);
public:
	QXmlHelper() {}
	virtual ~QXmlHelper() {}
};

#endif