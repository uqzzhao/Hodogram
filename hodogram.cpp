#include "hodogram.h"
#include "ui_hodogram.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include "util.h"
#include <QFileDialog>
#include "Zoomer.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qpalette.h>
#include <qmessagebox.h>

Hodogram::Hodogram(QWidget *parent) :
    QWidget(parent),
    m_Sequence(0),
    ui(new Ui::Hodogram)
{
    ui->setupUi(this);

    ui->btnClear->setEnabled(false);
    ui->btnPlot->setEnabled(false);
    ui->edtEnd->setEnabled(false);
    ui->edtStart->setEnabled(false);
    ui->spinBox->setEnabled(false);

    QIntValidator *intValidator=new QIntValidator();
    intValidator->setRange(0,100000);
    intValidator->setTop(100000);
    ui->edtEnd->setValidator(intValidator);
    ui->edtStart->setValidator(intValidator);



    m_tracePlot = new WaveformPlot (this);
    m_tracePlot->setTitle(tr("3-Component Waveform Display"));



    m_tracePlot->enableAxis(QwtPlot::yLeft);
    m_tracePlot->enableAxis(QwtPlot::yRight);
    m_tracePlot->enableAxis(QwtPlot::xTop);
    m_tracePlot->enableAxis(QwtPlot::xBottom);
    m_tracePlot->setAxisScale( m_tracePlot->yLeft,0,4);
    m_tracePlot->setAxisScale( m_tracePlot->yRight,0,4);
    m_tracePlot->setAxisScale( m_tracePlot->xBottom,0,100);
    m_tracePlot->setAxisScale( m_tracePlot->xTop,0,100);



    ui->TracePLotArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->TracePLotArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->TracePLotArea->setWidget(m_tracePlot);
    m_tracePlot->setMaximumHeight(280);
    m_tracePlot->setMinimumHeight(280);
    m_tracePlot->setMaximumWidth(800);
    m_tracePlot->setMinimumWidth(800);



    m_hodogramPlot = new WaveformPlot (this);
    m_hodogramPlot->setTitle(tr("Hodogram"));

    m_hodogramPlot->enableAxis(QwtPlot::yLeft);
    m_hodogramPlot->enableAxis(QwtPlot::yRight);
    m_hodogramPlot->enableAxis(QwtPlot::xTop);
    m_hodogramPlot->enableAxis(QwtPlot::xBottom);

    m_hodogramPlot->setAxisTitle( m_tracePlot->yLeft, tr("H2"));
    m_hodogramPlot->setAxisTitle( m_tracePlot->xBottom, tr("H1"));

    m_hodogramPlot->setAxisScale( m_tracePlot->yLeft,-1.0, 1.0);
    m_hodogramPlot->setAxisScale( m_tracePlot->xBottom,-1.0, 1.0);
    m_hodogramPlot->setAxisScale( m_tracePlot->yRight,-1.0, 1.0);
    m_hodogramPlot->setAxisScale( m_tracePlot->xTop,-1.0, 1.0);



    ui->HodogramArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->HodogramArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->HodogramArea->setWidget(m_hodogramPlot);
    m_hodogramPlot->setMaximumHeight(280);
    m_hodogramPlot->setMinimumHeight(280);
    m_hodogramPlot->setMaximumWidth(300);
    m_hodogramPlot->setMinimumWidth(300);


    connect(ui->btnOpen,SIGNAL(clicked(bool)),this, SLOT(OnBtnOpenClicked()));
    connect(ui->btnPlot, SIGNAL(clicked(bool)), this, SLOT(OnBtnPlotClicked()));
    connect(ui->btnClear,SIGNAL(clicked(bool)),this, SLOT(OnBtnClearClicked()));
    connect(ui->radioXYZ, SIGNAL(clicked(bool)), this, SLOT(OnRadioXYZClicked()));
    connect(ui->radioZXY, SIGNAL(clicked(bool)), this, SLOT(OnRadioZXYClicked()));
    connect(ui->spinBox,SIGNAL(valueChanged(int)) ,this, SLOT(OnSpinTraceNoChanged(int)));


    m_nTotalTraceNum=0;
    m_nTraceLen=0;

    m_nCurrentGeophone=1;

    m_nWindowStart=0; //时窗开始时间，对应P波初至时刻
    m_nWindowEnd=20;






}

Hodogram::~Hodogram()
{
    delete ui;
}

