
#include "general.h"

#include <QString>
#include <QCoreApplication>
#include <QtDebug>

QString humanCount(qint64 n)
{
    QString init = QString("%1").arg(n);
    
    if (init.length() < 4) return init;

    QByteArray buf = init.toLatin1();
    int len = buf.count();
    
    char rbuf[50] = {0};
    int ind = len - 1;
    int x = 0;
    for (int i = len + (len-1)/3 - 1; i >= 0; --i)
    {
        if (++x % 4 == 0) rbuf[i] = ',';
        else rbuf[i] = buf[ind--];
    }
    return rbuf;
}


qint64 powOf10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, Q_INT64_C(1000000000), Q_INT64_C(10000000000), Q_INT64_C(100000000000) };

QString humanCount(double n, int pers)
{
    qint64 dn = (qint64)n;
    qint64 pwr = powOf10[min(pers, 11)];
    qint64 fl = (qint64)((n - dn) * pwr);
    QString sfl = QString("%1").arg(fl).toLatin1();

    return humanCount(dn) + "." + sfl;

}

void flushAllEvents()
{
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents); // process the hide event. otherwise the 3d view is moved and it doesn't look nice.
    QCoreApplication::sendPostedEvents(NULL, 0);
    QCoreApplication::flush();
}


QDebug &operator<<(QDebug &ostr, const Vec3& a)
{
    ostr << "{" << a.v[0] << "," << a.v[1] << "," << a.v[2] << "}";
    return ostr;
}

QStringList parseArgs(const QString& s) 
{
    return s.split(QRegExp("[\\s,()]"), QString::SkipEmptyParts);
}