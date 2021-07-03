#include <qprinter.h>
#include <qprintdialog.h>
#include <qnumeric.h>
#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include "plot.h"
#include <qwt_matrix_raster_data.h>
#include <qwt_plot_zoneitem.h>
#include <qwt_plot_shapeitem.h>
#include <qwt_plot_canvas.h>

class Overlay : public QwtPlotZoneItem {
public:
    Overlay() : QwtPlotZoneItem()
    {
       QRect zone(0,0,0,0);
       for(int i=0; i<2; i++)
       {
           m_zones.append(zone);
       }
    }

    void setCoordonneesLines(QLine);
    void setCoordonneesPoints(QPoint);
    void setCoordonneesZones(QVector <QRect>);

    void draw(QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect) const
    {
        //Draw line
        painter->setPen(QPen(Qt::black, 6, Qt::SolidLine, Qt::RoundCap));
        painter->drawLine(m_line.x1(), m_line.y1(), m_line.x2(), m_line.y2());
        QwtPlotZoneItem::draw(painter,xMap,yMap,canvasRect);

        //Draw point
        painter->setPen(QPen(Qt::black, 16, Qt::SolidLine, Qt::RoundCap));
        painter->drawPoint(m_point.x(), m_point.y());
        QwtPlotZoneItem::draw(painter,xMap,yMap,canvasRect);

        for(int i=0; i<m_zones.size(); i++)
        {
            //qDebug() << "inDraw" << m_zones.at(i);
            painter->setPen(QPen(Qt::black, 8, Qt::SolidLine, Qt::RoundCap));
            painter->drawRect(m_zones.at(i));
            QwtPlotZoneItem::draw(painter,xMap,yMap,canvasRect);
        }
    }

private:
    QLine m_line;
    QPoint m_point;
    QVector <QRect> m_zones;
};

void Overlay::setCoordonneesLines(QLine line)
{
    m_line = line;
}

void Overlay::setCoordonneesPoints(QPoint point)
{
    m_point = point;
}

void Overlay::setCoordonneesZones(QVector <QRect> zones)
{
    //qDebug() << zones;
    m_zones.clear();
    for(int i=0; i<zones.size(); i++)
    {
        m_zones.append(zones.at(i));
    }
}

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};

class SpectrogramData: public QwtMatrixRasterData
{
public:
    SpectrogramData()
    {
        int sizeMatrix = LGN_NBR*(COL_NBR/2);
        double matrix[sizeMatrix]; //384

        setInterval( Qt::XAxis, QwtInterval( 0, COL_NBR) );
        setInterval( Qt::YAxis, QwtInterval( 0, LGN_NBR) );
        setInterval( Qt::ZAxis, QwtInterval( 0, 254 ) );

        for(int i = 0 ; i < sizeMatrix ; i++){
            //matrix[i] = (double) ((double)254/sizeMatrix) * i;
            matrix[i] = 0;
        }

        QVector<double> values;
        for ( uint i = 0; i < sizeof( matrix ) / sizeof( double ); i++ ){
            values += matrix[i];
        }

        setValueMatrix( values, COL_NBR );
    }

    SpectrogramData(QVector<QVector <double> > *dataMatrix)
    {
        setInterval( Qt::XAxis, QwtInterval( 0, COL_NBR) );
        setInterval( Qt::YAxis, QwtInterval( 0, LGN_NBR) );
        setInterval( Qt::ZAxis, QwtInterval( 0, 254) );

        QVector <double> matrix;

        for (int i = 0; i<LGN_NBR; i++)
        {
            for (int j = 0; j<COL_NBR; j++)
            {
                matrix.append(dataMatrix->at(i).at(j));
            }
        }
        setValueMatrix( matrix, COL_NBR );
    }

    void changeZScale(int max)
    {
        setInterval( Qt::ZAxis, QwtInterval( 0.0, max) );
    }

private:
    const int LGN_NBR = 96;
    const int COL_NBR = 16;

};

class LinearColorMapRGB: public QwtLinearColorMap
{
public:
    LinearColorMapRGB():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::RGB )
    {
        /*addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );*/
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.25, Qt::green );
        addColorStop( 0.5, Qt::yellow );
    }
};

class LinearColorMapIndexed: public QwtLinearColorMap
{
public:
    LinearColorMapIndexed():
        QwtLinearColorMap( Qt::darkCyan, Qt::red, QwtColorMap::Indexed )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }
};

class HueColorMap: public QwtColorMap
{
public:
    // class backported from Qwt 6.2

