#include "waveformplot.h"
#include <qevent.h>
#include "qwt_scale_widget.h"

WaveformPlot::WaveformPlot(QWidget* widget)
	:QwtPlot(widget)
{
}

void WaveformPlot::mousePressEvent( QMouseEvent *e )
{
	QPoint pt = e->pos();
	QPointF fpt;

	fpt.setX(invTransform(QwtPlot::xBottom,pt.x()-axisWidget(QwtPlot::yLeft)->width()));
	fpt.setY(invTransform(QwtPlot::yLeft,pt.y()));
    emit plotpick(fpt);
}
