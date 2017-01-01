#include "waitforsignalhelper.h"

WaitForSignalHelper::WaitForSignalHelper( QObject& object, const char* signal ) :
    m_bTimeout( false )
{
    connect(&object, signal, &m_eventLoop, SLOT(quit()));
}

bool WaitForSignalHelper::wait( int timeoutMs )
{
    QTimer timeoutHelper;
    if ( timeoutMs != 0 ) // manage timeout
    {
        timeoutHelper.setInterval( timeoutMs );
        timeoutHelper.start();
        connect(&timeoutHelper, SIGNAL(timeout()), this, SLOT(timeout()));
    }
    // else, wait forever

    m_bTimeout = false;

    m_eventLoop.exec();

    return !m_bTimeout;
}

void WaitForSignalHelper::timeout()
{
    m_bTimeout = true;
    m_eventLoop.quit();
}
