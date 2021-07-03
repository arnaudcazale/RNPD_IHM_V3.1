/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "popupwindow.h"
#include "displaywindow.h"
#include "sequencer.h"
#include "resultwindow.h"

#include <QLabel>
#include <QMessageBox>
#include <QtDebug>
#include <QDesktopWidget>
#include <QStyle>
#include <QTimer>

#include "math.h"
#include <QtEndian>
#include <windows.h>


//! [0]
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_status(new QLabel),
    m_console(new Console),
    m_settings(new SettingsDialog),
    m_popupwindow(new PopupWindow),
    m_popupwindowGravity(new PopupWindow),
    m_display(new DisplayWindow),
    m_display_pointure(new DisplayWindow),
    m_display_gravity(new DisplayWindow),
//! [1]
    m_serial(new QSerialPort(this)),
    m_data(new QVector<quint8>),
    m_sequencer(new Sequencer),
    m_player(new QMediaPlayer),
    m_videoWidget(new QVideoWidget),
    m_resultWindow(new ResultWindow)

//! [1]
{
//! [0]
    m_count = 0;
    filling = false;
    gravity = true;
    size = false;

    for(int i = 0; i < LGN_NBR; i++) //of course you might not want to init the vectors in a loop - this is just an example
    {
        QVector<double> foo; //create a QVector of doubles
        for(int j = 0; j < COL_NBR; j++)
        {
            foo.append(0);
        }
        m_data_left.append(foo);
        m_data_right.append(foo);
        m_data_bin_left.append(foo);
        m_data_bin_right.append(foo);
        m_data_filter_left.append(foo);
        m_data_filter_right.append(foo);
        m_data_left_heel.append(foo);
        m_data_right_heel.append(foo);
        m_data_left_toe.append(foo);
        m_data_right_toe.append(foo);
        m_data_left_full.append(foo);
        m_data_right_full.append(foo);
        m_data_left_buff.append(foo);
        m_data_right_buff.append(foo);
    }

    m_ui->setupUi(this);
    m_console->setEnabled(false);
    m_popupwindow->resize(900, 800);
    m_popupwindow->setWindowFlags(Qt::WindowStaysOnTopHint);
    m_popupwindow->show();
    m_popupwindowGravity->resize(900, 800);
    m_popupwindowGravity->setWindowFlags(Qt::WindowStaysOnTopHint);
    m_popupwindowGravity->show();
    //m_display->show();
    m_display->resize(500, 250);
    //m_display_pointure->show();
    m_display_pointure->resize(500, 250);
    //m_display_gravity->show();
    m_display_gravity->resize(500, 250);
    setCentralWidget(m_console);
    showMinimized();

    m_popupwindow->move(0,0);
    m_popupwindowGravity->move(950,0);
    //m_display->move(0,800);
    //m_display_pointure->move(500,800);
    //m_display_gravity->move(1000,800);

    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);

    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);

//! [2]
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
//! [2]
    connect(m_console, &Console::getData, this, &MainWindow::writeData);
    //connect(m_sequencer, &Sequencer::getData, this, &MainWindow::writeData);

//! [3]
    connect(this, SIGNAL(dataReady_left(QVector<QVector <double> > *)), m_popupwindow, SLOT(dataUpdate_left(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReady_right(QVector<QVector <double> > *)), m_popupwindow, SLOT(dataUpdate_right(QVector<QVector <double> > *)));
    //connect(this, SIGNAL(dataReady_line(QVector <QLine>)), m_popupwindow, SLOT(drawLine(QVector <QLine>)));
    connect(this, SIGNAL(dataReadyGravity_left(QVector<QVector <double> > *)), m_popupwindowGravity, SLOT(dataUpdate_left(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReadyGravity_right(QVector<QVector <double> > *)), m_popupwindowGravity, SLOT(dataUpdate_right(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReadyGravity_left(QVector<QVector <double> > *)), m_resultWindow, SLOT(dataUpdate_left(QVector<QVector <double> > *)));
    connect(this, SIGNAL(dataReadyGravity_right(QVector<QVector <double> > *)), m_resultWindow, SLOT(dataUpdate_right(QVector<QVector <double> > *)));
    //connect(this, SIGNAL(dataReadyGravity_line(QVector <QLine>)), m_popupwindowGravity, SLOT(drawLine(QVector <QLine>)));
    connect(this, SIGNAL(dataReady_point(QPoint, QPoint)), m_popupwindowGravity, SLOT(drawPoint(QPoint, QPoint)));
    connect(this, SIGNAL(dataReady_zone(QVector <QRect>, QVector <QRect>)), m_popupwindowGravity, SLOT(drawZone(QVector <QRect>, QVector <QRect>)));
    connect(this, SIGNAL(dataReady_line(QLine, QLine)), m_popupwindowGravity, SLOT(drawLine(QLine, QLine)));
    connect(m_sequencer, SIGNAL(getData(const QByteArray)), this, SLOT(writeData(const QByteArray)));

    openSerialPort();

    //Set video widget
    m_player->setMedia(QUrl("file:///C:/Users/arnau/Desktop/SMARTRONICS/RUNPAD/V3/IHM/V3.1/invitation.mp4"));
    m_player->setVideoOutput(m_videoWidget);
    m_videoWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    m_videoWidget->resize(900, 800);
    m_videoWidget->showMaximized();
    m_player->play();

    //m_resultWindow->showMaximized();

    //Start sequencer
    m_sequencer->RUN_SINGLE();
}
//! [3]

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
    delete m_player;
    delete m_videoWidget;
}

//! [4]
void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    /*qDebug()<<p.name;
    qDebug()<<p.baudRate;
    qDebug()<<p.dataBits;
    qDebug()<<p.parity;
    qDebug()<<p.stopBits;
    qDebug()<<p.flowControl;*/
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}
//! [4]

//! [5]
void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
        m_serial->close();
    m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionConfigure->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}
