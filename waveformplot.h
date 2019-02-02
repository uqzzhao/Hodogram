#ifndef WaveformPlot_H
#define WaveformPlot_H

#include <qwt_plot.h>


class WaveformPlot : public QwtPlot
{
	Q_OBJECT
public:
    WaveformPlot( QWidget * = NULL );

protected:

    virtual void mousePressEvent ( QMouseEvent * );
Q_SIGNALS:
    void plotpick(QPointF);

};

#endif // WaveformPlot_H
