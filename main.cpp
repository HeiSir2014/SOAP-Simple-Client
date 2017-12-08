#include <QCoreApplication>
#include "SoapClient.h"

void rsp(int errCode, const QString sFunctionName, const SOAPReqParma sRespValue)
{
    qDebug() << "错误信息" << sRespValue["LoginResult.ErrorMsg"];
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CSOAPClient client("http://222.190.145.130:8001/ceshi/webservice1.asmx"
                       ,"http://www.hpnet.com/webservices/Login");
    SOAPReqParma sReqList;
    sReqList["username"] = "123456";
    sReqList["password"] = "123456";
    client.Request("Login",sReqList,rsp);

    return a.exec();
}
