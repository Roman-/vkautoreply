#ifndef WAITFORSIGNALHELPER_H
#define WAITFORSIGNALHELPER_H

#include <QObject>
#include <QEventLoop>
#include <QTimer>

class WaitForSignalHelper : public QObject
{
    Q_OBJECT
public:
    WaitForSignalHelper( QObject& object, const char* signal );

    // return false if signal wait timed-out
    bool wait(int timeoutMs);

public slots:
    void timeout();

private:
    bool m_bTimeout;
    QEventLoop m_eventLoop;
};

#endif // WAITFORSIGNALHELPER_H