//! [5]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Simple Terminal"),
                       tr("The <b>Simple Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

//! [6]
void MainWindow::writeData(const QByteArray &data)
{
    qDebug() << data;
    m_serial->write(data);
}
//! [6]

//! [7]
void MainWindow::readData()
{
     while(m_serial->bytesAvailable() > 0)
     {
         //qDebug()<<m_serial->peek(1);
         //Append into buffer
         if(filling)
         {
             QByteArray data = m_serial->read(1);
             //qDebug() << data;
             //m_data->append(static_cast<quint8>(data[0]));
             m_data->append(data[0]);
             if(m_data->size() == 1536)//768
             {
                 //qDebug() << *m_data;
                 filling = false;
                 //Detection if someone is present on the runpad
                 if(int mean = getRNPDMean(m_data, m_data->size()) >= 5 )
                 {
                    //m_videoWidget->hide();
                    if(m_player->state() == QMediaPlayer::PlayingState){
                        m_player->stop();
                    }

                    m_count_measure++;
                    qDebug() << "m_count_measure" << m_count_measure;

                    if( m_count_measure == 1 )
                    {
                        //reset previously foot image
                        resetMatrix(&m_data_filter_left);
                        resetMatrix(&m_data_filter_right);
                        emit dataReadyGravity_left(&m_data_filter_left);
                        emit dataReadyGravity_right(&m_data_filter_right);

                        m_resultWindow->showMaximized();
                        m_resultWindow->display(0,0,0,m_count_measure);
                        msleep(3000);
                        m_sequencer->RUN_MULTI();

                    }else if(m_count_measure == 2){
                        getMeasure();
                        m_sequencer->RUN_SINGLE();

                    }else if(m_count_measure == 3){
                        m_resultWindow->display(0,0,0,m_count_measure);
                        msleep(3000);
                        m_sequencer->RUN_MULTI();

                    }else if(m_count_measure == 4){
                        getMeasure();
                        m_resultWindow->display(m_deviationMean, m_drop, m_size, m_count_measure);
                        m_count_measure = 0;
                        msleep(8000);
                        m_sequencer->RUN_SINGLE();
                    }
                 }else
                 {
                    //m_videoWidget->showMaximized();
                    m_resultWindow->hide();
                    if(m_player->state() == QMediaPlayer::StoppedState){
                        m_player->play();
                    }
                    m_sequencer->RUN_SINGLE();
                    //Reset counting if measure not finished and presence is false
                    m_count_measure = 0;
                 }

                 //Show spectrogramms (debug purpose)
                 /*splitDataFillZero();
                 fillLeftDataMeanNeightboorhood();
                 fillRightDataMeanNeightboorhood();
                 emit dataReady_left(&m_data_left);
                 emit dataReady_right(&m_data_right);
                 m_sequencer->RUN_SINGLE();*/
                 ///////////////////////////////////////

                 m_data->clear();
             }

         }else
         {
             //Catch trame start
             if( (m_serial->peek(1) !=  "\xFF") )
             {
                 //qDebug() << data_console;
                 // si les octets déjà lus ne commencent pas
                 // par la bonne séquence, on les ignore jusqu'à ce qu'on la trouve
                 m_serial->getChar(0);
             }else
             {
                 QByteArray data = m_serial->read(1);
                 //qDebug()<<"data start"<<data;
                 filling = true;
             }

             //Data goes to console
             //QByteArray data_console = m_serial->peek(1);
             //m_console->putData(data_console);
         }
     }
}

void MainWindow::getMeasure()
{
    splitDataFillZero();
    fillLeftDataMeanNeightboorhood();
    fillRightDataMeanNeightboorhood();

    //Show spectrogramms (debug purpose)
    emit dataReady_left(&m_data_left);
    emit dataReady_right(&m_data_right);

    if(m_count_measure == 2)
    {
        storeHeelData();
        pronationGet();
    }else if(m_count_measure == 4){
        storeToeData();
        m_size = sizeGet();
        //qDebug() << size;
        resetAccumulateVector();
        //m_count_measure = 0;
        //Display graph
        //emit dataReadyGravity_left(&m_data_left_full);
        //emit dataReadyGravity_right(&m_data_right_full);
    }
}

void MainWindow::resetMatrix(QVector <QVector <double> > *matrix)
{
    matrix->clear();

    for(int i = 0; i < LGN_NBR; i++)
    {
        QVector<double> foo;
        for(int j = 0; j < COL_NBR; j++)
        {
            foo.append(0);
        }
        matrix->append(foo);
    }
}

void MainWindow::msleep(int msec)
{
    QEventLoop loop;

    QTimer::singleShot(msec, &loop, &QEventLoop::quit);

    loop.exec();
}

void MainWindow::accumulate(QVector <QVector <double> > *matrix_left, QVector <QVector <double> > *matrix_right)
{
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
           m_data_left_buff[i][j]  += matrix_left->at(i).at(j);
           m_data_right_buff[i][j] += matrix_right->at(i).at(j);
        }
    }
}

void MainWindow::storeHeelData()
{
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
           m_data_left_heel[i][j]  = m_data_left[i][j];
           m_data_right_heel[i][j] = m_data_right[i][j];
        }
    }
}

void MainWindow::storeToeData()
{
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
           m_data_left_toe[i][j]   = m_data_left[i][j];
           m_data_right_toe[i][j]  = m_data_right[i][j];
           m_data_left_full[i][j]  = (m_data_left_heel[i][j] + m_data_left_toe[i][j]);
           m_data_right_full[i][j] = (m_data_right_heel[i][j] + m_data_right_toe[i][j]);
        }
    }
}

void MainWindow::resetAccumulateVector()
{
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
           m_data_left_buff[i][j]  = 0;
           m_data_right_buff[i][j] = 0;
        }
    }
}

int MainWindow::getRNPDMean(QVector <unsigned char> *data, int size){
    int mean = 0;
    for(int i=0; i<size; i++)
    {
        mean += data->at(i);
    }
    return mean/size;
}

void MainWindow::pronationGet(){

    //Binarize matrix
    binarizeFromMean(&m_data_left_heel, &m_data_bin_left);
    binarizeFromMean(&m_data_right_heel, &m_data_bin_right);
    //binarizeFromNoiseMargin(&m_data_left, &m_data_bin_left);
    //binarizeFromNoiseMargin(&m_data_right, &m_data_bin_right);

    //Filter matrix
    filterMatrix(&m_data_left_heel, &m_data_bin_left, &m_data_filter_left);
    filterMatrix(&m_data_right_heel, &m_data_bin_right, &m_data_filter_right);

    //Display graph debug
    emit dataReadyGravity_left(&m_data_filter_left);
    emit dataReadyGravity_right(&m_data_filter_right);

    //Find zones
    qDebug() << "**************************LEFT*******************************";
    QVector <QRect> zonesLeft = findZones(&m_data_filter_left);
    //Find barycentre of upper zone
    QPoint barycentreLeft = barycentreGet(&m_data_filter_left, zonesLeft);
    //Find neutral line (center of largest rows of each zones)
    QLine neutralLineLeft = neutralLineGet(&m_data_bin_left, zonesLeft);
    //Compute deviation between barycentre and neutral line
    double deviationLeft = deviationGet(neutralLineLeft, barycentreLeft, true);

    //repat same process for other foot
    qDebug() << "**************************RIGHT*******************************";
    QVector <QRect> zonesRight = findZones(&m_data_filter_right);
    QPoint barycentreRight = barycentreGet(&m_data_filter_right, zonesRight);
    QLine neutralLineRight = neutralLineGet(&m_data_bin_right, zonesRight);
    double deviationRight = deviationGet(neutralLineRight, barycentreRight, false);

    m_deviationMean = ( deviationLeft + deviationRight ) / 2;
    //m_pronation = pronationDisplay(deviationMean);
    //qDebug() << "Pronation type " << pronation;

    //Emit signals for displaying graphic items
    emit(dataReady_zone(zonesLeft, zonesRight));
    emit(dataReady_point(barycentreLeft, barycentreRight));
    emit(dataReady_line(neutralLineLeft, neutralLineRight));

    //Drop computing
    //Compute drop (poids relatif de la zone supérieure et inférieure
    QVector <long> sumLeft = gravityGet(&m_data_filter_left, zonesLeft);
    QVector <long> sumRight = gravityGet(&m_data_filter_right, zonesRight);
    m_drop = gravityCompute(sumLeft, sumRight);
}

 double MainWindow::gravityCompute(QVector <long> sumLeft, QVector <long> sumRight)
 {
     qDebug() << "**************************gravityCompute*******************************";
     double gravity = 0;
     double igravity = 0;
     if( (sumLeft.size() == 2) && (sumRight.size() == 2))
     {
         long totalSum = sumLeft.at(0) + sumLeft.at(1) + sumRight.at(0) + sumRight.at(0);
         qDebug() << "totalSum = " << totalSum;
         gravity = (double)(sumLeft.at(1) + sumRight.at(1)) / (double)totalSum;
         qDebug() << "gravity = " << gravity;
         double alpha = 2.0 / 3.0;
         igravity = 0;
         if( gravity >= alpha)
             igravity = 0;
         else
             igravity = (uint8_t)(((alpha - gravity) / alpha) * 17);
         qDebug() << "igravity = " << igravity;
     }else
     {
         qDebug() << "igravity non disponible";
     }

     return igravity;
 }

QVector <long> MainWindow::gravityGet(QVector <QVector <double> > *matrix, QVector <QRect> zones)
{
    qDebug() << "**************************dropGet*******************************";
    int medianRow = 0;
    QVector <long> sum (0,0);
    //Si 2 zones
    if(zones.size() > 1)
    {
        medianRow = zones.at(0).top() + ( (zones.at(1).top() - zones.at(1).height() + 1) - zones.at(0).top() ) / 2;
        qDebug() << "medianRow" << medianRow;
        long lowerSum = sumMatrix(&m_data_filter_left, 0, medianRow);
        long upperSum = sumMatrix(&m_data_filter_left, medianRow, LGN_NBR);
        sum.append(lowerSum);
        sum.append(upperSum);
        qDebug() << "upperSum" << upperSum;
        qDebug() << "lowerSum" << lowerSum;

    }else
    {
        qDebug() << "Drop indisponible";
    }

    return sum;
}

