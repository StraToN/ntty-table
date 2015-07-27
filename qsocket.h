#ifndef QSOCKET_H
#define QSOCKET_H

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>


class QSocket : public QObject
{
    Q_OBJECT
 public:
     explicit QSocket(QObject *parent = 0);

     void doConnect(QString host, int port);
     void sendString(QString msg);
      void sendInt(int msg);

 signals:

 public slots:
     void connected();
     void disconnected();
     void bytesWritten(qint64 bytes);
     void readyRead();

 private:
     QTcpSocket *socket;
     bool   isConnected;
     QString host;
     int port;

};

#endif // QSOCKET_H
