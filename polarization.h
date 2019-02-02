#ifndef POLARIZATION_H
#define POLARIZATION_H

#include<iostream>
#include<fstream>
#include<QVector>
#include <math.h>


#define PI 3.1415926535898

using namespace std;


class Polarization
{
     double a, b;

public:
     Polarization(const QVector<double>& x, const QVector<double>& y);
     double getY(const double x) const
     {
         return a*x + b;
     }

     void print() const
     {
         cout<<"y = "<<a<<"x + "<<b<<"\n";
     }

     double getAngle() { return 180*atan(a)/PI; }

     double getA() {return a;}
     double getB() {return b;}





};

#endif // POLARIZATION_H