QPoint MainWindow::barycentreGet(QVector <QVector <double> > *matrix, QVector <QRect> zones)
{
    qDebug() << "**************************barycentreGet*******************************";
    uint32_t bi = 0, bj = 0, coef_sum = 0;
    QPoint barycentre(bi, bj);
    int startLine = 0;
    int endLine = 0;
    int startCol = 0;
    int endCol = 0;
    bool validZone = false;
    //Si il y a 2 zones, on selectionne la zone supérieure
    if(zones.size() > 1)
    {
        qDebug() << "zones.at(0)" << zones.at(0);
        qDebug() << "zones.at(1)" << zones.at(1);
        startLine = zones.at(1).top() - zones.at(1).height() + 1 ;
        endLine = zones.at(1).top();
        startCol = zones.at(1).left();
        endCol = zones.at(1).right() - 1;
    }else
    {
        qDebug() << zones.at(0);
        startLine = zones.at(0).top() - zones.at(0).height() + 1;
        endLine = zones.at(0).top();
        startCol = zones.at(0).left();
        endCol = zones.at(0).right() - 1;
    }
    qDebug() << "startLine" << startLine << "endLine" << endLine;
    qDebug() << "startCol" << startCol << "endCol" << endCol;

    //Check if data is Valid
    if(startLine >= 0 && startLine <= 47)
    {
        if(endLine >= 0 && endLine <= 47)
        {
            if(startCol >= 0 && startCol <= 15)
            {
                if(endCol >= 0 && endCol <= 15)
                {
                    validZone = true;
                }
            }
        }
    }
    if(endLine - startLine > 0)
    {
        if(endCol - startCol > 0)
        {
            validZone = true;
        }
    }

    if(validZone)
    {
        // calcul des coordonnées du barycentre //
        for( int i = startLine; i < endLine; i++)
        {
            for( int j = startCol; j < endCol; j++)
            {
                bi += matrix->at(i).at(j) * i;
                bj += matrix->at(i).at(j) * j;
                coef_sum += matrix->at(i).at(j);
            }
        }

        bi /= coef_sum;
        bj /= coef_sum;

        barycentre.setX(bj);
        barycentre.setY(bi);
    }else
    {
        qDebug() << "ZONE UNVALID";
    }

    qDebug() << "barycentre" << barycentre ;
    return barycentre;
}

QLine MainWindow::neutralLineGet(QVector <QVector <double> > *matrix, QVector <QRect> zones)
{
    qDebug() << "**************************neutralLineGet*******************************";

    QLine line(QPoint(0,0), QPoint(0,0));
    int startLine = 0;
    int endLine = 0;
    int startCol = 0;
    int endCol = 0;
    int largestRowIdx = 0;
    double centerColIdx = 0;
    QVector<int> rowSum;

    //Si 2 zones
    if(zones.size() > 1)
    {
        //Find center of largest row of upper zone
        startLine = zones.at(1).top() - zones.at(1).height() + 1 ;
        endLine = zones.at(1).top();

        rowSum = getRowSum(startLine, endLine, matrix);
        qDebug() << "rowSum UP" << rowSum;
        largestRowIdx = getLargestRowIdx(startLine, endLine, &rowSum, true);
        qDebug() << "largestRowIdx UP" << largestRowIdx;
        centerColIdx = getCenterColIdx(largestRowIdx, matrix);
        qDebug() << "centerColIdx UP" << centerColIdx;
        line.setP1(QPoint(centerColIdx, largestRowIdx));

        //Find center of largest row of lower zone
        startLine = zones.at(0).top() - zones.at(0).height() + 1 ;
        endLine = zones.at(0).top();

        rowSum = getRowSum(startLine, endLine, matrix);
        qDebug() << "rowSum DOWN" << rowSum;
        largestRowIdx = getLargestRowIdx(startLine, endLine, &rowSum, false);
        qDebug() << "largestRowIdx DOWN" << largestRowIdx;
        centerColIdx = getCenterColIdx(largestRowIdx, matrix);
        qDebug() << "centerColIdx DOWN" << centerColIdx;
        line.setP2(QPoint(centerColIdx, largestRowIdx));

    }else //Si 1 seule zone, on cherche la ligne qui la sépare à la moitié
    {
        startLine = zones.at(0).top() - zones.at(0).height() + 1 ;
        endLine = zones.at(0).top();
        startCol = zones.at(0).left();
        endCol = zones.at(0).right() - 1;
        int centerCol = startCol + ( (endCol - startCol) / 2);
        line.setP1(QPoint(centerCol, startLine));
        line.setP2(QPoint(centerCol, endLine));
    }

    return line;
}

void MainWindow::computeSize(){

    //size computing
    emit dataReady_left(&m_data_left);
    emit dataReady_right(&m_data_right);

    double size = sizeGet();
    m_pointure.append(size);

    if( (m_count % 2) == 0)
    {
        qDebug()<<m_pointure;
        double pointure_mean = ( m_pointure.at(m_pointure.size()-1) + m_pointure.at(m_pointure.size()-2)) / 2;
        double pointure_round = round(pointure_mean*2)/2;
        dataDisplay_pointure.append(QString::number(pointure_round) + "\n");
        m_display_pointure->putData(dataDisplay_pointure);

        size = false;
        gravity = true;
    }

    m_display->putData(dataDisplay);

    dataDisplay.clear();
    dataDisplay_pointure.clear();
    m_lines.clear();
}

void MainWindow::splitData()
{
    m_data_left.clear();
    m_data_right.clear();

    QVector <double>  left;
    QVector <double>  right;

    for(int i = 0; i < m_data->size()/2; i+=16)
    {
        for(int j = 0; j <16; j++)
        {
            left.append(m_data->at(i+j));
        }
        m_data_left.append(left);
        left.clear();
    }

    for(int i = m_data->size()/2; i < m_data->size(); i+=16)
    {
        for(int j = 0; j < 16; j++)
        {
            right.append(m_data->at(i+j));
        }
        m_data_right.append(right);
        right.clear();
    }

    m_data->clear();
}

void MainWindow::splitDataFillZero()
{
    m_data_left.clear();
    m_data_right.clear();

    QVector <QVector <double> > left;
    QVector <QVector <double> > right;

    for(int i = 0; i < LGN_NBR; i++) //of course you might not want to init the vectors in a loop - this is just an example
    {
        QVector<double> foo; //create a QVector of doubles
        left.append(foo);
        right.append(foo);
    }

    bool toggle = false;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            for(int j = 0; j < COL_NBR/2; j++)
            {
                left[i].append(0);
                left[i].append(m_data->at( (i* (COL_NBR) ) + j));  
                //qDebug()<< i*(COL_NBR/2) + j;
            }
            toggle = true;
        }else
        {
            for(int j = 0; j < COL_NBR/2; j++)
            { 
                left[i].append(m_data->at( (i* (COL_NBR) ) + j));
                left[i].append(0);
            }
            toggle = false;
        }
        //qDebug()<< "left["<< i << "] = " << left.at(i);
    }

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            for(int j = COL_NBR/2; j < COL_NBR; j++)
            {
                right[i].append(m_data->at( (i* (COL_NBR) ) + j));
                right[i].append(0);
                //qDebug()<< (i* (COL_NBR/2) ) + j + (LGN_NBR*(COL_NBR/2));
            }
            toggle = true;
        }else
        {
            for(int j = COL_NBR/2; j < COL_NBR; j++)
            {
                right[i].append(0);
                right[i].append(m_data->at( (i* (COL_NBR) ) + j));   
            }
            toggle = false;
        }
        //qDebug()<< "right["<< i << "] = " << right.at(i);
    }

    //Reverse order
    /*for(int i=left.size()-1; i>=0;i--){
      m_data_left.append(left.at(i));
    }

    //Reverse order
    for(int i=right.size()-1; i>=0;i--){
      m_data_right.append(right.at(i));
    }*/

    for(int i=0; i<right.size();i++){
      m_data_left.append(left.at(i));
    }
    //qDebug()<< m_data_left;

    for(int i=0; i<right.size();i++){
      m_data_right.append(right.at(i));
    }

}

