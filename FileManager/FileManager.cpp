#include "FileManager/FileManager.h"


/*static*/ void FileManager::loadXML(QDomElement& data, const QString xsdPath, const QString xmlPath)
{
	try
	{
		QFile xsd(xsdPath);
		if (!xsd.open(QIODevice::ReadOnly))
		{
			throw std::exception();
		}
		QFile xml(xmlPath);
		if (!xml.open(QIODevice::ReadOnly))
		{
			throw std::exception();
		}

		QXmlSchema schema;

		schema.load(&xsd, QUrl::fromLocalFile(xsd.fileName()));

		if (schema.isValid())
		{
			QXmlSchemaValidator validator(schema);
			if (!validator.validate(QUrl::fromLocalFile(xml.fileName())))
				throw std::exception();
		}
		else
		{
			throw std::exception();
		}

		QDomDocument xmlData;
		xmlData.setContent(&xml);

		data = xmlData.documentElement();

		xsd.close();
		xml.close();
	}
	catch (const std::exception& exe)
	{
		//TODO : add something
		throw;
	}
}

/*static*/ void FileManager::loadXMLasBIN(QDomElement& data, const QString xsdPath, const QString xmlPath)
{
	try
	{
		QFile xsdBIN(xsdPath);
		if (!xsdBIN.open(QIODevice::ReadOnly))
		{
			throw std::exception();
		}
		QFile xmlBIN(xmlPath);
		if (!xmlBIN.open(QIODevice::ReadOnly))
		{
			throw std::exception();
		}
		QByteArray xsd = QByteArray::fromBase64(xsdBIN.readAll());
		QByteArray xml = QByteArray::fromBase64(xmlBIN.readAll());


		QXmlSchema schema;

		schema.load(xsd);

		if (schema.isValid())
		{
			QXmlSchemaValidator validator(schema);
			if (!validator.validate(xml))
				throw std::exception();
		}
		else
		{
			throw std::exception();
		}

		QDomDocument xmlData;
		xmlData.setContent(xml);

		data = xmlData.documentElement();

		xsdBIN.close();
		xmlBIN.close();
	}
	catch (const std::exception& exe)
	{
		//TODO : add something
		throw;
	}
}