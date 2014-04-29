#ifndef Q_XML_RPC_SERVER_H
#define Q_XML_RPC_SERVER_H

#include <QtCore>
#include <QtXml>
#include <QtNetwork>

/*
 * QXmlRpcServer
 */
class QXmlRpcServer : public QObject {
	Q_OBJECT
public:
	void addMethod(QString method, QObject *responseObject, const char* responseSlot);
	void removeMethod(QString method);
public:
	void listen(quint16 port = 9527);
public:
	QXmlRpcServer(QObject* parent = 0);
	virtual ~QXmlRpcServer();
public slots:
	void getMethod(QString method, QObject **responseObject, const char** responseSlot);
private slots:
	void newConnection();
private:
	QTcpServer server;
	QHash<QString, QObject*> objectMap;
	QHash<QString, const char*> slotMap;
	QList<QHostAddress> *allowedAddresses;

	friend class QXmlRpcServerConnection;
};
#endif