#ifndef Q_XML_RPC_OBJECT_H
#define Q_XML_RPC_OBJECT_H

#include <QtCore>
#include <QtXml>
#include <QtNetwork>

#include "QXmlHelper.h"

/*
 * QXmlRpcClientObject
 */
class QXmlRpcClientObject : public QObject {
	Q_OBJECT
public:
	QString prepareCall(QString method, QList<QVariant> args);
public:
	QXmlRpcClientObject(QObject* parent = 0);
private:
	QXmlHelper helper;
};

#endif