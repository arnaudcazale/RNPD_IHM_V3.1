/****************************************************************************
** Meta object code from reading C++ file 'plot.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../plot.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QVector>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Plot_t {
    QByteArrayData data[22];
    char stringdata0[240];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Plot_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Plot_t qt_meta_stringdata_Plot = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Plot"
QT_MOC_LITERAL(1, 5, 10), // "changeAlgo"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 2), // "on"
QT_MOC_LITERAL(4, 20, 15), // "showSpectrogram"
QT_MOC_LITERAL(5, 36, 11), // "setColorMap"
QT_MOC_LITERAL(6, 48, 8), // "setAlpha"
QT_MOC_LITERAL(7, 57, 13), // "setMatrixData"
QT_MOC_LITERAL(8, 71, 26), // "QVector<QVector<double> >*"
QT_MOC_LITERAL(9, 98, 15), // "setResampleMode"
QT_MOC_LITERAL(10, 114, 4), // "mode"
QT_MOC_LITERAL(11, 119, 11), // "updateScale"
QT_MOC_LITERAL(12, 131, 3), // "max"
QT_MOC_LITERAL(13, 135, 11), // "cancelNoise"
QT_MOC_LITERAL(14, 147, 8), // "drawLine"
QT_MOC_LITERAL(15, 156, 14), // "QVector<QLine>"
QT_MOC_LITERAL(16, 171, 5), // "lines"
QT_MOC_LITERAL(17, 177, 9), // "drawPoint"
QT_MOC_LITERAL(18, 187, 15), // "QVector<QPoint>"
QT_MOC_LITERAL(19, 203, 6), // "points"
QT_MOC_LITERAL(20, 210, 17), // "updateNoiseMargin"
QT_MOC_LITERAL(21, 228, 11) // "noiseMargin"

    },
    "Plot\0changeAlgo\0\0on\0showSpectrogram\0"
    "setColorMap\0setAlpha\0setMatrixData\0"
    "QVector<QVector<double> >*\0setResampleMode\0"
    "mode\0updateScale\0max\0cancelNoise\0"
    "drawLine\0QVector<QLine>\0lines\0drawPoint\0"
    "QVector<QPoint>\0points\0updateNoiseMargin\0"
    "noiseMargin"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Plot[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x0a /* Public */,
       4,    1,   72,    2, 0x0a /* Public */,
       5,    1,   75,    2, 0x0a /* Public */,
       6,    1,   78,    2, 0x0a /* Public */,
       7,    1,   81,    2, 0x0a /* Public */,
       9,    1,   84,    2, 0x0a /* Public */,
      11,    1,   87,    2, 0x0a /* Public */,
      13,    1,   90,    2, 0x0a /* Public */,
      14,    1,   93,    2, 0x0a /* Public */,
      17,    1,   96,    2, 0x0a /* Public */,
      20,    1,   99,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, 0x80000000 | 8,    2,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, QMetaType::Int,   12,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, 0x80000000 | 15,   16,
    QMetaType::Void, 0x80000000 | 18,   19,
    QMetaType::Void, QMetaType::Int,   21,

       0        // eod
};

void Plot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Plot *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeAlgo((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->showSpectrogram((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->setColorMap((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setAlpha((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->setMatrixData((*reinterpret_cast< QVector<QVector<double> >*(*)>(_a[1]))); break;
        case 5: _t->setResampleMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->updateScale((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->cancelNoise((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->drawLine((*reinterpret_cast< QVector<QLine>(*)>(_a[1]))); break;
        case 9: _t->drawPoint((*reinterpret_cast< QVector<QPoint>(*)>(_a[1]))); break;
        case 10: _t->updateNoiseMargin((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 8:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QLine> >(); break;
            }
            break;
        case 9:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QVector<QPoint> >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Plot::staticMetaObject = { {
    &QwtPlot::staticMetaObject,
    qt_meta_stringdata_Plot.data,
    qt_meta_data_Plot,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Plot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Plot::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Plot.stringdata0))
        return static_cast<void*>(this);
    return QwtPlot::qt_metacast(_clname);
}

int Plot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