void Hodogram::PlotTrace()
{


    QwtPlotCurve* traceCurve1 = new QwtPlotCurve();
    QwtPlotCurve* traceCurve2 = new QwtPlotCurve();
    QwtPlotCurve* traceCurve3 = new QwtPlotCurve();


    traceCurve1->attach( m_tracePlot);//设置曲线
    traceCurve2->attach( m_tracePlot);//设置曲线
    traceCurve3->attach( m_tracePlot);//设置曲线


    //加入网格
    QwtPlotGrid *grid=new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setMajorPen(QPen(Qt::gray,0,Qt::DotLine));
    grid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));
    grid->attach( m_tracePlot);

    traceCurve1->setRenderHint(QwtPlotItem::RenderAntialiased);
    traceCurve1->setPen(QPen(Qt::blue));
    traceCurve2->setRenderHint(QwtPlotItem::RenderAntialiased);
    traceCurve2->setPen(QPen(Qt::red));
    traceCurve3->setRenderHint(QwtPlotItem::RenderAntialiased);
    traceCurve3->setPen(QPen(Qt::darkGreen));

    //设置放大缩小zoomer功能
    QwtPlotCanvas* traceCanvas = new QwtPlotCanvas();
    m_tracePlot->setCanvas(traceCanvas);

    QRectF rect(0,0,m_nTraceLen,4);

    m_traceZoomer = new Zoomer(traceCanvas,rect);
    m_traceZoomer->setRect(rect);
    m_traceZoomer->setZoomBase(rect);
    m_traceZoomer->setMousePattern(QwtEventPattern::MouseSelect2,
                                   Qt::RightButton,Qt::ControlModifier);
    m_traceZoomer->setMousePattern(QwtEventPattern::MouseSelect3,
                                   Qt::RightButton);
    m_traceZoomer->setRubberBandPen(QPen(Qt::red, 2, Qt::DotLine));
    m_traceZoomer->setTrackerMode(QwtPicker::AlwaysOff);
    m_traceZoomer->setEnabled(true);

    m_tracePlot->setAxisScale( m_tracePlot->xBottom, 0, double(m_nTraceLen));
    m_tracePlot->setAxisScale( m_tracePlot->xTop, 0, double(m_nTraceLen));


    switch(m_Sequence)
    {
    case 0:
    {

        traceCurve1->setSamples(vectorX,m_trace1);
        traceCurve1->setTitle(tr("Z Component"));

        traceCurve2->setSamples(vectorX,m_trace2);
        traceCurve2->setTitle(tr("X Component"));

        traceCurve3->setSamples(vectorX,m_trace3);
        traceCurve3->setTitle(tr("Y Component"));
        m_tracePlot->replot();
        break;
    }
    case 1:
    {
        traceCurve1->setSamples(vectorX,m_trace2);
        traceCurve1->setTitle(tr("X Component"));

        traceCurve2->setSamples(vectorX,m_trace3);
        traceCurve2->setTitle(tr("Y Component"));

        traceCurve3->setSamples(vectorX,m_trace1);
        traceCurve3->setTitle(tr("Z Component"));
        m_tracePlot->replot();
        break;
    }
    }



}

void Hodogram::PlotHodogram()
{

    QwtPlotCurve* hodogramCurve = new QwtPlotCurve();


    hodogramCurve->attach(m_hodogramPlot);//设置曲线
    hodogramCurve->setSamples(m_xHodoData,m_yHodoData);


    hodogramCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    hodogramCurve->setPen(QPen(Qt::red));


    m_hodogramPlot->replot();
}

void Hodogram::CalculateAzimuth()
{

    m_polar = new Polarization(m_xHodoData,m_yHodoData);
    double angle = m_polar->getAngle();

    if (angle>0 || angle ==0)
    {
        m_azimuthAngle = angle;
    }
    else
    {
        m_azimuthAngle = 180 + angle;
    }

}

void Hodogram::ShowAzimuth()
{
    ui->edtAzimuth->setText(QString("%1").arg(m_azimuthAngle));
}

