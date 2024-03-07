/****************************************************************************
** Meta object code from reading C++ file 'emxMessageHandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../LicenseManager/emxMessageHandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'emxMessageHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_emxMessageHandler_t {
    QByteArrayData data[12];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_emxMessageHandler_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_emxMessageHandler_t qt_meta_stringdata_emxMessageHandler = {
    {
QT_MOC_LITERAL(0, 0, 17), // "emxMessageHandler"
QT_MOC_LITERAL(1, 18, 10), // "messageBox"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 4), // "icon"
QT_MOC_LITERAL(4, 35, 5), // "title"
QT_MOC_LITERAL(5, 41, 4), // "text"
QT_MOC_LITERAL(6, 46, 4), // "int*"
QT_MOC_LITERAL(7, 51, 3), // "ret"
QT_MOC_LITERAL(8, 55, 28), // "QMessageBox::StandardButtons"
QT_MOC_LITERAL(9, 84, 7), // "buttons"
QT_MOC_LITERAL(10, 92, 27), // "QMessageBox::StandardButton"
QT_MOC_LITERAL(11, 120, 13) // "defaultButton"

    },
    "emxMessageHandler\0messageBox\0\0icon\0"
    "title\0text\0int*\0ret\0QMessageBox::StandardButtons\0"
    "buttons\0QMessageBox::StandardButton\0"
    "defaultButton"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_emxMessageHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    6,   29,    2, 0x0a /* Public */,
       1,    5,   42,    2, 0x2a /* Public | MethodCloned */,
       1,    4,   53,    2, 0x2a /* Public | MethodCloned */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6, 0x80000000 | 8, 0x80000000 | 10,    3,    4,    5,    7,    9,   11,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6, 0x80000000 | 8,    3,    4,    5,    7,    9,
    QMetaType::Void, QMetaType::Int, QMetaType::QString, QMetaType::QString, 0x80000000 | 6,    3,    4,    5,    7,

       0        // eod
};

void emxMessageHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        emxMessageHandler *_t = static_cast<emxMessageHandler *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->messageBox((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QMessageBox::StandardButtons(*)>(_a[5])),(*reinterpret_cast< QMessageBox::StandardButton(*)>(_a[6]))); break;
        case 1: _t->messageBox((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4])),(*reinterpret_cast< QMessageBox::StandardButtons(*)>(_a[5]))); break;
        case 2: _t->messageBox((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< int*(*)>(_a[4]))); break;
        default: ;
        }
    }
}

const QMetaObject emxMessageHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_emxMessageHandler.data,
      qt_meta_data_emxMessageHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *emxMessageHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *emxMessageHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_emxMessageHandler.stringdata0))
        return static_cast<void*>(const_cast< emxMessageHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int emxMessageHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
