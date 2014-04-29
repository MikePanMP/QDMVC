#ifndef Q_XML_RPC_CLIENT_H
#define Q_XML_RPC_CLIENT_H

#include <QtCore>
#include <QtXml>
#include <QtNetwork>

#include "QXmlRpcCall.h"
#include "QXmlRpcObject.h"
#include "QXmlHelper.h"

/*
 * QXmlRpcClient
 */
class QXmlRpcClient : public QObject {
	Q_OBJECT
public:
	void call(QXmlRpcCall & cp);
public:
	void connectToHost(const QHostAddress &address, quint16 port = 9527);
	void connectToHost(const QString &address, quint16 port = 9527);
	void connectToHost(quint16 port = 9527);
	bool isConnected() {
		return connectedState;
	}
public:
	QXmlRpcClient(QObject* parent = 0);
	virtual ~QXmlRpcClient();
	
	private slots:
		void socketConnected() {
			//qDebug() << "connected";
			connectedState = true;
		}
		void socketDisconnected() {
			//qDebug() << "connected";
			connectedState = false;
		}
private:
	QTcpSocket* socket;
	QXmlHelper helper;
	bool connectedState;
};

#endif