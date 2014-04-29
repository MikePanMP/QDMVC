#include "QXmlRpcServer.h"
#include "QXmlRpcServerConnection.h"

void QXmlRpcServer::addMethod(QString method,
	 QObject* responseObject, const char* responseSlot) {
	objectMap[method] = responseObject;
	slotMap[method] = responseSlot;
}

void QXmlRpcServer::removeMethod(QString method) {
	objectMap.remove(method);
	slotMap.remove(method);
}

void QXmlRpcServer::getMethod(QString method, QObject **responseObject, const char **responseSlot) {
	if(!objectMap.contains(method)) {
		*responseObject = NULL;
		*responseSlot = NULL;
		return;
	}
	*responseObject = objectMap[method];
	*responseSlot = slotMap[method];
}

void QXmlRpcServer::listen(quint16 port) {
	//bool ret = server.listen(QHostAddress::Any, port);
	bool ret = server.listen(QHostAddress::Any, port);
	qDebug() << "listen on" << port << ", result" << ret;
}

QXmlRpcServer::QXmlRpcServer(QObject* parent) 
	: QObject(parent), allowedAddresses(NULL) {
	connect(&server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

QXmlRpcServer::~QXmlRpcServer() {
}

void QXmlRpcServer::newConnection() {
	qDebug() << "newConnection called";
	QTcpSocket *connection = server.nextPendingConnection();
	if (!this->allowedAddresses || 
		this->allowedAddresses->isEmpty() || 
		this->allowedAddresses->contains(connection->peerAddress())) {

		QXmlRpcServerConnection *client = new QXmlRpcServerConnection(connection, this);
		connect(client, SIGNAL(getMethod(QString, QObject **, const char**)),
			this, SLOT(getMethod(QString, QObject **, const char**)));
	} else {
		qWarning() << "Rejected connection attempt from" << connection->peerAddress().toString();
		connection->disconnectFromHost();
	}
}