    HueColorMap():
        d_hue1(0),
        d_hue2(359),
        d_saturation(150),
        d_value(200)
    {
        updateTable();

    }

    virtual QRgb rgb( const QwtInterval &interval, double value ) const
    {
        if ( qIsNaN(value) )
            return 0u;

        const double width = interval.width();
        if ( width <= 0 )
            return 0u;

        if ( value <= interval.minValue() )
            return d_rgbMin;

        if ( value >= interval.maxValue() )
            return d_rgbMax;

        const double ratio = ( value - interval.minValue() ) / width;
        int hue = d_hue1 + qRound( ratio * ( d_hue2 - d_hue1 ) );

        if ( hue >= 360 )
        {
            hue -= 360;

            if ( hue >= 360 )
                hue = hue % 360;
        }

        return d_rgbTable[hue];
    }

    virtual unsigned char colorIndex( const QwtInterval &, double ) const
    {
        // we don't support indexed colors
        return 0;
    }


private:
    void updateTable()
    {
        for ( int i = 0; i < 360; i++ )
            d_rgbTable[i] = QColor::fromHsv( i, d_saturation, d_value ).rgb();

        d_rgbMin = d_rgbTable[ d_hue1 % 360 ];
        d_rgbMax = d_rgbTable[ d_hue2 % 360 ];
    }

    int d_hue1, d_hue2, d_saturation, d_value;
    QRgb d_rgbMin, d_rgbMax, d_rgbTable[360];
};

class AlphaColorMap: public QwtAlphaColorMap
{
public:
    AlphaColorMap()
    {
        //setColor( QColor("DarkSalmon") );
        setColor( QColor("SteelBlue") );
    }
};

QwtText QwtPlotPicker::trackerTextF( const QPointF &pos ) const
{
    QwtPlotItemList list = plot()->itemList(QwtPlotItem::Rtti_PlotSpectrogram);
    if (list.count() == 0)
        return trackerTextF( pos );
    else {
        QwtPlotSpectrogram * sp = static_cast<QwtPlotSpectrogram *> (list.at(0));
        double z = sp->data()->value(pos.x(), pos.y());

        QString text;
        text.sprintf( "%.4f, %.4f, %.4f", pos.x(), pos.y(), z );
        return QwtText( text );
    }
}

Plot::Plot( QWidget *parent ):
    QwtPlot( parent ),
    d_alpha(255),
    d_mode(0),
    d_noise_cancel(0),
    d_noise_margin(20)
{
    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    m_matrix_data = new QVector <QVector <double> >;
    m_matrix_data_noise = new QVector <QVector <double> >;

    QList<double> contourLevels;
    for ( double level = 0.5; level < 10.0; level += 1.0 )
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );

    d_spectrogram->setData( new SpectrogramData() );
    d_spectrogram->attach( this );

    QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    // A color bar on the right axis
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );

    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    //enableAxis( QwtPlot::yRight );

    setAxisScale(yLeft,0, 96);
    enableAxis(xBottom, false);
    enableAxis(yLeft, false);

    plotLayout()->setAlignCanvasToScales( true );

    setColorMap( Plot::RGBMap );

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size

    QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );

    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );

    const QColor c( Qt::darkBlue );
    zoomer->setRubberBandPen( c );
    zoomer->setTrackerPen( c );

    // create QwtPlotPicker to allow user moving the points on the plot
    QwtPlotPicker *picker = new QwtPlotPicker(canvas());

    d_overlay = new Overlay();
    //overlay->set_coordonnees(line);
    d_overlay->setZ(8);
    d_overlay->attach(this);
}

void Plot::changeAlgo( bool on )
{
    d_mode = on;
    setResampleMode(d_mode);
}

void Plot::showSpectrogram( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(
        on ? QPen( Qt::black, 0 ) : QPen( Qt::NoPen ) );

    replot();
}

void Plot::setColorMap( int type )
{
    QwtScaleWidget *axis = axisWidget( QwtPlot::yRight );
    QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );

    d_mapType = type;

    int alpha = d_alpha;
    switch( type )
    {
        case Plot::HueMap:
        {
            d_spectrogram->setColorMap( new HueColorMap() );
            axis->setColorMap( zInterval, new HueColorMap() );
            break;
        }
        case Plot::AlphaMap:
        {
            alpha = 255;
            d_spectrogram->setColorMap( new AlphaColorMap() );
            axis->setColorMap( zInterval, new AlphaColorMap() );
            break;
        }
        case Plot::IndexMap:
        {
            d_spectrogram->setColorMap( new LinearColorMapIndexed() );
            axis->setColorMap( zInterval, new LinearColorMapIndexed() );
            break;
        }
        case Plot::RGBMap:
        default:
        {
            d_spectrogram->setColorMap( new LinearColorMapRGB() );
            axis->setColorMap( zInterval, new LinearColorMapRGB() );
        }
    }
    d_spectrogram->setAlpha( alpha );

    replot();
}

