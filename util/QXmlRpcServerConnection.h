#ifndef Q_XML_RPC_SERVER_CONNECTION_H
#define Q_XML_RPC_SERVER_CONNECTION_H

#include <QtCore>
#include <QtXml>
#include <QtNetwork>

#include "QXmlHelper.h"
/*
 * QXmlRpcServerConnection
 */
class QXmlRpcServerConnection : public QObject {
	Q_OBJECT
public:
	QXmlRpcServerConnection(QTcpSocket *connection, QObject *parent = 0);
	~QXmlRpcServerConnection();

signals:
	void getMethod(QString method, QObject **responseObject, const char **responseSlot);

	private slots:
		void handelRequest();

private:
	void sendResponse(QString content);
	void parseCall(QString call);
private:
	bool invokeMethodWithVariants(QObject *obj,
		const QByteArray &method, const QVariantList &args,
		QVariant *ret, Qt::ConnectionType type = Qt::AutoConnection);
	static QByteArray getReturnType(const QMetaObject *obj,
		const QByteArray &method, const QList<QByteArray> argTypes);

private:
	QTcpSocket *clientConnection;
	QXmlHelper helper;
};
#endif