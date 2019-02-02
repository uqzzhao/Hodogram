#include "Polarization.h"

Polarization::Polarization(const QVector<double> &x, const QVector<double> &y)
{
    double t1=0, t2=0, t3=0, t4=0;
    for(int i=0; i<x.size(); ++i)
    {
        t1 += x[i]*x[i];
        t2 += x[i];
        t3 += x[i]*y[i];
        t4 += y[i];
    }
    a = (t3*x.size() - t2*t4) / (t1*x.size() - t2*t2);
    //b = (t4 - a*t2) / x.size();
    b = (t1*t4 - t2*t3) / (t1*x.size() - t2*t2);

}