void Hodogram::PlotFittedCurve()
{
    double a, b, j,k;
    QVector<double> fittedX, fittedY;
    a = m_polar->getA();
    b = m_polar->getB();
    j=0.1;
    k=-0.5;
    for (int i=0; i<10; i++)
    {
        fittedX<<k;
        fittedY<<a*k+b;
        k+=j;
    }

    QwtPlotCurve* fittedCurve = new QwtPlotCurve();


    fittedCurve->attach(m_hodogramPlot);//设置曲线
    fittedCurve->setSamples(fittedX,fittedY);


    fittedCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
    fittedCurve->setPen(QPen(Qt::black));


    //加入十字标记线
    QwtPlotMarker* hodoMarker;
    hodoMarker = new QwtPlotMarker();
    hodoMarker->setLineStyle(QwtPlotMarker::Cross);
    hodoMarker->setLinePen(QPen(Qt::black, 0, Qt::DotLine));
    hodoMarker->setYValue(0.0);
    hodoMarker->setXValue(0.0);
    hodoMarker->attach(m_hodogramPlot);


    //设置背景色为白色
    QwtPlotCanvas* hodogramCanvas = new QwtPlotCanvas();
    QPalette hodoPalette(Qt::white);
    hodogramCanvas->setPalette(hodoPalette);
    m_hodogramPlot->setCanvas(hodogramCanvas);




    //设置放大缩小zoomer功能
    QRectF rect(0,0,2,2);

    m_hodoZoomer = new Zoomer(hodogramCanvas,rect);
    m_hodoZoomer->setRect(rect);
    m_hodoZoomer->setZoomBase(rect);
    m_hodoZoomer->setMousePattern(QwtEventPattern::MouseSelect2,
                                  Qt::RightButton,Qt::ControlModifier);
    m_hodoZoomer->setMousePattern(QwtEventPattern::MouseSelect3,
                                  Qt::RightButton);
    m_hodoZoomer->setRubberBandPen(QPen(Qt::red, 2, Qt::DotLine));
    m_hodoZoomer->setTrackerMode(QwtPicker::AlwaysOff);
    m_hodoZoomer->setEnabled(true);



    m_hodogramPlot->replot();
}

