#include "popupwindow.h"
#include <QLabel>
#include <QObject>
#include <QVBoxLayout>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <QtDebug>


PopupWindow::PopupWindow(QWidget *parent) :
    QMainWindow(parent)
{
    d_plot_left = new Plot( this );
    d_plot_right = new Plot( this );
    d_noiseMargin = 10;

    QGridLayout *glayout = new QGridLayout();
    glayout->addWidget(d_plot_left, 0,0,1,1);
    glayout->addWidget(d_plot_right, 0,1,1,1);
    //glayout->addWidget(d_overlay_left, 0,0,1,1);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout( glayout );

    setCentralWidget( window );

    QToolBar *toolBar = new QToolBar( this );

#ifndef QT_NO_PRINTER
    QToolButton *btnPrint = new QToolButton( toolBar );
    btnPrint->setText( "Print" );
    btnPrint->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
    toolBar->addWidget( btnPrint );
    /*connect( btnPrint, SIGNAL( clicked() ),
        d_plot_left, SLOT( printPlot() ) );
    connect( btnPrint, SIGNAL( clicked() ),
        d_plot_right, SLOT( printPlot() ) );*/

    toolBar->addSeparator();
#endif

    toolBar->addWidget( new QLabel("Color Map " ) );
    QComboBox *mapBox = new QComboBox( toolBar );
    mapBox->addItem( "RGB" );
    //mapBox->addItem( "Indexed Colors" );
    //mapBox->addItem( "Hue" );
    mapBox->addItem( "Alpha" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    toolBar->addWidget( mapBox );
    connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             d_plot_left, SLOT( setColorMap( int ) ) );
    connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             d_plot_right, SLOT( setColorMap( int ) ) );

    toolBar->addWidget( new QLabel( " Opacity " ) );
    QSlider *slider = new QSlider( Qt::Horizontal );
    slider->setRange( 0, 255 );
    slider->setValue( 255 );
    connect( slider, SIGNAL( valueChanged( int ) ),
        d_plot_left, SLOT( setAlpha( int ) ) );
    connect( slider, SIGNAL( valueChanged( int ) ),
        d_plot_right, SLOT( setAlpha( int ) ) );

    toolBar->addWidget( slider );
    toolBar->addWidget( new QLabel("   " ) );

    QCheckBox *btnSpectrogram = new QCheckBox( "Spectrogram", toolBar );
    toolBar->addWidget( btnSpectrogram );
    connect( btnSpectrogram, SIGNAL( toggled( bool ) ),
        d_plot_left, SLOT( showSpectrogram( bool ) ) );
    connect( btnSpectrogram, SIGNAL( toggled( bool ) ),
        d_plot_right, SLOT( showSpectrogram( bool ) ) );

    QCheckBox *btnAlgo = new QCheckBox( "Algo", toolBar );
    toolBar->addWidget( btnAlgo );
    connect( btnAlgo, SIGNAL( toggled( bool ) ),
        d_plot_left, SLOT( changeAlgo( bool ) ) );
    connect( btnAlgo, SIGNAL( toggled( bool ) ),
        d_plot_right, SLOT( changeAlgo( bool ) ) );

    QCheckBox *btnNoise = new QCheckBox( "Noise", toolBar );
    toolBar->addWidget( btnNoise );
    connect( btnNoise, SIGNAL( toggled( bool ) ),
        d_plot_left, SLOT( cancelNoise( bool ) ) );
    connect( btnNoise, SIGNAL( toggled( bool ) ),
        d_plot_right, SLOT( cancelNoise( bool ) ) );

    QLabel *integerLabel = new QLabel(tr("noise margin"));
       QSpinBox *integerSpinBox = new QSpinBox;
       integerSpinBox->setRange(0, 50);
       integerSpinBox->setSingleStep(10);
       integerSpinBox->setValue(20);
        toolBar->addWidget( integerSpinBox );
    toolBar->addWidget( integerLabel );
        connect( integerSpinBox, SIGNAL( valueChanged( int ) ),
            d_plot_left, SLOT( updateNoiseMargin( int ) ) );
        connect( integerSpinBox, SIGNAL( valueChanged( int ) ),
            d_plot_right, SLOT( updateNoiseMargin( int ) ) );

    addToolBar( toolBar );

    btnSpectrogram->setChecked( true );
    btnAlgo->setChecked( false );
}

