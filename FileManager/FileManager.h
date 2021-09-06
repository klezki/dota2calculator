#pragma once

#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomElement>
#include <QByteArray>
#include <exception>

class FileManager
{
public:
	static void loadXML(QDomElement& data, const QString xsdPath, const QString xmlPath);
	static void loadXMLasBIN(QDomElement& data, const QString xsdPath, const QString xmlPath);
};