int Hodogram::SetWindowLength()
{
    QString input1 = ui->edtStart->text();
    QString input2 = ui->edtStart->text();
    if ( input1.isEmpty() || input2.isEmpty() )
    {
        QMessageBox::warning(this, "Warning", "Incorrect window parameters!\nPlease input window start and end.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        return 0;
    }
    else
    {
        m_nWindowStart=ui->edtStart->text().toInt();
        m_nWindowEnd = ui->edtEnd->text().toInt();

        if (m_nWindowStart >= m_nWindowEnd)
        {
            QMessageBox::warning(this, "Warning", "Incorrect window parameters!\nWindow end should be larger than window start.", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            return 0;

        }
        else
        {
            return 1;
        }
    }



}

void Hodogram::GetHodogramData()
{

    for (int i= m_nWindowStart; i<m_nWindowEnd+1; i++)
    {
        switch(m_Sequence)
        {
        case 0:
        {
            m_zHodoData.append((*m_Traces[m_nCurrentGeophone*3-3])[i-1]);
            m_xHodoData.append((*m_Traces[m_nCurrentGeophone*3-2])[i-1]);
            m_yHodoData.append((*m_Traces[m_nCurrentGeophone*3-1])[i-1]);
            break;
        }
        case 1:
        {
            m_xHodoData.append((*m_Traces[m_nCurrentGeophone*3-3])[i-1]);
            m_yHodoData.append((*m_Traces[m_nCurrentGeophone*3-2])[i-1]);
            m_zHodoData.append((*m_Traces[m_nCurrentGeophone*3-1])[i-1]);
            break;
        }

        }


    }
}

void Hodogram::ClearData()
{

    if (!vectorX.isEmpty())
    {
        vectorX.clear();
    }


    int i,j;
    if (!m_Traces.isEmpty())
    {
        i=m_Traces.count();
        for(j=i-1;j>=0;j--)
            m_Traces[j]->clear();
        m_Traces.clear();

    }

}


void Hodogram::ClearTracePlot()
{

    if (!m_trace1.isEmpty())
    {
        m_trace1.clear();
    }
    if (!m_trace2.isEmpty())
    {
        m_trace2.clear();
    }
    if (!m_trace3.isEmpty())
    {
        m_trace3.clear();
    }

    m_tracePlot->detachItems();
    m_tracePlot->replot();

}

void Hodogram::ClearHodogram()
{

    if (!m_zHodoData.isEmpty())
    {
        m_zHodoData.clear();
    }
    if (!m_yHodoData.isEmpty())
    {
        m_yHodoData.clear();
    }
    if (!m_xHodoData.isEmpty())
    {
        m_xHodoData.clear();
    }


    m_hodogramPlot->detachItems();
    m_hodogramPlot->replot();


}

void Hodogram::UpdateTracePlot()
{

    ClearTracePlot();
    SetGeophones(m_nCurrentGeophone);
    PlotTrace();

}

void Hodogram::LoadData()
{
    m_nTotalTraceNum=m_segy.getTotalTraceNumber();
    m_nTraceLen = m_segy.getSamplesNumber();

    QVector<double>* m_ChangeData;
    double sampleValue= 0.0;
    QVector <double> tempVector;
    float* pTrace[m_nTotalTraceNum];

    float j= 0.0;

    for (int i=0; i<m_nTraceLen; i++)
    {
        vectorX<<j;
        j+=1.0;

    }


    for (int i=0; i<m_nTotalTraceNum; i++)
    {

        m_ChangeData=new QVector<double>;
        m_ChangeData->clear();
        pTrace[i] = m_segy.GetTraceData(i+1);


        tempVector.clear();

        for (int j=0; j<m_nTraceLen; j++)
        {
            m_ChangeData->append(sampleValue);


        }


        for (int k=0; k<m_nTraceLen; k++)
        {
            tempVector<<(double)(pTrace[i])[k];

        }
        //地震道单道归一化，每个采样点振幅值除以该道最大振幅值的绝对值
        double absMin=0;
        GetVectorMax(tempVector, absMax[i]);

        for (int k=0; k<m_nTraceLen; k++)
        {

            tempVector[k] = ( tempVector[k]-absMin)/(absMax[i]-absMin);

            m_ChangeData->replace (k, tempVector[k]);


        }

        m_Traces<<m_ChangeData;


    }
}

void Hodogram::SetGeophones(int num)
{
    double temp1 = 0.0;
    double temp2 = 0.0;
    double temp3 = 0.0;
    for (int i=0; i<m_nTraceLen; i++)
    {
        temp1= (*m_Traces[num*3-3])[i];
        temp1 =1.0*0.5*temp1;
        temp1 =1.0+temp1;
        m_trace1<<temp1;

        temp2= (*m_Traces[num*3-2])[i];
        temp2=1.0*0.5*temp2;  //显示波形的幅度
        temp2 =2.0+temp2;
        m_trace2<<temp2;

        temp3= (*m_Traces[num*3-1])[i];
        temp3=1.0*0.5*temp3;  //显示波形的幅度
        temp3 =3.0+temp3;
        m_trace3<<temp3;

    }
}

void Hodogram::OnBtnOpenClicked()
{
    ClearData();
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Select one SEG-Y file"),
                tr(""),
                tr("SEG-Y Files(*.sgy *.segy)")
                );

    if ( fileName .length()>0 )
    {
        ClearData();
        char szFilePath[2000];
        memset(szFilePath,0,2000);
        QStringData *data = fileName.data_ptr();
        ushort *s = data->data();

        for (int i = 0; i<data->size; i++ )
        {
            szFilePath[i] = s[i];
        }


        m_segy.OpenFile(szFilePath);
        ui->edtEnd->setEnabled(true);
        ui->edtStart->setEnabled(true);

        ui->edtFileName->setText(fileName);
        //设置在窗口底部显示采样率
        float sampleRate = float(m_segy.getSamplesInterval())/1000;
        QString strSampleRate;
        strSampleRate.sprintf("%.2f", sampleRate);
        ui->edtSampleRate->setText(QString("%1").arg(strSampleRate));
        //显示总道数
        ui->edtTotalTraces->setText((QString("%1").arg(m_segy.getTotalTraceNumber())));
        LoadData();
        SetGeophones(m_nCurrentGeophone);
        PlotTrace();

    }
}

void Hodogram::OnBtnPlotClicked()
{
    ui->btnClear->setEnabled(true);
    if (SetWindowLength() == 1)
    {
        GetHodogramData();
        PlotHodogram();
        CalculateAzimuth();
        ShowAzimuth();
        PlotFittedCurve();
    }

}

void Hodogram::OnBtnClearClicked()
{
    ClearHodogram();

}

void Hodogram::OnRadioZXYClicked()
{
    m_Sequence =0;
    ui->spinBox->setEnabled(true);
    ui->btnPlot->setEnabled(true);
}

void Hodogram::OnRadioXYZClicked()
{
    m_Sequence =1;
    ui->spinBox->setEnabled(true);
    ui->btnPlot->setEnabled(true);
}

void Hodogram::OnSpinTraceNoChanged(int num)
{
    m_nCurrentGeophone = num;

    UpdateTracePlot();
}
