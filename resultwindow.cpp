#include "resultwindow.h"
#include "plot.h"
#include <QObject>
#include <QtDebug>
#include <QGridLayout>
#include <QVBoxLayout>

ResultWindow::ResultWindow(QWidget *parent) :
    QMainWindow(parent)
{
    //Add picture on background
    QPixmap bkgnd("C:/Users/arnau/Desktop/SMARTRONICS/RUNPAD/V3/IHM/V3.1/background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);

    //Another way to set the background of the main interface, it is too troublesome to adjust to tile (unsuccessful)
    /*setAutoFillBackground(true);
    QPixmap pixmap=QPixmap("C:/Users/arnau/Desktop/SMARTRONICS/RUNPAD/V3/IHM/V3.1/background.jpg").scaled(this->size());
    QPalette palette(this->palette());
    palette.setBrush(this->backgroundRole(),QBrush(pixmap.scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    this->setPalette(palette);*/

    d_plot_left = new Plot( this );
    d_plot_left->detachOverlay();
    d_plot_left->changeAlgo(true);

    d_plot_right = new Plot( this );
    d_plot_right->detachOverlay();
    d_plot_right->changeAlgo(true);

    m_text = new QTextEdit();
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(d_plot_left);
    hlayout->addWidget(m_text);
    hlayout->addWidget(d_plot_right);

    //Set Font
    QFont font("Times", 20, QFont::Bold);
    m_text->setFont(font);
    //text->setTextColor(Qt::darkCyan); //BlueELectric = #7DF9FF
    QColor newColor = QColor::fromRgb(125, 249, 255, 255);
    m_text->setTextColor(newColor);
    m_text->viewport()->setAutoFillBackground(false);
    //text->append("TEST POLICE");
    m_text->setAlignment(Qt::AlignCenter);
    m_text->setFrameStyle(QFrame::NoFrame);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout( hlayout );

    setCentralWidget( window );
}

ResultWindow::~ResultWindow()
{

}

void ResultWindow::display(QString text)
{
    m_text->setText(text);
}

void ResultWindow::dataUpdate_left(QVector<QVector <double> > *dataPacket)
{
    d_plot_left->setMatrixData(dataPacket);
}

void ResultWindow::dataUpdate_right(QVector<QVector <double> > *dataPacket)
{
    d_plot_right->setMatrixData(dataPacket);
}