//! [7]
void MainWindow::fillLeftDataMeanNeightboorhood()
{
    bool toggle = false;
    double mean_data = 0;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            //qDebug() << "i"<< i;
            for(int j = 0; j < COL_NBR; j = j+2)
            {
                //qDebug() << "j"<< j;

                //Si 1ere ligne et 1ere colonne
                if( (i == 0) && (j == 0) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j+1) ) / 2;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i == 0) && (j != 0) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);

                //Sinon Si 1ere colonne
                }else if( (i != 0) && (j == 0) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i != 0) && (j != 0) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 4;
                    m_data_left[i].replace(j, mean_data);
                }
            }

            toggle = true;

        }else
        {
            for(int j = 1; j < COL_NBR; j = j+2)
            {
                if( (i == LGN_NBR-1) && (j == COL_NBR-1) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) ) / 2;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i == LGN_NBR-1) && (j != COL_NBR-1) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 3;
                    m_data_left[i].replace(j, mean_data);

                //Sinon Si dernière colonne
                }else if( (i != LGN_NBR-1) && (j == COL_NBR-1) )
                {
                    mean_data = ( m_data_left.at(i+1).at(j) + m_data_left.at(i).at(j-1) + m_data_left.at(i-1).at(j) ) / 3;
                    m_data_left[i].replace(j, mean_data);
                }else if( (i != LGN_NBR-1) && (j != COL_NBR-1) )
                {
                    mean_data = ( m_data_left.at(i).at(j-1) + m_data_left.at(i+1).at(j) + m_data_left.at(i-1).at(j) + m_data_left.at(i).at(j+1) ) / 4;
                    m_data_left[i].replace(j, mean_data);
                }
            }

            toggle = false;
        }
    }
}

void MainWindow::fillRightDataMeanNeightboorhood()
{
    bool toggle = false;
    int mean_data;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        if(!toggle)
        {
            //qDebug() << "i"<< i;
            for(int j = 1; j < COL_NBR; j = j+2)
            {
                //qDebug() << "j"<< j;
                //Si 1ere ligne et 1ere colonne
                if( (i == 0) && (j == COL_NBR-1) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j-1) ) / 2;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i == 0) && (j != COL_NBR-1) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j+1) ) / 3;
                    m_data_right[i].replace(j, mean_data);

                //Sinon Si 1ere colonne
                }else if( (i != 0) && (j == COL_NBR-1) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j-1) ) / 3;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i != 0) && (j != COL_NBR-1) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 4;
                    m_data_right[i].replace(j, mean_data);
                }
            }

            toggle = true;

        }else
        {
            for(int j = 0; j < COL_NBR; j = j+2)
            {
                if( (i == LGN_NBR-1) && (j == 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j+1) + m_data_right.at(i-1).at(j) ) / 2;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i == LGN_NBR-1) && (j != 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 3;
                    m_data_right[i].replace(j, mean_data);

                //Sinon Si dernière colonne
                }else if( (i != LGN_NBR-1) && (j == 0) )
                {
                    mean_data = ( m_data_right.at(i+1).at(j) + m_data_right.at(i).at(j+1) + m_data_right.at(i-1).at(j) ) / 3;
                    m_data_right[i].replace(j, mean_data);
                }else if( (i != LGN_NBR-1) && (j != 0) )
                {
                    mean_data = ( m_data_right.at(i).at(j-1) + m_data_right.at(i+1).at(j) + m_data_right.at(i-1).at(j) + m_data_right.at(i).at(j+1) ) / 4;
                    m_data_right[i].replace(j, mean_data);
                }
            }

            toggle = false;
        }
    }
}

void MainWindow::binarizeFromNoiseMargin(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin)
{
    matrix_bin->clear();

    //int noiseMargin = m_popupwindow->getNoiseMargin();
    int noiseMargin = 50;
    qDebug() << noiseMargin;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        QVector<double> foo;

        for(int j = 0; j < COL_NBR; j++)
        {
            if( matrix->at(i).at(j) > noiseMargin)
                foo.append(1);
            else
                foo.append(0);
        }

        matrix_bin->append(foo);
        //qDebug() << matrix_bin->at(i);
    }
}

void MainWindow::binarizeFromMean(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin)
{
    matrix_bin->clear();

    // calculate means of both matrixes
    double mean = calc_mean(matrix);
    qDebug() << "matrix_mean" << mean;
    //unsigned int mean_right = calc_mean(&m_data_right);
    //unsigned int mean = (mean_left + mean_right)/2;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        QVector<double> foo;

        for(int j = 0; j < COL_NBR; j++)
        {
            if( matrix->at(i).at(j) > mean)
                foo.append(1);
            else
                foo.append(0);
        }

        matrix_bin->append(foo);
    }
}

QVector<int> MainWindow::getRowSum(int startLine, int endLine, QVector <QVector <double> > *matrix)
{
    QVector<int> rowSum;
    // make sum tab for each lines from bin matrix*/
    for( int i = startLine; i < endLine; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix->at(i).at(j);
        }
        rowSum.append(sum);
    }
    return rowSum;
}

int MainWindow::getLargestRowIdx(int startLine, int endLine, QVector<int> *rowSum, bool isUp)
{
    int largestRowIdx = 0;
    int rowMax = 0;

    //Si zone supérieure et largeur de lignes égales, on priorise la ligne la plus haute
    if(isUp)
    {
        //Find index of larger line
        for(int i = 0; i < rowSum->size(); i++)
        {
            if(rowSum->at(i) >= rowMax)
            {
                largestRowIdx = i;
                rowMax = rowSum->at(i);
            }
        }
        largestRowIdx += startLine;

    //Si zone inférieure et largeur de lignes égales, on priorise la ligne la plus basse
    }else
    {
        //Find index of larger line
        int cpt = 0;
        for(int i = rowSum->size()-1; i > 0; i--)
        {
            cpt++;
            if(rowSum->at(i) >= rowMax)
            {
                largestRowIdx = cpt;
                rowMax = rowSum->at(i);
            }
        }
        int temp = endLine - largestRowIdx;
        largestRowIdx = temp;
    }

    return largestRowIdx;
}

double MainWindow::getCenterColIdx(int largestRowIdx, QVector <QVector <double> > *matrix)
{
    double centerColIdx = 0;
    int startCol = 0;
    int endCol = 0;

    //Find left beggining of largest line
    for(int j = 0; j<COL_NBR; j++)
    {
        if(matrix->at(largestRowIdx).at(j) == 1){
            startCol = j;
         break;
        }
    }
    qDebug() << "startCol" << startCol;

    //Find right end of largest line
    for(int j = COL_NBR-1; j>=0; j--)
    {
        if(matrix->at(largestRowIdx).at(j) == 1){
            endCol = j;
         break;
        }
    }
    qDebug() << "endCol" << endCol;


    centerColIdx = (startCol + (endCol-startCol)/2) ;

    return centerColIdx;
}

double MainWindow::deviationGet(QLine neutralLine, QPoint barycentrePoint, bool isLeft)
{
    double a = 0;
    double b = 0;
    double tcol = 0;
    double dev = 0;

    double angle = 0;
    double deltaY = 0;
    double deltaX = 0;
    //double angle_relatif = 0;

    deltaY = (double) neutralLine.p1().y() - (double) neutralLine.p2().y();
    deltaX = (double) neutralLine.p1().x() - (double) neutralLine.p2().x();
    //qDebug() << "deltaY" << deltaY;
    //qDebug() << "deltaX" << deltaX;
    angle = abs(atan( (deltaY/2) / deltaX ));

    if(neutralLine.p1().x() == neutralLine.p2().x())
    {
        qDebug() << "droite verticale";
        tcol = neutralLine.p1().x();
    }else
    {
        qDebug() << "fonction affine";
        a = ( (double)neutralLine.p1().y() - (double)neutralLine.p2().y() ) / ( (double)neutralLine.p1().x() - (double)neutralLine.p2().x() )  ;
        b = (double)neutralLine.p1().x() - (a * (double)neutralLine.p1().x());
        qDebug() << "y = " << a << "x +" << b;
        qDebug() << "bi = " << barycentrePoint.x();
        tcol = ((double)barycentrePoint.x() - b) / a;
    }
    qDebug() << "tcol = " << tcol;

    if(isLeft)
    {
        dev = (barycentrePoint.x()) - tcol;
        m_left_angle = angle;
    }else
    {
        dev = tcol - (barycentrePoint.x());
        m_right_angle = angle;
    }
    qDebug() << "deviation = " << dev;

    return dev;
}

