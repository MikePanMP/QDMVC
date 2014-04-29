#include "QXmlHelper.h"


QDomElement QXmlHelper::VariantToXml(QVariant arg) {
	
	//dummy document
	QDomDocument doc;
	//value element, we need this in each case
	QDomElement tagValue = doc.createElement("value");

	switch(arg.type()) {
	case QVariant::String: {

		QDomElement tagString = doc.createElement("string"); 
		QDomText textString = doc.createTextNode(arg.toString());
		
		tagValue.appendChild(tagString);
		tagString.appendChild(textString);

		return tagValue;

	} case QVariant::Int: {

		QDomElement tagInt = doc.createElement("int"); 
		QDomText textInt = doc.createTextNode(QString::number(arg.toInt()));
		
		tagValue.appendChild(tagInt);
		tagInt.appendChild(textInt);

		return tagValue;

	} case QVariant::Double: {

		QDomElement tagDouble = doc.createElement("double"); 
		QDomText textDouble = doc.createTextNode(QString::number(arg.toDouble()));
		
		tagValue.appendChild(tagDouble);
		tagDouble.appendChild(textDouble);

		return tagValue;

	} case QVariant::Bool: {
	
		QString textValue = arg.toBool() ? "1" : "0";

		QDomElement tag = doc.createElement("boolean"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::ByteArray: {

		QString textValue = arg.toByteArray().toBase64();

		QDomElement tag = doc.createElement("base64"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::DateTime: {
	
		QString textValue = arg.toDateTime().toString("yyyyMMddThh:mm:ss");

		QDomElement tag = doc.createElement("datetime.iso8601"); 
		QDomText text = doc.createTextNode(textValue);
		
		tagValue.appendChild(tag);
		tag.appendChild(text);

		return tagValue;

	} case QVariant::List: {

		QDomElement tagArray = doc.createElement("array");
		QDomElement tagData = doc.createElement("data");
		tagArray.appendChild(tagData);
		tagValue.appendChild(tagArray);

		const QList<QVariant> args = arg.toList();
		for(int i = 0; i < args.size(); ++i) {
			tagData.appendChild(VariantToXml(args.at(i)));
		}
	
		return tagValue;

	} case QVariant::Map: {

		QDomElement tagStruct = doc.createElement("struct");
		QDomElement member;
		QDomElement name;

		tagValue.appendChild(tagStruct);

		QMap<QString, QVariant> map = arg.toMap();
		QMapIterator<QString, QVariant> i(map);
		while(i.hasNext()) {
			i.next();

			member = doc.createElement("member");
			name = doc.createElement("name");

			// (key) -> name -> member -> struct
			tagStruct.appendChild(member);
			member.appendChild(name);
			name.appendChild(doc.createTextNode(i.key()));

			// add variables by recursion
			member.appendChild(VariantToXml(i.value()));
		}

		return tagValue;

	} default:
		qDebug() << "Failed to marshal unknown variant type: " << arg.type() << endl;
	}
	return QDomElement(); //QString::null;
}


QVariant QXmlHelper::VariantFromXml(const QDomElement &elem) {
	if(elem.tagName().toLower() != "value") {
		return QVariant();
	}
	
	// If no type is indicated, the type is string.
	if(!elem.firstChild().isElement()) {
		return QVariant(elem.text());
	}
	
	const QDomElement typeElement = elem.firstChild().toElement();	
	const QString typeName = typeElement.tagName().toLower();

	if(typeName == "string")
		return QVariant(typeElement.text());
	else if(typeName == "i4" || typeName == "int")
		return QVariant(typeElement.text().toInt());
	else if(typeName == "double")
		return QVariant(typeElement.text().toDouble());
	else if (typeName == "boolean") {
		if(typeElement.text().toLower() == "true" || typeElement.text() == "1")
			return QVariant(true);
		else
			return QVariant(false);
	} else if(typeName == "base64")
		return QVariant(QByteArray::fromBase64( typeElement.text().toLatin1()));
	else if(typeName == "datetime" || typeName == "datetime.iso8601")
		return QVariant(QDateTime::fromString(typeElement.text(), "yyyyMMddThh:mm:ss"));
	else if(typeName == "nil") // Non-standard extension: http://ontosys.com/xml-rpc/extensions.php
		return QVariant();
	else if ( typeName == "array" ) {
		QList<QVariant> values;
		QDomNode valueNode = typeElement.firstChild().firstChild();
		while(!valueNode.isNull()) {
			values << VariantFromXml(valueNode.toElement());
			valueNode = valueNode.nextSibling();
		}
		return QVariant(values);
	}
	else if ( typeName == "struct" ) {
		QMap<QString, QVariant> map;
		QDomNode memberNode = typeElement.firstChild();
		while(!memberNode.isNull())	{
			const QString key = memberNode.toElement().elementsByTagName("name").item(0).toElement().text();
			const QVariant data = VariantFromXml(memberNode.toElement().elementsByTagName("value").item(0).toElement());
			map[key] = data;
			memberNode = memberNode.nextSibling();
		}
		return QVariant(map);
	} else {
		qDebug() << "Cannot demarshal unknown type " << typeElement.tagName().toLower();
	}
	return QVariant();
}


QString QXmlHelper::VariantToResponse(QVariant arg) {

	QDomDocument doc;

	QDomProcessingInstruction header = doc.createProcessingInstruction( "xml", QString("version=\"1.0\" encoding=\"UTF-8\"" )); 
	doc.appendChild(header);
	
	QDomElement methodResponse = doc.createElement("methodResponse");
	QDomElement params = doc.createElement("params");
	QDomElement param;

	doc.appendChild(methodResponse);

	methodResponse.appendChild(params);

	if(!arg.isNull()) {
		param = doc.createElement("param");
		param.appendChild(VariantToXml(arg));
		params.appendChild(param);
	}
	return doc.toString();
}


QVariant QXmlHelper::ResponseToVariant(QString response) {
	QDomDocument doc;
	QVariant arg;
	QString errorMsg;
	int errorLine;
	int errorColumn;
	if(!doc.setContent(response, &errorMsg, &errorLine, &errorColumn)) {
		return QVariant(FaultToVariant(-32700, QString("parse error: response not well formed at line %1: %2").arg(errorLine).arg(errorMsg)));
	}
	if(doc.documentElement().firstChild().toElement().tagName().toLower() == "params") {
		QDomNode paramNode = doc.documentElement().firstChild().firstChild();
		if(!paramNode.isNull()) {
			arg = VariantFromXml( paramNode.firstChild().toElement() );
		}
		return arg;
	} else if(doc.documentElement().firstChild().toElement().tagName().toLower() == "fault") {
		const QVariant errorVariant = VariantFromXml(doc.documentElement().firstChild().firstChild().toElement());
		return QVariant(FaultToVariant(errorVariant.toMap() [ "faultCode" ].toInt(),
		           errorVariant.toMap() [ "faultString" ].toString()));
	} else {
		return QVariant(FaultToVariant(-32600,
		           tr("parse error: invalid xml-rpc. not conforming to spec.")));
	}
}

QString QXmlHelper::FaultToVariant(int faultCode, const QString & faultString) {
	return QString("Fault Code: %1, Error Message: %2").arg(faultCode).arg(faultString);
}