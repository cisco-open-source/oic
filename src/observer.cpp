#include "Observer.h"

IoTObserver::IoTObserver(IoTObserverCb Cb) :
        m_callback(Cb), m_destroy(false), m_started(false)
{
    m_observerThread[0] = thread(&IoTObserver::observerThread, this);
}

IoTObserver::~IoTObserver()
{
    terminate();
}

void IoTObserver::start()
{
    if (!m_started)
    {
        {
            lock_guard<mutex> lock(m_mutex);
            m_started = true;
        }
        m_cond.notify_one();
    }
}

void IoTObserver::stop()
{
    m_started = false;
}

void IoTObserver::terminate()
{
    m_destroy = true;
    stop();
    m_cond.notify_one();
    m_observerThread[0].join();
}

void IoTObserver::observerThread()
{
    while (!m_destroy)
    {
        unique_lock<mutex> lock(m_mutex);
        if (!m_started)
            m_cond.wait(lock);
        while (m_started)
        {
            m_callback();
        }
    }
}
