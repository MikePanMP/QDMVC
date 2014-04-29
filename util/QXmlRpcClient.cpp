#include "QXmlRpcClient.h"

void QXmlRpcClient::call(QXmlRpcCall & cp) {
	qDebug() << "rpc call";
	QString method = cp.getMethod();
	QList<QVariant> args = cp.getArgs();
	QXmlRpcClientObject* c = new QXmlRpcClientObject(this);
	QString prepared = c->prepareCall(method, args);
	qDebug() << "request:" << prepared;
	QByteArray block;
	block.append(prepared.toUtf8());
	socket->write(block);
	socket->waitForBytesWritten();
	qDebug() << "waitForBytesWritten";
	socket->waitForReadyRead();
	qDebug() << "waitForReadyRead";
	QString response = socket->readAll();
	qDebug() << "client get response:" << response;
	QVariant ret = helper.ResponseToVariant(response);
	qDebug() << ret;
	cp.setReturn(ret);
}

void QXmlRpcClient::connectToHost(const QHostAddress &address, quint16 port) {
	socket->connectToHost(address, port);
	socket->waitForConnected();
	qDebug() << "connected:";
}

void QXmlRpcClient::connectToHost(const QString &address, quint16 port) {
	socket->connectToHost(address, port);
	socket->waitForConnected();
	qDebug() << "connected:";
}
void QXmlRpcClient::connectToHost(quint16 port) {
	qDebug() << "connectToHost on " << port;
	//socket->connectToHost(QHostAddress::Any, port);
	socket->connectToHost(QHostAddress::LocalHost, port);
	socket->waitForConnected();
	//qDebug() << "connected:" << ret;
}

QXmlRpcClient::QXmlRpcClient(QObject* parent) : QObject(parent), connectedState(false) {
	socket = new QTcpSocket(this);
	connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
	connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

QXmlRpcClient::~QXmlRpcClient() {
	if (socket)
		delete socket;
	socket = 0;
}