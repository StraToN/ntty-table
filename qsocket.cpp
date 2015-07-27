#include "qsocket.h"

QSocket::QSocket(QObject *parent) :
    QObject(parent)
{
}

void QSocket::doConnect(QString host, int port)
{
    this->host = host;
    this->port = port;
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(readyRead()));

    qDebug() << "connecting...";

    // this is not blocking call
    socket->connectToHost(host, port);

    // we need to wait...
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void QSocket::sendString(QString msg){

    QByteArray array;
    array.append(msg);
    //qDebug()<<msg;
    this->socket->write(array);


}
void QSocket::sendInt(int msg){

     //qDebug()<<msg;
     this->socket->write((const char*) &msg, sizeof(msg));

}


void QSocket::connected()
{
    qDebug() << "connected...";
}

void QSocket::disconnected()
{
    qDebug() << "disconnected...";

    // Si on se fait deconnecter, on reconnecte.
    //doConnect(host, port);
}

void QSocket::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void QSocket::readyRead()
{
    qDebug() << "reading...";

    // read the data from the socket
    qDebug() << socket->readAll();
}