QString MainWindow::pronationDisplay(double deviationMean)
{
    qDebug() << "deviation mean" << deviationMean;
    QString pronation = "";

    if( deviationMean < -3)
        pronation = "SUPINAL\n";
    else if( deviationMean > 3)
        pronation = "CONTROL\n";
    else if( (deviationMean >=-3) && (deviationMean <=-1.5) )
        pronation = "NEUTRE TENDANCE SUPINAL\n";
    else if( (deviationMean >=1.5) && (deviationMean <=3) )
        pronation = "NEUTRE TENDANCE CONTROL\n";
    else
        pronation = "NEUTRE\n";

    return pronation;
}

unsigned int MainWindow::get_median_line(QVector <QVector <double> > *matrix_bin)
{
    QVector <double> *sum_lines = new QVector <double> ();
    QVector <double> *extremum_line = new QVector <double> ();
    QVector <double> *extremum_value = new QVector <double> ();
    QVector <double> *extremum_type = new QVector <double> ();

    int sum = 0;
    int psum = 0;
    int dir = 1, pdir = 1;
    int max = 0, max_idx = 0;
    int min = 255, min_idx = 0;

    /* make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_bin->at(i).at(j);
        }
        sum_lines->append(sum);
        sum = 0;
    }

    //qDebug() << *sum_lines;

    /* locate extremums */
    for( int i = 0; i < sum_lines->size(); i++)
    {
        /* current direction */
        if( sum_lines->at(i) > psum)
            dir = 1;
        else if( sum_lines->at(i) < psum)
            dir = -1;
        else dir = 0;

        if( dir != pdir)
        {
            extremum_line->append(i);
            extremum_value->append(psum);
            extremum_type->append((dir<pdir) ? 1 : 0);
        }

        psum = sum_lines->at(i);
        pdir = dir;
    }

    //qDebug() << *extremum_line;
    //qDebug() << *extremum_value;
    //qDebug() << *extremum_type;

    /* find max from top of image */
    for( int i = 0; i < extremum_line->size(); i++)
    {
        if( (extremum_value->at(i) > max) && (extremum_type->at(i) == 1) )
        {
            max = extremum_value->at(i);
            max_idx = i;
        }
    }

    //qDebug() << max_idx << max;

    /* find lowest from previous max */
    for( int i = max_idx; i < extremum_value->size()-1; i++)
    {
        if( (extremum_value->at(i) <= min) && (extremum_type->at(i) == 0) )
        {
            min = extremum_value->at(i);
            min_idx = i;
        }
    }

    //qDebug() << min_idx << min;
    //qDebug() << extremum_line->at(min_idx);

    return extremum_line->at(min_idx);
}

void MainWindow::get_coor_extr_left_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    qDebug() << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1  ;
    *ya = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xa)
        {
            *xa = tab->at(i);
            *ya = i;
            break; //Find first bottom pixel found is OK
        }
    }

    //check upper line
    /**xa = COL_NBR - 1;
    for( int i = *ya+1; i < median_line; i++)
    {
        if( (tab->at(i) < *xa) )
        {
            *xa = tab->at(i);
            *ya = i;
        }
    }*/
    *xa = tab->at(*ya+1);
    *ya = *ya+1;

    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR LEFT FOOT \n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR - 1 ;
    *yb = LGN_NBR;
    for( int i = *ya; i < LGN_NBR; i++)
    {
        if( (tab->at(i) < *xb) )
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }

    //check upper line
    /**xb = 0 ;
    for( int i = *yb+1; i < *ya; i++)
    {
        if( (tab->at(i) > *xb) && (tab->at(i) < 15) )
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }*/

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_extr_axial_left(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    //unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    /*for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }*/

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << "sumTab" << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1  ;
    *ya = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xa)
        {
            *xa = tab->at(i);
            *ya = i;
            break; //Find first bottom pixel found is OK
        }
    }

    //*xa = (COL_NBR - 1) - *xa;

    //Check upper line
    *xa = tab->at(*ya+1);
    *ya = *ya+1;

    *xa = (COL_NBR - 1) - *xa;

    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR LEFT FOOT \n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR - 1 ;
    *yb = LGN_NBR - 1;
    for( int i = LGN_NBR - 1; i >= 0; i--)
    {
        if( (tab->at(i) < *xb) || (tab->at(i) == 0))    //Find upper position or if too much angle, thumb can be outside pad
        {
            *xb = tab->at(i);
            *yb = i;
            break;
        }
    }


    //Check down line
    *xb = tab->at(*yb-1);
    *yb = *yb-1;
    *xb = (COL_NBR - 1) - *xb;

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_right_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
            break;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    //Check upper line
    *xc = COL_NBR - 1 ;
    for( int i = *yc+1; i < median_line; i++)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR LEFT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = COL_NBR - 1 ;
    *yd = LGN_NBR;
    for( int i = median_line; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xd)
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    *xd = ( COL_NBR - 1 ) - *xd;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");

    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_left_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xa, ya
    *xa = COL_NBR - 1;
    *ya = median_line;

    for( int i = 0; i < median_line; i++)
    {
        if( tab->at(i) < *xa) //Found first pixel on bottom
        {
            *xa = tab->at(i);
            *ya = i;
            break;
        }
    }

    //check upper line
    *xa = COL_NBR - 1;
    for( int i = *ya+1; i < median_line; i++)
    {
        if( (tab->at(i) < *xa) )
        {
            *xa = tab->at(i);
            *ya = i;
        }
    }


    qDebug() << "xa = " << *xa << "ya = " << *ya;
    dataDisplay.append("LEFT FOR RIGHT FOOT\n");
    dataDisplay.append("xa = " + QString::number(*xa) + " ya = " + QString::number(*ya) + "\n");

    // top half -> xb, yb
    *xb = COL_NBR;
    *yb = median_line;
    for( int i = median_line; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xb)
        {
            *xb = tab->at(i);
            *yb = i;
        }
    }

    qDebug() << "xb = " << *xb << "yb = " << *yb;
    dataDisplay.append("xb = " + QString::number(*xb) + " yb = " + QString::number(*yb) + "\n");

    QLine line(*xa, *ya, *xb, *yb);
    m_lines.append(line);
}

