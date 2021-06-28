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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPort>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;
class PopupWindow;
class DisplayWindow;
class Sequencer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    struct point_t{
        int line;
        int col;
    };

    struct line_zone_t{
        int 	index;
        int		start_line;
        int 	end_line;
        int		n_lines;
    };

    struct column_zone_t{
        int 	index;
        int		start_col;
        int 	end_col;
        int		n_col;
    };


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void dataReady_left(QVector<QVector <double> > *);
    void dataReady_right(QVector <QVector <double> > *);
    void dataReadyGravity_line(QVector <QLine> );
    void dataReady_point(QPoint, QPoint);
    void dataReady_line(QLine, QLine );
    void dataReady_zone(QVector <QRect>, QVector <QRect> );
    void dataReadyGravity_left(QVector<QVector <double> > *);
    void dataReadyGravity_right(QVector <QVector <double> > *);

private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);

private:
    void initActionsConnections();
    void splitData();
    void splitDataFillZero();
    void fillLeftDataMeanNeightboorhood();
    void fillRightDataMeanNeightboorhood();
    void binarizeFromNoiseMargin(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin);
    void binarizeFromMean(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin);
    unsigned int get_median_line(QVector <QVector <double> > *matrix_bin);
    unsigned int calc_mean(QVector <QVector <double> > *matrix);
    double sizeGet(void);
    int calc_gravity(void);
    void get_hilo_pos(QVector <QVector <double> > *matrix, int *hi, int *low);
    void get_coor_extr_left_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb);
    void get_coor_extr_right_for_left_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd);
    void get_coor_extr_left_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb);
    void get_coor_extr_right_for_right_foot(QVector <QVector <double> > *matrix_bin, unsigned int *xc, unsigned int *yc, unsigned int *xd, unsigned int *yd);
    void get_extr_axial_left(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb);
    void get_extr_axial_right(QVector <QVector <double> > *matrix_bin, unsigned int *xa, unsigned int *ya, unsigned int *xb, unsigned int *yb);
    void filterMatrix(QVector <QVector <double> > *matrix, QVector <QVector <double> > *matrix_bin, QVector <QVector <double> > *matrix_filtered);
    int gvtGet(QVector <QVector <double> > *matrix_filtered, point_t *A, point_t *B);
    void findLocalMaximaMinima(int n, QVector <int> arr);
    static int compare_n_lines(const void *a, const void *b);
    static int compare_index(const void *a, const void *b);
    static int compare_n_cols( const void *a, const void *b);
    static long sumMatrix(QVector <QVector <double> > *matrix, int startLine, int endLine);
    void computeSize();
    void pronationGet();
    QVector <QRect> findZones(QVector <QVector <double> > *matrix);
    QVector <long> gravityGet(QVector <QVector <double> > *matrix, QVector <QRect> zones);
    double gravityCompute(QVector <long> sumLeft, QVector <long> sumRight);
    QPoint barycentreGet(QVector <QVector <double> > *matrix, QVector <QRect> zones);
    QLine neutralLineGet(QVector <QVector <double> > *matrix, QVector <QRect> zones);
    double deviationGet(QLine neutralLine, QPoint barycentrePoint, bool isLeft);
    QString pronationDisplay(double deviationMean);
    int checkSizeZone(line_zone_t *zx, int zoneNbr);
    QVector<int> getRowSum(int startLine, int endLine, QVector <QVector <double> > *matrix);
    int getLargestRowIdx(int startLine, int endLine, QVector<int> *rowSum, bool isUp);
    double getCenterColIdx(int largestRowIdx, QVector <QVector <double> > *matrix);
    int getRNPDMean(QVector <unsigned char> *, int);
    void getMeasure(void);
    void accumulate(QVector <QVector <double> > *, QVector <QVector <double> > *);
    void storeHeelData(void);
    void storeToeData(void);
    void resetAccumulateVector();
    void msleep(int msec);

private:
    void showStatusMessage(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_console = nullptr;
    SettingsDialog *m_settings = nullptr;
    PopupWindow *m_popupwindow = nullptr;
    PopupWindow *m_popupwindowGravity = nullptr;
    DisplayWindow *m_display = nullptr;
    DisplayWindow *m_display_pointure = nullptr;
    DisplayWindow *m_display_gravity = nullptr;
    QSerialPort *m_serial = nullptr;
    QVector<quint8> *m_data = nullptr;
    Sequencer *m_sequencer = nullptr;
    QMediaPlayer *m_player = nullptr;

    QVector <QVector <double> > m_data_left;
    QVector <QVector <double> > m_data_right;
    QVector <QVector <double> > m_data_bin_left;
    QVector <QVector <double> > m_data_bin_right;
    QVector <QVector <double> > m_data_filter_left;
    QVector <QVector <double> > m_data_filter_right;
    QVector <QVector <double> > m_data_left_heel;
    QVector <QVector <double> > m_data_right_heel;
    QVector <QVector <double> > m_data_left_toe;
    QVector <QVector <double> > m_data_right_toe;
    QVector <QVector <double> > m_data_left_full;
    QVector <QVector <double> > m_data_right_full;
    QVector <QVector <double> > m_data_left_buff;
    QVector <QVector <double> > m_data_right_buff;

    QString dataDisplay;
    QString dataDisplay_pointure;
    QString dataDisplay_gravity;
    QVector <QLine> m_lines;
    QVector <QLine> m_linesGravity;
    QVector <double> m_pointure;

    int m_count;
    bool filling;
    bool gravity;
    bool size;
    bool m_presence = false;
    static const int LGN_NBR = 96;
    static const int COL_NBR = 16;
    static const unsigned int START_MARKER = 0xFFFE;
    static const unsigned int STOP_MARKER = 0xFFFF;
    static const int TOTAL_PIXELS_PER_SIDE = 768;
    int noiseMargin = 10;
    int m_count_measure = 0;
    bool m_wait_data = false;

};

#endif // MAINWINDOW_H
