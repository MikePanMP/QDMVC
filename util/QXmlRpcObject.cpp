#include "QXmlRpcObject.h"


QXmlRpcClientObject::QXmlRpcClientObject(QObject* parent) : QObject(parent) {
	QDomImplementation::setInvalidDataPolicy(QDomImplementation::DropInvalidChars);
}

QString QXmlRpcClientObject::prepareCall(QString method, QList<QVariant> args) {

	QDomDocument doc;

	QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", QString("version=\"1.0\" encoding=\"UTF-8\"" ));
	doc.appendChild(header);
	
	QDomElement methodCall = doc.createElement("methodCall");
	QDomElement methodName = doc.createElement("methodName");
	QDomElement params = doc.createElement("params");
	QDomElement param;

	doc.appendChild(methodCall);
	methodCall.appendChild(methodName);
	methodName.appendChild(doc.createTextNode(method));

	methodCall.appendChild(params);

	for(int i = 0; i < args.size(); ++i) {
		param = doc.createElement("param");
		param.appendChild(helper.VariantToXml(args.at(i)));
		params.appendChild(param);
	}

	return doc.toString();
}