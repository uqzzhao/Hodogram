#ifndef HODOGRAM_H
#define HODOGRAM_H

#include <QWidget>
#include <csegy.h>
#include <waveformplot.h>
#include <Zoomer.h>
#include <polarization.h>

namespace Ui {
class Hodogram;
}

class Hodogram : public QWidget
{
    Q_OBJECT

public:
    explicit Hodogram(QWidget *parent = 0);
    ~Hodogram();

private:
    Ui::Hodogram *ui;


public:

    void LoadData(); //获取要处理的道数据
    void SetGeophones(int num);
    void PlotTrace();
    void PlotHodogram();
    void CalculateAzimuth();
    void ShowAzimuth();
    void PlotFittedCurve();
    int SetWindowLength();
    void GetHodogramData();

    void PlotTrace(int num);
    void ClearData();
    void ClearTracePlot();
    void ClearHodogram();
    void UpdateTracePlot();



private:
    CSegy m_segy;
    WaveformPlot* m_tracePlot; //地震道波形显示
    WaveformPlot* m_hodogramPlot; //矢端图显示

    Zoomer* m_traceZoomer;
    Zoomer*  m_hodoZoomer;

private:

    int m_Sequence; // 0= z x y; 1= x y z; SEGY文件记录顺序有时不同

    int m_nTotalTraceNum;
    int m_nTraceLen;
    int m_nCurrentGeophone;

    int m_nWindowStart; //时窗开始时间，对应P波初至时刻
    int m_nWindowEnd;

    QVector<double> vectorX;
    QVector<QVector<double>*> m_Traces; //一个SEGY文件中所有道的数据
    double absMax[100];

    QVector<double> m_trace1;
    QVector<double> m_trace2;
    QVector<double> m_trace3;

    QVector<double> m_zHodoData;
    QVector<double> m_xHodoData;
    QVector<double> m_yHodoData;

    Polarization *m_polar;
    double m_azimuthAngle;

private slots:
    void OnBtnOpenClicked();
    void OnBtnPlotClicked();
    void OnBtnClearClicked();
    void OnRadioZXYClicked();
    void OnRadioXYZClicked();
    void OnSpinTraceNoChanged(int num);


};

#endif // HODOGRAM_H
