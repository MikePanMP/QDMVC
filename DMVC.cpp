#include "DMVC.h"


QString DMVC::LoadHostName(const QString& filename) {
	qDebug() << filename;
	QFile file(filename);
	qDebug() << file.exists();
	if(!file.exists()) 
		return NULL;
	if(!file.open(QIODevice::ReadOnly))
		return NULL;
	QTextStream in(&file);
	QString line = in.readLine();
	file.close();
	qDebug() << line;
	return line;
}

DMVCApp::DMVCApp(int argc, char *argv[]) : a(argc, argv) {
}
DMVCApp::~DMVCApp() {
}

void DMVCModel::addMethod(QString id, const char* responseSlot) {
	server->addMethod(id, this, responseSlot);
}

DMVCModel::DMVCModel() {
	server = new QXmlRpcServer(this);
	server->listen(9528);
}
DMVCModel::~DMVCModel() {
	if (server)
		delete server;
	server = 0;
}

DMVCStubModel::DMVCStubModel() {
	rpc = new QXmlRpcClient(this);
}
DMVCStubModel::~DMVCStubModel() {
	if (rpc)
		delete rpc;
	rpc = 0;
}

void DMVCStubModel::rpc_call(QXmlRpcCall & cp) {
	rpc->call(cp);
}

bool DMVCStubModel::connectToHost() {
	rpc->connectToHost(DMVC::LoadHostName("host.txt"), 9528);
	return rpc->isConnected();
}
bool DMVCStubModel::connectToHost(const QString & host) {
	rpc->connectToHost(host, 9528);
	return rpc->isConnected();
}