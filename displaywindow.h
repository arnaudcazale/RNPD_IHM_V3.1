#ifndef DISPLAYWINDOWS_H
#define DISPLAYWINDOWS_H

#include <QPlainTextEdit>

class DisplayWindow: public QPlainTextEdit
{
public:
    DisplayWindow();

    void putData(QString &data);

private:

};

#endif // DISPLAYWINDOWS_H