void Plot::setAlpha( int alpha )
{
    // setting an alpha value doesn't make sense in combination
    // with a color map interpolating the alpha value

    d_alpha = alpha;
    if ( d_mapType != Plot::AlphaMap )
    {
        d_spectrogram->setAlpha( alpha );
        replot();
    }
}

#ifndef QT_NO_PRINTER

void Plot::printPlot()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setOrientation( QPrinter::Landscape );
    printer.setOutputFileName( "spectrogram.pdf" );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( this, printer );
    }
}

#endif

void Plot::setMatrixData(QVector < QVector<double> > *dataMatrix)
{
    //int max = *std::max_element(dataMatrix->constBegin(), dataMatrix->constEnd());
    //qDebug()<<"plot.Cpp" <<d_noise_margin;
    m_matrix_data->clear();
    m_matrix_data_noise->clear();

    //Fill data

    QVector<double> foo(0);
    QVector<double> foo_noise(0);

    for(int i = 0; i <LGN_NBR; i++)
    {
        for(int j = 0; j < COL_NBR; j++)
        {
            foo.append(dataMatrix->at(i).at(j));
            foo_noise.append(dataMatrix->at(i).at(j));

            if(foo.at(j) < d_noise_margin)
            {
                foo_noise.replace(j, 0);
            }

        }

        m_matrix_data->append(foo);
        m_matrix_data_noise->append(foo_noise);

        foo.clear();
        foo_noise.clear();
    }

    if(d_noise_cancel)
    {
        SpectrogramData *matrix = new SpectrogramData(m_matrix_data_noise);
        d_spectrogram->setData( matrix );
    }else
    {
        SpectrogramData *matrix = new SpectrogramData(m_matrix_data);
        d_spectrogram->setData( matrix );
    }

    setResampleMode(d_mode);
    //updateScale(max);
    replot();
}

void Plot::cancelNoise(bool on)
{
    d_noise_cancel = on;
    m_matrix_data_noise->clear();

    if((m_matrix_data->size()!=0)){

        if(d_noise_cancel )
        {
            QVector<double> foo(0);
            QVector<double> foo_noise(0);

            for(int i = 0; i <LGN_NBR; i++)
            {
                for(int j = 0; j < COL_NBR; j++)
                {
                    foo.append(m_matrix_data->at(i).at(j));
                    foo_noise.append(m_matrix_data->at(i).at(j));

                    if(foo.at(j) < d_noise_margin)
                    {
                        foo_noise.replace(j, 0);
                    }

                }
                m_matrix_data_noise->append(foo_noise);

                foo.clear();
                foo_noise.clear();
            }

            SpectrogramData *matrix = new SpectrogramData(m_matrix_data_noise);
            d_spectrogram->setData( matrix );
        }else
        {
            SpectrogramData *matrix = new SpectrogramData(m_matrix_data);
            d_spectrogram->setData( matrix );
        }

        setResampleMode(d_mode);
        //updateScale(max);
        replot();
    }

}


void Plot::updateScale(int max)
{
    QwtInterval zInterval( 0.0, max, QwtInterval::IncludeBorders );
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis( QwtPlot::yRight );
    plotLayout()->setAlignCanvasToScales( true );
    setColorMap( Plot::RGBMap );
}


void Plot::setResampleMode( int mode )
{
    d_mode = mode;
    SpectrogramData *data = static_cast<SpectrogramData *>( d_spectrogram->data() );
    data->setResampleMode(
        static_cast<QwtMatrixRasterData::ResampleMode>( mode ) );

    replot();
}

void Plot::drawLine(QLine line)
{
    d_overlay->setCoordonneesLines(line);
}

void Plot::drawPoint(QPoint point)
{
    d_overlay->setCoordonneesPoints(point);
}

void Plot::drawZone(QVector <QRect> zones)
{
    d_overlay->setCoordonneesZones(zones);
}

void Plot::updateNoiseMargin(int noiseMargin)
{
    d_noise_margin = noiseMargin;
    //qDebug() << d_noise_margin;
}

int Plot::getNoiseMargin(void)
{
    return d_noise_margin;
}

void Plot::detachOverlay( void )
{
    d_overlay->detach();
}