void MainWindow::get_coor_extr_right_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = ( COL_NBR - 1 ); j >= 0; j--)
        {
            tab->replace(i, ( COL_NBR - 1 ) - j);
            if(matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = median_line;

    for( int i = median_line; i > 0; i--)
    {
        if( tab->at(i) < *xc)
        {
            *xc = tab->at(i);
            *yc = i;
        }
    }

    *xc = (COL_NBR - 1) - *xc;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR RIGHT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = 0 ;
    *yd = *yc;

    for( int i = 0; i < *yc; i++)
    {
        if( (tab->at(i) > *xd) && (tab->at(i) < 15))
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    //Check upperlign
    *xd = 0 ;
    for( int i = *yd+1; i < *yc; i++)
    {
        if( (tab->at(i) > *xd) && (tab->at(i) < 15))
        {
            *xd = tab->at(i);
            *yd = i;
        }
    }

    *xd = ( COL_NBR - 1 ) - *xd;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");


    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

void MainWindow::get_extr_axial_right(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd )
{
    QVector <double> *tab = new QVector <double> ();
    //unsigned int median_line = get_median_line(matrix_bin); //Median_line corresponding to the line number of thumb low position
    //qDebug() << "median_line" << median_line;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        tab->append(0);
        for(int j = 0; j < COL_NBR; j++)
        {
            tab->replace(i, j);
            if (matrix_bin->at(i).at(j) == 1)
            {
                break;
            }
        }
    }

    //qDebug() << *tab;

    // bottom half -> xc, yc
    *xc = COL_NBR - 1 ;
    *yc = 0;

    for( int i = 0; i < LGN_NBR; i++)
    {
        if( tab->at(i) < *xc)  //Find first pixel
        {
            *xc = tab->at(i);
            *yc = i;
            break;
        }
    }

    //Check upper line
    *xc = tab->at(*yc+1);
    *yc = *yc+1;

    qDebug() << "xc = " << *xc << "yc = " << *yc;
    dataDisplay.append("RIGHT FOR RIGHT FOOT\n");
    dataDisplay.append("xc = " + QString::number(*xc) + " yc = " + QString::number(*yc) + "\n");

    // top half -> xb, yb
    *xd = COL_NBR - 1 ;
    *yd = LGN_NBR - 1;

    for( int i = LGN_NBR - 1; i >= 0; i--)
    {
        if( (tab->at(i) < *xd) ) //Find upper position or if too much angle, thumb can be outside pad
        {
            *xd = tab->at(i);
            *yd = i;
            break;
        }
    }

    //Check down line
    *xd = tab->at(*yd-1);
    *yd = *yd-1;

    qDebug() << "xd = " << *xd << "yd = " << *yd;
    dataDisplay.append("xd = " + QString::number(*xd) + " yd = " + QString::number(*yd) + "\n");


    QLine line(*xc, *yc, *xd, *yd);
    m_lines.append(line);
}

unsigned int MainWindow::calc_mean(QVector <QVector <double> > *matrix)
{
    double mean = 0.0;

    for(int i = 0; i < LGN_NBR ; i++)
    {
        for(int j = 0; j < COL_NBR; j++)
        {
            mean += matrix->at(i).at(j);
        }
    }

    mean /= (double)LGN_NBR*COL_NBR;

    return mean;
}

double MainWindow::sizeGet()
{
    qDebug() << "*************************sizeGet********************************";
    int hi = 0, low = 0;
    //unsigned int xa, xb, ya, yb, xc, yc, xd, yd;
    //double  a1 = 0;
    //double left_angle, right_angle, sum_angle = 0;
    //double xy_ratio = 3.3;
    double left_size, right_size;
    double offset = 5;

    //Binarize matrix
    binarizeFromMean(&m_data_left_full, &m_data_bin_left);
    binarizeFromMean(&m_data_right_full, &m_data_bin_right);

    //Filter matrix
    filterMatrix(&m_data_left_full, &m_data_bin_left, &m_data_filter_left);
    filterMatrix(&m_data_right_full, &m_data_bin_right, &m_data_filter_right);

    emit dataReadyGravity_left(&m_data_filter_left);
    emit dataReadyGravity_right(&m_data_filter_right);


    qDebug() << "*************************leftFoot********************************";
    //LEFT FOOT
    //binarizeFromNoiseMargin(&m_data_left_full, &m_data_bin_left);
    //get_extr_axial_left(&m_data_bin_left, &xa, &ya, &xb, &yb);
    get_hilo_pos(&m_data_filter_left, &hi, &low);

    /*xa *= xy_ratio;
    xb *= xy_ratio;
    xc *= xy_ratio;
    xd *= xy_ratio;*/

    /*if( (yb - ya) != 0)
        a1 = atan(((double) xb - (double) xa) / ((double) yb - (double) ya));
    else
        a1 = 0;

    if ( abs(a1*(180/M_PI)) > 25) a1 = 0; //detection problem*/

    //left_angle =  (M_PI/2) - a1;
    qDebug() << "left_angle" << m_left_angle*(180/M_PI);
    left_size = (double(hi) - double(low))/2;
    qDebug() << "left_size" << left_size;
    left_size = left_size / sin(m_left_angle);
    qDebug() << "left_size_angle" << left_size;
    left_size = left_size + offset;
    qDebug() << "left_size_offset" << left_size;
    //dataDisplay.append("LEFT ANGLE = " + QString::number(left_angle*(180/M_PI)) + "\n");
    //dataDisplay.append("LEFT SIZE = " + QString::number(left_size) + "\n");

    qDebug() << "*************************rightFoot********************************";
    //RIGHT FOOT
    //binarizeFromNoiseMargin(&m_data_right_full, &m_data_bin_right);
    //get_coor_extr_left_for_right_foot(&m_data_bin_right, &xa, &ya, &xb, &yb);
    //get_coor_extr_right_for_right_foot(&m_data_bin_right, &xc, &yc, &xd, &yd);
    //get_extr_axial_right(&m_data_bin_right, &xa, &ya, &xb, &yb);
    get_hilo_pos(&m_data_filter_right, &hi, &low);

    /*xa *= xy_ratio;
    xb *= xy_ratio;
    xc *= xy_ratio;
    xd *= xy_ratio;*/

    /*if( (yb - ya) != 0)
        a1 = atan(((double) xa - (double) xb) / ((double) ya - (double) yb));
    else
        a1 = 0;

    if ( abs(a1*(180/M_PI)) > 25) a1 = 0; //detection problem

    right_angle = (M_PI/2)-a1;*/
    qDebug() << "right_angle" << m_right_angle*(180/M_PI);
    right_size = (double(hi) - double(low))/2;
    qDebug() << "right_size" << right_size;
    right_size = right_size / sin(m_right_angle);
    qDebug() << "right_size_angle" << right_size;
    right_size = right_size + offset;
    qDebug() << "right_size_offset" << right_size;

    //dataDisplay.append("RIGHT ANGLE = " + QString::number(right_angle*(180/M_PI)) +"\n");
    //dataDisplay.append("RIGHT SIZE = " + QString::number(right_size) +"\n");

    double pointure = (left_size + right_size ) / 2;
    //double pointure_arrondie = round( (pointure)*2)/2;

    qDebug() << "total_size" << pointure;
    //qDebug() << "rounded total_size" << pointure_arrondie;

    //dataDisplay.append("POINTURE = " + QString::number(pointure) + "\n");
    //dataDisplay.append("POINTURE ARRONDIE = " + QString::number(pointure_arrondie) + "\n");

    return pointure;
}

int MainWindow::calc_gravity(){

    binarizeFromMean(&m_data_left, &m_data_bin_left);
    binarizeFromMean(&m_data_right, &m_data_bin_right);
    filterMatrix(&m_data_left, &m_data_bin_left, &m_data_filter_left);
    filterMatrix(&m_data_right, &m_data_bin_right, &m_data_filter_right);

    point_t Aleft, Bleft, Aright, Bright;
    int err_code;
    err_code = gvtGet(&m_data_filter_left, &Aleft, &Bleft);
    if(!err_code){
        qDebug() << "ERROR gvtGet(left)";
        dataDisplay_gravity.append("ERROR POSITION\n");
        return 0;
    }
    err_code = gvtGet(&m_data_filter_right, &Aright, &Bright);
    if(!err_code){
        qDebug() << "ERROR gvtGet(right)";
        dataDisplay_gravity.append("ERROR POSITION\n");
        return 0;
    }
    qDebug() << "Aleft line = " << Aleft.line << "Aleft col = " << Aleft.col;
    qDebug() << "Bleft line = " << Bleft.line << "Bleft col = " << Bleft.col;
    qDebug() << "Aright line = " << Aright.line << "Aright col = " << Aright.col;
    qDebug() << "Bright line = " << Bright.line << "Bright col = " << Bright.col;

    int leftMedianLine = (Aleft.line + Bleft.line) / 2;
    int rightMedianLine = (Aright.line + Bright.line) / 2;
    qDebug()<< "leftMedianLine" << leftMedianLine;
    qDebug()<< "rightMedianLine" << rightMedianLine;

    long leftLowerSum = sumMatrix(&m_data_filter_left, 0, leftMedianLine);
    long leftUpperSum = sumMatrix(&m_data_filter_left, leftMedianLine, LGN_NBR);
    long rightLowerSum = sumMatrix(&m_data_filter_right, 0, rightMedianLine);
    long rightUpperSum = sumMatrix(&m_data_filter_right, rightMedianLine, LGN_NBR);

    qDebug() << "leftLowerSum = " << leftLowerSum;
    qDebug() << "leftUpperSum = " << leftUpperSum;
    qDebug() << "rightLowerSum = " << rightLowerSum;
    qDebug() << "rightUpperSum = " << rightUpperSum;

    long totalSum = leftLowerSum + leftUpperSum + rightLowerSum + rightUpperSum;
    qDebug() << "totalSum = " << totalSum;
    double gravity = (double)(leftLowerSum + rightLowerSum) / (double)totalSum;
    qDebug() << "gravity = " << gravity;

    double alpha = 2.0 / 3.0;
    double igravity = 0;
    if( gravity >= alpha)
        igravity = 0;
    else
        igravity = (uint8_t)(((alpha - gravity) / alpha) * 17);
    qDebug() << "igravity = " << igravity;

    return igravity;
}

// Function to find all the local maxima
// and minima in the given array arr[]
/*void MainWindow::findLocalMaximaMinima(int n, QVector <int> arr)
{

    // Empty vector to store points of
    // local maxima and minima
    QVector<int> mx, mn;

    // Checking whether the first point is
    // local maxima or minima or none
    if (arr[0] > arr[1])
        mx.push_back(0);

    else if (arr[0] < arr[1])
        mn.push_back(0);

    // Iterating over all points to check
    // local maxima and local minima
    for(int i = 1; i < n - 1; i++)
    {

    // Condition for local minima
    if ((arr[i - 1] > arr[i]) and
        (arr[i] < arr[i + 1]))
        mn.push_back(i);

    // Condition for local maxima
    else if ((arr[i - 1] < arr[i]) and
                (arr[i] > arr[i + 1]))
        mx.push_back(i);
    }

    // Checking whether the last point is
    // local maxima or minima or none
    if (arr[n - 1] > arr[n - 2])
        mx.push_back(n - 1);

    else if (arr[n - 1] < arr[n - 2])
        mn.push_back(n - 1);

    // Print all the local maxima and
    // local minima indexes stored
    if (mx.size() > 0)
    {
        qDebug() << "Points of Local maxima are : ";
        for(int a : mx)
        qDebug() << a << " ";
        qDebug() << endl;
    }
    else
        qDebug() << "There are no points of "
            << "Local Maxima \n";

    if (mn.size() > 0)
    {
        qDebug() << "Points of Local minima are : ";
        for(int a : mn)
        qDebug() << a << " ";
        qDebug() << endl;
    }
    else
        qDebug() << "There are no points of "
            << "Local Minima \n";
}*/
int MainWindow::checkSizeZone(line_zone_t *zx, int zoneNbr)
{
    switch(zoneNbr)
    {
    case 2:
        if(zx[0].n_lines < 6)
        {
            zx[0] = zx[1];
            zoneNbr --;
        }if(zx[1].n_lines < 6)
        {
            zoneNbr --;
        }
    break;

    case 1:
        if(zx[0].n_lines < 6)
        {
            zx[0] = zx[1];
            zoneNbr --;
        }
        break;
    }
    return zoneNbr;
}

QVector <QRect> MainWindow::findZones(QVector <QVector <double> > *matrix)
{
    qDebug() << "**************************findZone*******************************";
    QVector <QRect> zones;
    line_zone_t   zx[47];
    column_zone_t zy[15];
    QVector <int> *linSum = new QVector <int> ();
    QVector <int> *colSum = new QVector <int> ();
    int moy = 0;
    int cpt = 0;
    int val = 0;
    int index = 0;
    int nbrZones = 0;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix->at(i).at(j);
        }
        linSum->append(sum);
    }
    qDebug() << "linSum: " << *linSum;

    // mean of lines sum
    moy = 0;
    cpt = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        moy += linSum->at(i);
    }
    moy /= LGN_NBR;

    //find row zones relative to 0 (not good with V3.1 but with V3.2)
    /*for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) > 0) && (val == 0) )
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }else if( ( (linSum->at(i) == 0) || (i==LGN_NBR-1) ) && (val > 0) )
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            index ++;
        }
        val = linSum->at(i);
    }*/

    //find row zones relative to moy
    qDebug() << "*******ROW ZONES******* ";
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) >= moy) && (val <= moy) )
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }
        else if( ( (linSum->at(i) <= moy) || (i==LGN_NBR-1) ) && (val >= moy))
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            index ++;
        }
        val = linSum->at(i);
    }

    qDebug() << "found" << index << "zones";
    for( int i = 0; i < index; i++)
    {
        qDebug() << "zone X " << i << "start from " << zx[i].start_line << "to line " << zx[i].end_line;
    }

    // sort and take the two biggest zones or keep the only zone
    if(index>1){
        qsort( (void *)zx, index, sizeof(line_zone_t), compare_n_lines);
        qsort( (void *)zx, 2, sizeof(line_zone_t), compare_index);
        nbrZones = checkSizeZone(zx, 2);
    }else if (index == 0)
    {
        nbrZones = 0;
    }else
    {
        nbrZones = checkSizeZone(zx, 1);
    }

    qDebug() << "keep" << nbrZones << "zone";

    //Suivant le nombre de zones, on cherche leur largeur
    qDebug() << "*******COLUMN ZONES******* ";
    for(int i = 0; i < nbrZones; i++)
    {
        // make column sum for each columns//
        colSum->clear();
        for( int j = 0; j < COL_NBR; j++)
        {
            int sum = 0;
            for(int k = zx[i].start_line; k < zx[i].end_line; k++)
            {
                sum += matrix->at(k).at(j);
            }
            colSum->append(sum);
        }

        // mean of columns sum
        moy = 0;
        for( int j = 0; j < COL_NBR; j++)
        {
            moy += colSum->at(j);
        }
        moy /= COL_NBR;

        //find column zones
        val = index = 0;
        memset( (void *)zy, 0, sizeof(zy));
        for( int j = 0; j < COL_NBR; j++)
        {
            if( (colSum->at(j) >= moy) && (val <= moy))
            {
                zy[index].index = index;
                zy[index].start_col = j;
            }
            else if( ((colSum->at(j) <= moy)||(j == COL_NBR-1)) && (val >= moy))
            {
                zy[index].end_col = j;
                zy[index].n_col = zy[index].end_col - zy[index].start_col;
                qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
                index ++;
            }
            val = colSum->at(j);
        }

        //Count zone number
        if( index > 1)
        {
            // sort and take biggest one //
            qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
            //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
            index = 1;
        }else qDebug() << "NO COLUMN ZONE FOUNDED ON LEFT FOOT";

        qDebug() << "ZONE LEFT" << i <<"is lines [" << zx[i].start_line << "," << zx[i].end_line << "] and columns [" << zy[0].start_col << "," << zy[0].end_col << "]" ;
        int X = zy[0].start_col;
        int Y = zx[i].end_line;
        int width = zy[0].end_col - zy[0].start_col + 2;
        int height = zx[i].end_line - zx[i].start_line + 1;

        QRect zone(X,Y,width,height);
        zones.append(zone);
    }

    if(nbrZones == 0)
    {
        zones.append(QRect(0,0,0,0));
    }
    return zones;
}

