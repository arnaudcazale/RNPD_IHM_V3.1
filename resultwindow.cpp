#include "resultwindow.h"
#include "plot.h"
#include <QObject>
#include <QtDebug>
#include <QGridLayout>
#include <QVBoxLayout>

ResultWindow::ResultWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    //Add picture on background
    /*QPixmap bkgnd("C:/Users/arnau/Desktop/SMARTRONICS/RUNPAD/V3/IHM/V3.1/background.jpg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);*/

    //Black background
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
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

    //Set TextEdit
    m_text = new QTextEdit();
    m_text->setUndoRedoEnabled(false);

    //Set progressBar
    m_roundBar = new QRoundProgressBar;
    m_roundBar->setFormat("%v");
    m_roundBar->setDecimals(0);
    m_roundBar->setFixedSize(300, 300);
    m_roundBar->setAutoFillBackground(false);
    m_roundBar->hide();

    //m_roundBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //m_roundBar->show();

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(d_plot_left);
        QVBoxLayout *vlayout = new QVBoxLayout();
        vlayout->addWidget(m_text,4);
        vlayout->addWidget(m_roundBar,1, Qt::AlignCenter);
        vlayout->addStretch(1);
    hlayout->addLayout(vlayout);
    hlayout->addWidget(d_plot_right);

    //Set Font
    QFont font("Times", 40, QFont::Bold);
    m_text->setFont(font);
    //text->setTextColor(Qt::darkCyan); //BlueELectric = #7DF9FF
    QColor newColor = QColor::fromRgb(125, 249, 255, 255);
    m_text->setTextColor(newColor);
    m_text->viewport()->setAutoFillBackground(false);
    //text->append("TEST POLICE");
    m_text->setFrameStyle(QFrame::NoFrame);

    // Set layout in QWidget
    QWidget *window = new QWidget();
    window->setLayout( hlayout );

    setCentralWidget( window );
}

ResultWindow::~ResultWindow()
{

}

void ResultWindow::display(double deviationMean, double drop, double size, int step)
{
    qDebug() << "step" << step;
    /*qDebug() << "deviationMean" << deviationMean;
    qDebug() << "drop" << drop;
    qDebug() << "size" << size;*/

    switch (step)
    {
        case 1:
            m_text->clear();
            m_text->append( QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("STABILISEZ"));
            m_text->setAlignment(Qt::AlignCenter);
            break;

        case 2:
            m_text->clear();
            m_text->append( QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("ANALYSE"));
            m_text->setAlignment(Qt::AlignCenter);
            m_text->textChanged();
            m_roundBar->show();
            m_roundBar->setValue(20);
            break;

        case 3:
            m_roundBar->setValue(40);
            break;

        case 4:
            m_roundBar->setValue(60);
            break;

        case 5:
            m_roundBar->setValue(80);
            break;

        case 6:
            m_text->clear();
            m_text->append( QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("LEVEZ LES TALONS"));
            m_text->setAlignment(Qt::AlignCenter);
            m_roundBar->hide();
            break;
        case 7:
            m_text->clear();
            m_text->append( QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("ANALYSE"));
            m_text->setAlignment(Qt::AlignCenter);
            m_text->textChanged();
            m_roundBar->show();
            m_roundBar->setValue(20);
            break;

        case 8:
            m_roundBar->setValue(40);
            break;

        case 9:
            m_roundBar->setValue(60);
            break;

        case 10:
            m_roundBar->setValue(80);
            break;

        case 11:
            m_text->clear();
            m_roundBar->hide();
            m_text->append( QString("\n") +
                            QString("\n") +
                            QString("\n") +
                            QString("\n") );
            m_text->setAlignment(Qt::AlignCenter);

            //Display pronation
            if( deviationMean < -3){
                m_text->append( QString("SUPINAL\n") );
                m_text->setAlignment(Qt::AlignCenter);
            }
            else if( deviationMean > 3){
                m_text->append( QString("CONTROL\n") );
                m_text->setAlignment(Qt::AlignCenter);
            }
            else if( (deviationMean >=-3) && (deviationMean <=-1.5) ){
                m_text->append( QString("NEUTRE\n")   +
                                QString("TENDANCE\n") +
                                QString("SUPINAL\n") );
                m_text->setAlignment(Qt::AlignCenter);
            }
            else if( (deviationMean >=1.5) && (deviationMean <=3) ){
                m_text->append( QString("NEUTRE\n")   +
                                QString("TENDANCE\n") +
                                QString("CONTROL\n") );
                m_text->setAlignment(Qt::AlignCenter);
            }
            else{
                m_text->append( QString("NEUTRE\n") );
                m_text->setAlignment(Qt::AlignCenter);
            }

            //Display drop
            m_text->append( QString("DROP: ") +
                            QString::number(drop) +
                            QString("\n"));
            m_text->setAlignment(Qt::AlignCenter);

            //Display size
            m_text->append( QString("SIZE: ") +
                            QString::number(size, 'f', 1) +
                            QString("\n"));
            m_text->setAlignment(Qt::AlignCenter);

            break;

        case 12:
            m_text->clear();
            m_roundBar->hide();
            break;

    }

    m_text->setAlignment(Qt::AlignCenter);

}

void ResultWindow::dataUpdate_left(QVector<QVector <double> > *dataPacket)
{
    d_plot_left->setMatrixData(dataPacket);
}

void ResultWindow::dataUpdate_right(QVector<QVector <double> > *dataPacket)
{
    d_plot_right->setMatrixData(dataPacket);
}
