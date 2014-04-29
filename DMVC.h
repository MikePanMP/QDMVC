#ifndef DMVC_H
#define DMVC_H

#include <QtCore>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsSceneEvent>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMainwindow>
#include <QtWidgets/QWidget>

#include "MVC.h"
#include "MVCMacro.h"

#include "DMVCUtil"

namespace DMVC {
	QString LoadHostName(const QString& filename);
}
class DMVCApp {
public:
	DMVCApp(int argc, char *argv[]);
	virtual ~DMVCApp();
public:
	int exec() {
		return a.exec();
	}
private:
	QCoreApplication a;
};

class DMVCModel : public QObject {
	Q_OBJECT
public: // for overwrite
	virtual void registerMethod() {
	}
public: // 
	void addMethod(QString id, const char* responseSlot);
public:
	DMVCModel();
	virtual ~DMVCModel();
private:
	QXmlRpcServer* server;
};

class QXmlRpcClient;
class DMVCStubModel : public MVCModel {
	Q_OBJECT
public:
	DMVCStubModel();
	virtual ~DMVCStubModel();
public:
	void rpc_call(QXmlRpcCall & cp);
	bool connectToHost();
	bool connectToHost(const QString & host);
private:
	QXmlRpcClient* rpc;
};

#endif // DMVC_H