void MainWindow::filterMatrix(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin, QVector <QVector <double> > *matrix_filter)
{
    matrix_filter->clear();

    for( int i = 0; i < LGN_NBR; i++)
    {
        QVector<double> foo;

        for( int j = 0; j < COL_NBR; j++)
        {

            if(matrix_bin->at(i).at(j) == 1)
               foo.append(matrix->at(i).at(j));
            else foo.append(0);
        }

        matrix_filter->append(foo);
    }

}

int MainWindow::gvtGet(QVector <QVector <double> > *matrix_filter, point_t *A, point_t *B)
{
    line_zone_t   zx[10];
    column_zone_t zy[10];
    QVector <unsigned long> *linSum = new QVector <unsigned long> ();
    QVector <unsigned long> *colSum = new QVector <unsigned long> ();
    unsigned long moy = 0;
    unsigned long val = 0;
    unsigned long index = 0;

    // make sum tab for each lines*/
    for( int i = 0; i < LGN_NBR; i++)
    {
        int sum = 0;
        for(int j = 0; j < COL_NBR; j++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        linSum->append(sum);
    }
    qDebug() << *linSum;

    // mean of lines sum
    moy = 0;
    for( int i = 0; i < LGN_NBR; i++)
    {
        moy += linSum->at(i);
    }
    moy /= LGN_NBR;
    qDebug() << moy;

    //find zones
    for( int i = 0; i < LGN_NBR; i++)
    {
        if( (linSum->at(i) >= moy) && (val <= moy))
        {
            zx[index].index = index;
            zx[index].start_line = i;
        }

        else if( ( (linSum->at(i) <= moy) || (i==LGN_NBR-1)) && (val >= moy))
        {
            zx[index].end_line = i;
            zx[index].n_lines = zx[index].end_line - zx[index].start_line;
            qDebug() << "zone X " << index << "start from " << zx[index].start_line << "to line " << zx[index].end_line;
            index ++;
        }
        val = linSum->at(i);
    }

    //Count zone number
    if( index <= 1)
    {
        qDebug() << "FOOT POSITIONEMENT PROBLEM";
        return 0;
    }
    else
    {
        /* sort and take the two biggest zones */
        qsort( (void *)zx, index, sizeof(line_zone_t), compare_n_lines);
        qsort( (void *)zx, 2, sizeof(line_zone_t), compare_index);
        index = 2;

        //qDebug()<< "zone X heel = " << zx[0].start_line << "to " << zx[0].end_line;
        //qDebug()<< "zone X toe = "  << zx[1].start_line << "to " << zx[1].end_line;

        //Zone heel
        // make column sum for each columns*/
        colSum->clear();
        for( int j = 0; j < COL_NBR; j++)
        {
            int sum = 0;
            for(int i = zx[0].start_line; i < zx[0].end_line; i++)
            {
                sum += matrix_filter->at(i).at(j);
            }
            colSum->append(sum);
        }

        // mean of columns sum
        moy = 0;
        for( int i = 0; i < COL_NBR; i++)
        {
            moy += colSum->at(i);
        }
        moy /= COL_NBR;
        //qDebug() << moy;

        //find zones
        val = index = 0;
        memset( (void *)zy, 0, sizeof(zy));
        for( int i = 0; i < COL_NBR; i++)
        {
            if( (colSum->at(i) >= moy) && (val <= moy))
            {
                zy[index].index = index;
                zy[index].start_col = i;
            }
            else if( ((colSum->at(i) <= moy)||(i == COL_NBR-1)) && (val >= moy))
            {
                zy[index].end_col = i;
                zy[index].n_col = zy[index].end_col - zy[index].start_col;
                //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
                index ++;
            }
            val = colSum->at(i);
        }

        //Count zone number
        if( index > 1)
        {
            /* sort and take biggest one */
            qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
            //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
            index = 1;
        }
        else if( !index)
        {
            //Problem of positionnement
        }

        A->line = (zx[0].start_line + zx[0].end_line) / 2;
        A->col  = (zy[0].start_col + zy[0].end_col) / 2;

        //qDebug() << "A line " << A->line << "A col " << A->col;

        //Zone toe
        // make column sum for each columns
        colSum->clear();
        for( int j = 0; j < COL_NBR; j++)
        {
            int sum = 0;
            for(int i = zx[1].start_line; i < zx[1].end_line; i++)
            {
                sum += matrix_filter->at(i).at(j);
            }
            colSum->append(sum);
        }

        // mean of columns sum
        moy = 0;
        for( int i = 0; i < COL_NBR; i++)
        {
            moy += colSum->at(i);
        }
        moy /= COL_NBR;
        //qDebug() << moy;

        //find zones
        val = index = 0;
        memset( (void *)zy, 0, sizeof(zy));
        for( int i = 0; i < COL_NBR; i++)
        {
            if( (colSum->at(i) >= moy) && (val <= moy))
            {
                zy[index].index = index;
                zy[index].start_col = i;
            }
            else if( ((colSum->at(i) <= moy)||(i == COL_NBR-1)) && (val >= moy))
            {
                zy[index].end_col = i;
                zy[index].n_col = zy[index].end_col - zy[index].start_col;
                //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
                index ++;
            }
            val = colSum->at(i);
        }

        //Count zone number
        if( index > 1)
        {
            /* sort and take biggest one */
            qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
            //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
            index = 1;
        }
        else if( !index)
        {
            //Problem of positionnement
        }

        B->line = (zx[1].start_line + zx[1].end_line) / 2;
        B->col  = (zy[0].start_col + zy[0].end_col) / 2;

        //qDebug() << "B line " << B->line << "B col " << B->col;

        return 1;
    }

    //qDebug()<< "zone X heel = " << zx[0].start_line << "to " << zx[0].end_line;
    //qDebug()<< "zone X toe = "  << zx[1].start_line << "to " << zx[1].end_line;

    //Zone heel
    // make column sum for each columns*/
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[0].start_line; i < zx[0].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;
    //qDebug() << moy;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) >= moy) && (val <= moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        else if( ((colSum->at(i) <= moy)||(i == COL_NBR-1)) && (val >= moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    A->line = (zx[0].start_line + zx[0].end_line) / 2;
    A->col  = (zy[0].start_col + zy[0].end_col) / 2;

    //qDebug() << "A line " << A->line << "A col " << A->col;

    //Zone toe
    // make column sum for each columns
    colSum->clear();
    for( int j = 0; j < COL_NBR; j++)
    {
        int sum = 0;
        for(int i = zx[1].start_line; i < zx[1].end_line; i++)
        {
            sum += matrix_filter->at(i).at(j);
        }
        colSum->append(sum);
    }

    // mean of columns sum
    moy = 0;
    for( int i = 0; i < COL_NBR; i++)
    {
        moy += colSum->at(i);
    }
    moy /= COL_NBR;
    //qDebug() << moy;

    //find zones
    val = index = 0;
    memset( (void *)zy, 0, sizeof(zy));
    for( int i = 0; i < COL_NBR; i++)
    {
        if( (colSum->at(i) >= moy) && (val <= moy))
        {
            zy[index].index = index;
            zy[index].start_col = i;
        }
        else if( ((colSum->at(i) <= moy)||(i == COL_NBR-1)) && (val >= moy))
        {
            zy[index].end_col = i;
            zy[index].n_col = zy[index].end_col - zy[index].start_col;
            //qDebug() << "zone Y " << index << "start from " << zy[index].start_col << "to line " << zy[index].end_col;
            index ++;
        }
        val = colSum->at(i);
    }

    //Count zone number
    if( index > 1)
    {
        /* sort and take biggest one */
        qsort( (void *)zy, index, sizeof(column_zone_t), compare_n_cols);
        //qsort( (void *)zy, index, sizeof(column_zone_t), compare_index);
        index = 1;
    }
    else if( !index)
    {
        //Problem of positionnement
    }

    B->line = (zx[1].start_line + zx[1].end_line) / 2;
    B->col  = (zy[0].start_col + zy[0].end_col) / 2;

    //qDebug() << "B line " << B->line << "B col " << B->col;

    return 1;
}

void MainWindow::get_hilo_pos(QVector <QVector <double> > *matrix, int *hi, int *low)
{
    //int noiseMargin = m_popupwindow->getNoiseMargin();
    //qDebug() << noiseMargin;
    noiseMargin = 0;
    bool found = false;
    /* find low position */
    for( int i = 0; i < LGN_NBR; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
            if (found == false)
            {
                if (matrix->at(i).at(j) > noiseMargin)
                {
                    found = true;
                    *low = i;
                    break;
                }
            }
            else
                break;
        }
    }

    found = false;
    /* find hi position */
    for( int i = LGN_NBR-1; i >= 0; i--)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
            if (found == false)
            {
                if (matrix->at(i).at(j) > noiseMargin)
                {
                    found = true;
                    *hi = i;
                    break;
                }
            }
            else
                break;
        }
    }

    qDebug() << "hi = " << *hi << "low = " << *low;
}

