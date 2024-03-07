/****************************************************************************
** Meta object code from reading C++ file 'QtLicenseManager.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LicenseManager/QtLicenseManager.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QtLicenseManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_LicenseCheckerThread_t {
    QByteArrayData data[13];
    char stringdata0[142];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LicenseCheckerThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LicenseCheckerThread_t qt_meta_stringdata_LicenseCheckerThread = {
    {
QT_MOC_LITERAL(0, 0, 20), // "LicenseCheckerThread"
QT_MOC_LITERAL(1, 21, 7), // "message"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "icon"
QT_MOC_LITERAL(4, 35, 5), // "title"
QT_MOC_LITERAL(5, 41, 4), // "text"
QT_MOC_LITERAL(6, 46, 4), // "int*"
QT_MOC_LITERAL(7, 51, 3), // "ret"
QT_MOC_LITERAL(8, 55, 28), // "QMessageBox::StandardButtons"
QT_MOC_LITERAL(9, 84, 7), // "buttons"
QT_MOC_LITERAL(10, 92, 27), // "QMessageBox::StandardButton"
QT_MOC_LITERAL(11, 120, 13), // "defaultButton"
QT_MOC_LITERAL(12, 134, 7) // "exitApp"

    },
    "LicenseCheckerThread\0message\0\0icon\0"
    "title\0text\0int*\0ret\0QMessageBox::StandardButtons\0"
    "buttons\0QMessageBox::StandardButton\0"
    "defaultButton\0exitApp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LicenseCheckerThread[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    6,   34,    2, 0x06 /* Public */,
       1,    5,   47,    2, 0x26 /* Public | MethodCloned */,
       1,    4,   58,    2, 0x26 /* Public | MethodCloned */,
      12,    0,   67,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6, 0x80000000 | 8, 0x80000000 | 10,    3,    4,    5,    7,    9,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6, 0x80000000 | 8,    3,    4,    5,    7,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6,    3,    4,    5,    7,
    QMetaType::Void,

       0        // eod
};

void LicenseCheckerThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        LicenseCheckerThread *_t = static_cast<LicenseCheckerThread *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QMessageBox::StandardButtons(*)>(_a[5])),(*reinterpret_cast< QMessageBox::StandardButton(*)>(_a[6]))); break;
        case 1: _t->message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QMessageBox::StandardButtons(*)>(_a[5]))); break;
        case 2: _t->message((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4]))); break;
        case 3: _t->exitApp(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (LicenseCheckerThread::*_t)(int , const QString & , const QString & , int * , QMessageBox::StandardButtons , QMessageBox::StandardButton );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LicenseCheckerThread::message)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (LicenseCheckerThread::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&LicenseCheckerThread::exitApp)) {
                *result = 3;
                return;
            }
        }
    }
}

const QMetaObject LicenseCheckerThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_LicenseCheckerThread.data,
      qt_meta_data_LicenseCheckerThread,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *LicenseCheckerThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LicenseCheckerThread::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_LicenseCheckerThread.stringdata0))
        return static_cast<void*>(const_cast< LicenseCheckerThread*>(this));
    return QThread::qt_metacast(_clname);
}

int LicenseCheckerThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void LicenseCheckerThread::message(int _t1, const QString & _t2, const QString & _t3, int * _t4, QMessageBox::StandardButtons _t5, QMessageBox::StandardButton _t6)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 3
void LicenseCheckerThread::exitApp()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
