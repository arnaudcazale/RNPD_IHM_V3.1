#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QTextEdit>
#include <QString>

QT_BEGIN_NAMESPACE

namespace Ui {
class ResultWindow;
}

class Plot;

QT_END_NAMESPACE

class ResultWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit ResultWindow(QWidget *parent = nullptr);
        ~ResultWindow();

        void display(QString text);

    protected:


    public slots:
        void dataUpdate_left(QVector<QVector <double> > *dataPacket);
        void dataUpdate_right(QVector<QVector <double> > *dataPacket);


    private:
        Plot *d_plot_left;
        Plot *d_plot_right;
        QTextEdit *m_text;
};

#endif // RESULTWINDOW_H
