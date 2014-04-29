
#ifndef Q_XML_RPC_FAULT_H
#define Q_XML_RPC_FAULT_H

#include <QtCore>
#include <QtXml>

#include "QXmlHelper.h"

class QXmlRpcFault : public QObject {
	Q_OBJECT
public:
	QXmlRpcFault(int faultCode = 0, QString faultString = QString(), QObject *parent = 0);
	QXmlRpcFault(const QXmlRpcFault &other);
	QString toString();
	QMap<QString, QVariant> fault;
private:
	QXmlHelper helper;
};

Q_DECLARE_METATYPE(QXmlRpcFault)


#endif
