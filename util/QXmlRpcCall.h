#ifndef Q_XML_RPC_CALL_H
#define Q_XML_RPC_CALL_H

#include <QtCore>

class QXmlRpcCall : public QObject {
public:
	QXmlRpcCall(QString _id) : id(_id) {}
public:
	void appendArg(QVariant arg) {
		args.append(arg);
	}
	void clearArgs() {
		args.clear();
	}
	void setReturn(QVariant& _ret) {
		ret = _ret;
	}
public:
	QString getMethod() {
		return id;
	}
	QList<QVariant> getArgs() {
		return args;
	}
	QVariant getReturn() {
		return ret;
	}
private:
	QString id;
	QList<QVariant> args;
	QVariant ret;
private:
	QXmlRpcCall() {}
};

#endif