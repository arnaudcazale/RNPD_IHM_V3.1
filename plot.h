#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>

class Overlay;

class Plot: public QwtPlot
{
    Q_OBJECT

public:
    enum ColorMap
    {
        RGBMap,
        IndexMap,
        HueMap,
        AlphaMap
    };

    Plot( QWidget * = NULL );

    int getNoiseMargin(void);

public Q_SLOTS:
    void changeAlgo( bool on );
    void showSpectrogram( bool on );
    void setColorMap( int );
    void setAlpha( int );
    void setMatrixData( QVector <QVector <double> >* );
    void setResampleMode( int mode );
    void updateScale(int max);
    void cancelNoise( bool on );
    void drawLine(QLine);
    void drawPoint(QPoint);
    void drawZone(QVector <QRect> zones);
    void updateNoiseMargin(int noiseMargin);

public:

#ifndef QT_NO_PRINTER
    void printPlot();
#endif

private:
    QwtPlotSpectrogram *d_spectrogram;

    Overlay *d_overlay;

    int d_mapType;
    int d_alpha;
    int d_mode;
    int d_noise_cancel;
    int d_noise_margin;

    QVector <QVector <double> >* m_matrix_data;
    QVector <QVector <double> >* m_matrix_data_noise;

    const int LGN_NBR = 96;
    const int COL_NBR = 16;

};
