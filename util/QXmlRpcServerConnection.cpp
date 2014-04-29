#include "QXmlRpcServerConnection.h"
#include "QXmlRpcObject.h"
#include "QXmlRpcFault.h"

QXmlRpcServerConnection::QXmlRpcServerConnection(QTcpSocket *connection, QObject* parent) 
	: QObject(parent), clientConnection(connection) {
	connect(clientConnection, SIGNAL(readyRead()), this, SLOT(handelRequest()));
    connect(clientConnection, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

QXmlRpcServerConnection::~QXmlRpcServerConnection() {
	clientConnection->deleteLater();
}

void QXmlRpcServerConnection::handelRequest() {
	QString request = clientConnection->readAll();
	qDebug() << "server receives request: " << request;

	parseCall(request);
}

void QXmlRpcServerConnection::sendResponse(QString content) {
	QByteArray block;
	block.append(content.toUtf8());
	clientConnection->write(block);
	//clientConnection->disconnectFromHost();
}

void QXmlRpcServerConnection::parseCall(QString call) {
	//qDebug() << call;
	QDomDocument doc;
	QList<QVariant> args;
	QVariant ret;
	QString response;
	QObject *responseObject;
	const char *responseSlot;
	
	if(!doc.setContent(call)) { /* recieved invalid xml */
		QXmlRpcFault fault(-32700, "parse error: not well formed");
		sendResponse(fault.toString());
		return;
	}
	
	QDomElement methodNameElement = doc.documentElement().firstChildElement("methodName");
	QDomElement params = doc.documentElement().firstChildElement("params");
	if(methodNameElement.isNull()) { /* invalid call */
		QXmlRpcFault fault(-32600, "server error: invalid xml-rpc. not conforming to spec");
		sendResponse(fault.toString());
		return;
	}
	
	QString methodName = methodNameElement.text();
	
	emit getMethod(methodName, &responseObject, &responseSlot);
	if(!responseObject) { /* unknown method */
		QXmlRpcFault fault(-32601, "server error: requested method not found");
		sendResponse(fault.toString());
		return;
	}
	
	QDomNode paramNode = params.firstChild();
	while(!paramNode.isNull()) {
		args << helper.VariantFromXml( paramNode.firstChild().toElement());
		paramNode = paramNode.nextSibling();
	}
	
	
	if(!invokeMethodWithVariants(responseObject, responseSlot, args, &ret)) { /* error invoking... */
		QXmlRpcFault fault(-32602, "server error: invalid method parameters");
		sendResponse(fault.toString());
		return;
	}
	
	qDebug() << "invoked";

	if(ret.canConvert<QXmlRpcFault>()) {
		response = ret.value<QXmlRpcFault>().toString();
	} else {
		response = helper.VariantToResponse(ret);
	}
	
	qDebug() << "server response:" << response;

	sendResponse(response);
}

/*	taken from http://delta.affinix.com/2006/08/14/invokemethodwithvariants/
	thanks to Justin Karneges once again :) */
bool QXmlRpcServerConnection::invokeMethodWithVariants(QObject *obj,
			const QByteArray &method, const QVariantList &args,
			QVariant *ret, Qt::ConnectionType type) {

	// QMetaObject::invokeMethod() has a 10 argument maximum
	if(args.count() > 10)
		return false;

	QList<QByteArray> argTypes;
	for(int n = 0; n < args.count(); ++n)
		argTypes += args[n].typeName();

	// get return type
	int metatype = 0;
	QByteArray retTypeName = getReturnType(obj->metaObject(), method, argTypes);
	if(!retTypeName.isEmpty()  && retTypeName != "QVariant") {
		metatype = QMetaType::type(retTypeName.data());
		if(metatype == 0) // lookup failed
			return false;
	}

	QGenericArgument arg[10];
	for(int n = 0; n < args.count(); ++n)
		arg[n] = QGenericArgument(args[n].typeName(), args[n].constData());

	QGenericReturnArgument retarg;
	QVariant retval;
	if(metatype != 0) {
		retval = QVariant(metatype, (const void *)0);
		retarg = QGenericReturnArgument(retval.typeName(), retval.data());
	} else { /* QVariant */
		retarg = QGenericReturnArgument("QVariant", &retval);
	}

	if(retTypeName.isEmpty()) { /* void */
		if(!QMetaObject::invokeMethod(obj, method.data(), type,
						arg[0], arg[1], arg[2], arg[3], arg[4],
						arg[5], arg[6], arg[7], arg[8], arg[9]))
			return false;
	} else {
		if(!QMetaObject::invokeMethod(obj, method.data(), type, retarg,
						arg[0], arg[1], arg[2], arg[3], arg[4],
						arg[5], arg[6], arg[7], arg[8], arg[9]))
			return false;
	}

	if(retval.isValid() && ret)
		*ret = retval;
	return true;
}

QByteArray QXmlRpcServerConnection::getReturnType(const QMetaObject *obj,
			const QByteArray &method, const QList<QByteArray> argTypes) {
	for(int n = 0; n < obj->methodCount(); ++n) {
		QMetaMethod m = obj->method(n);
#if QT_VERSION >= 0x050000
		QByteArray sig = m.methodSignature();
#else
		QByteArray sig = m.signature();
#endif
		int offset = sig.indexOf('(');
		if(offset == -1)
			continue;
		QByteArray name = sig.mid(0, offset);
		if(name != method)
			continue;
		if(m.parameterTypes() != argTypes)
			continue;

		return m.typeName();
	}
	return QByteArray();
}
