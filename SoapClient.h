#ifndef SOAPCLIENT_H
#define SOAPCLIENT_H
#include "NetworkHelper.h"

#include <QDebug>
#include <QHash>
#include <QXmlStreamReader>

#define SOAPReqParma QHash<QString,QString>
typedef void (*SOAPRespFun)(int errCode, const QString sFunctionName, const SOAPReqParma sRespValue);

class CSOAPClient
{
public:
    CSOAPClient(QString sUrl,QString SOAPAction = QString("http://www.hpnet.com/webservices/Login"));

    static void OnNetworkReply( int errCode, const QByteArray& bytes, void* pCusData );
    bool Request(QString sFunctionName, SOAPReqParma sParmaList, SOAPRespFun fun);
    void Reply(int errCode, const QByteArray &bytes);
private:
    QString m_url,m_content,m_SOAPAction,m_CurrentFunctionName;
    SOAPRespFun m_RespFun;
};

#endif // SOAPCLIENT_H