PopupWindow::~PopupWindow()
{

}

void PopupWindow::dataUpdate_left(QVector<QVector <double> > *dataPacket)
{
    d_plot_left->setMatrixData(dataPacket);
}

void PopupWindow::dataUpdate_right(QVector<QVector <double> > *dataPacket)
{
    d_plot_right->setMatrixData(dataPacket);
}

QLine PopupWindow::convertLine2pixel(QLine line)
{
    const QwtScaleMap xMap = d_plot_left->canvasMap( QwtPlot::xBottom );
    const QwtScaleMap yMap = d_plot_left->canvasMap( QwtPlot::yLeft );

    int x1 = xMap.transform( line.x1() + 0.5 );
    int y1 = yMap.transform( line.y1() );
    int x2 = xMap.transform( line.x2() + 0.5 );
    int y2 = yMap.transform( line.y2() );

    QLine line_pixels(x1, y1, x2, y2);
    return line_pixels;

}

QPoint PopupWindow::convertPoint2pixel(QPoint point)
{
    const QwtScaleMap xMap = d_plot_left->canvasMap( QwtPlot::xBottom );
    const QwtScaleMap yMap = d_plot_left->canvasMap( QwtPlot::yLeft );

    int x1 = xMap.transform( point.x() + 0.5 );
    int y1 = yMap.transform( point.y() + 0.5 );

    QPoint point_pixels(x1, y1);
    return point_pixels;
}

QRect PopupWindow::convertZone2pixel(QRect zone)
{
    const QwtScaleMap xMap = d_plot_left->canvasMap( QwtPlot::xBottom );
    const QwtScaleMap yMap = d_plot_left->canvasMap( QwtPlot::yLeft );

    const int left = xMap.transform(zone.left());
    const int top = yMap.transform(zone.top());
    const int right = xMap.transform(zone.right());
    const int bottom = yMap.transform(zone.bottom());

    int width = right - left;
    int height = top-bottom;

    QRect zone_pixel(left, top, width, height);
    return zone_pixel;
}

void PopupWindow::drawLine(QLine lineLeft, QLine lineRight)
{
    QLine line_left_pix;
    QLine line_right_pix;

    line_left_pix = convertLine2pixel(lineLeft);
    line_right_pix = convertLine2pixel(lineRight);

    d_plot_left->drawLine(line_left_pix);
    d_plot_right->drawLine(line_right_pix);
}

void PopupWindow::drawPoint(QPoint pointLeft, QPoint pointRight)
{
    QPoint point_left_pix;
    QPoint point_right_pix;

    point_left_pix = convertPoint2pixel(pointLeft);
    point_right_pix = convertPoint2pixel(pointRight);

    d_plot_left->drawPoint(point_left_pix);
    d_plot_right->drawPoint(point_right_pix);
}

void PopupWindow::drawZone(QVector <QRect> zoneLeft, QVector <QRect> zoneRight)
{
    QVector<QRect> zone_left_pix;
    for(int i =0; i<zoneLeft.size(); i++)
    {
        zone_left_pix.append(convertZone2pixel(zoneLeft.at(i)));
    }
    QVector<QRect> zone_right_pix;
    for(int i =0; i<zoneRight.size(); i++)
    {
        zone_right_pix.append(convertZone2pixel(zoneRight.at(i)));
    }
    d_plot_left->drawZone(zone_left_pix);
    d_plot_right->drawZone(zone_right_pix);
}

int PopupWindow::getNoiseMargin(void)
{
    return d_plot_left->getNoiseMargin();
}

