#include "QXmlRpcFault.h"
#include "QXmlHelper.h"

QXmlRpcFault::QXmlRpcFault(const QXmlRpcFault &other) : QObject(other.parent()) {
	fault = other.fault;
}

QXmlRpcFault::QXmlRpcFault(int faultCode, QString faultString, QObject *parent) : QObject(parent) {
	fault["faultCode"] = faultCode;
	fault["faultString"] = faultString;
}

QString QXmlRpcFault::toString() {
	QDomDocument doc;
	QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", QString("version=\"1.0\" encoding=\"UTF-8\"" )); 
	doc.appendChild(header);
	QDomElement methodResponse = doc.createElement("methodResponse");
	doc.appendChild(methodResponse);
	QDomElement faultelement = doc.createElement("fault");
	methodResponse.appendChild(faultelement);
	faultelement.appendChild(helper.VariantToXml(fault));
	return doc.toString();
}