//! [8]
void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}
//! [8]

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_console, &Console::clear);
    connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}

int MainWindow::compare_n_lines(const void *a, const void *b)
{
    struct line_zone_t{
        int 	index;
        int		start_line;
        int 	end_line;
        int		n_lines;
    };

    uint8_t	n_line_a = ((line_zone_t *)a)->n_lines;
    uint8_t	n_line_b = ((line_zone_t *)b)->n_lines;

    return (n_line_a < n_line_b) - (n_line_a > n_line_b);

}

int MainWindow::compare_index( const void *a, const void *b)
{
    struct line_zone_t{
        int 	index;
        int		start_line;
        int 	end_line;
        int		n_lines;
    };

    uint8_t index_a = ((line_zone_t *)a)->index;
    uint8_t index_b = ((line_zone_t *)b)->index;
    return (index_a > index_b) - (index_a < index_b);
}

int MainWindow::compare_n_cols( const void *a, const void *b)
{
    struct column_zone_t{
        int 	index;
        int		start_col;
        int 	end_col;
        int		n_col;
    };

    uint8_t n_col_a = ((column_zone_t *)a)->n_col;
    uint8_t n_col_b = ((column_zone_t *)b)->n_col;
    return (n_col_a < n_col_b) - (n_col_a > n_col_b);
}

long MainWindow::sumMatrix(QVector <QVector <double> > *matrix, int startLine, int endLine){

    long sum = 0;

    for(int i = startLine; i < endLine; i++)
    {
        for( int j = 0; j < COL_NBR; j++)
        {
               sum += matrix->at(i).at(j);
        }

    }

    return sum;
}
