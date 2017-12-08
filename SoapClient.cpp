#include "SoapClient.h"

CSOAPClient::CSOAPClient(QString sUrl,QString SOAPAction)
{
    m_url = sUrl;
    m_SOAPAction = SOAPAction;
    m_RespFun = NULL;
}

void CSOAPClient::OnNetworkReply(int errCode, const QByteArray &bytes, void *pCusData)
{
    auto pThis = reinterpret_cast<CSOAPClient*>(pCusData);
    if(pThis != NULL)
    {
        pThis->Reply(errCode,bytes);
    }
}


bool CSOAPClient::Request(QString sFunctionName, SOAPReqParma sParmaList, SOAPRespFun fun)
{
    auto content = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n\
            <soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">\r\n\
              <soap:Body>\r\n\
                %1\r\n\
              </soap:Body>\r\n\
            </soap:Envelope>";
    QString sFunctionXML;
    auto xmlWriter =  new QXmlStreamWriter(&sFunctionXML);
    xmlWriter->setAutoFormatting(true);
    xmlWriter->writeStartElement(sFunctionName);
    xmlWriter->writeAttribute("xmlns","http://www.hpnet.com/webservices/");
    if(!sParmaList.empty())
    {
        xmlWriter->writeStartElement("req");
        for (auto i = sParmaList.constBegin(); i != sParmaList.constEnd(); ++i) {
            qDebug() << i.key() << ":" << i.value();
            if(!i.key().isEmpty())
            {
                xmlWriter->writeAttribute(i.key(),i.value());
            }
        }
        xmlWriter->writeAttribute("xmlns","http://www.hpnet.com/WevService");
        xmlWriter->writeEndElement();
    }
    xmlWriter->writeEndElement();
    delete xmlWriter;
    m_content = QString(content).arg(sFunctionXML);
    m_RespFun = fun;
    m_CurrentFunctionName = sFunctionName;
    NetworkHelper::post( m_url, m_content.toUtf8(),m_SOAPAction, OnNetworkReply, this );
}

void CSOAPClient::Reply(int errCode, const QByteArray &bytes)
{
    auto xmlReader =  new QXmlStreamReader(bytes);
    auto bIsPushResult = false;
    SOAPReqParma respResult;
    QStringList sList;
    while(!xmlReader->atEnd() && !xmlReader->hasError())
    {
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        if (xmlReader->isStartElement() )
        {
            if(xmlReader->name() == m_CurrentFunctionName + "Result")
            {
                bIsPushResult = true;
            }
            if(bIsPushResult)
            {
                auto EleName = xmlReader->name().toString();
                sList.push_back(EleName);

                QString sKey = sList.join('.') + ".";
                auto attrs = xmlReader->attributes();
                for(auto i = 0;i < attrs.length();i++)
                {
                    auto attr = attrs[i];
                    if(attr.name() != "xmlns")
                    {
                        respResult[sKey+attr.name().toString()] = attr.value().toString();
                    }
                }
                respResult[sKey + "value"] = xmlReader->readElementText();
            }
        }
        else if(xmlReader->isEndElement())
        {
            if(bIsPushResult && !sList.isEmpty())
            {
                sList.pop_back();
            }
            if(xmlReader->name() == m_CurrentFunctionName+"Result")
            {
                bIsPushResult = false;
                break;
            }
        }
    }
    delete xmlReader;
    xmlReader = NULL;
    if(m_RespFun != NULL)
    {
        m_RespFun(0,m_CurrentFunctionName,respResult);
    }
}
