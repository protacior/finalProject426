/****************************************************************************
** Meta object code from reading C++ file 'main.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TextEdit[] = {
    "TextEdit\0\0returnPressed()\0"
};

void TextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEdit *_t = static_cast<TextEdit *>(_o);
        switch (_id) {
        case 0: _t->returnPressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEdit::staticMetaObject = {
    { &QTextEdit::staticMetaObject, qt_meta_stringdata_TextEdit,
      qt_meta_data_TextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEdit))
        return static_cast<void*>(const_cast< TextEdit*>(this));
    return QTextEdit::qt_metacast(_clname);
}

int TextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void TextEdit::returnPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_PrivateMessage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      30,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      50,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrivateMessage[] = {
    "PrivateMessage\0\0closeWindow()\0"
    "sendPM(QVariantMap)\0gotReturn()\0"
};

void PrivateMessage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrivateMessage *_t = static_cast<PrivateMessage *>(_o);
        switch (_id) {
        case 0: _t->closeWindow(); break;
        case 1: _t->sendPM((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 2: _t->gotReturn(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PrivateMessage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrivateMessage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PrivateMessage,
      qt_meta_data_PrivateMessage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrivateMessage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrivateMessage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrivateMessage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrivateMessage))
        return static_cast<void*>(const_cast< PrivateMessage*>(this));
    return QWidget::qt_metacast(_clname);
}

int PrivateMessage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void PrivateMessage::closeWindow()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void PrivateMessage::sendPM(QVariantMap _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
static const uint qt_meta_data_FileSharing[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   38,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FileSharing[] = {
    "FileSharing\0\0shareFiles(FileSharing*)\0"
    "fileList\0gotFilesSelected(QStringList)\0"
};

void FileSharing::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FileSharing *_t = static_cast<FileSharing *>(_o);
        switch (_id) {
        case 0: _t->shareFiles((*reinterpret_cast< FileSharing*(*)>(_a[1]))); break;
        case 1: _t->gotFilesSelected((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FileSharing::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FileSharing::staticMetaObject = {
    { &QFileDialog::staticMetaObject, qt_meta_stringdata_FileSharing,
      qt_meta_data_FileSharing, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileSharing::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileSharing::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileSharing::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileSharing))
        return static_cast<void*>(const_cast< FileSharing*>(this));
    return QFileDialog::qt_metacast(_clname);
}

int FileSharing::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFileDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void FileSharing::shareFiles(FileSharing * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_NetSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x0a,
      24,   10,   10,   10, 0x0a,
      44,   10,   10,   10, 0x0a,
      71,   62,   10,   10, 0x0a,
      91,   10,   10,   10, 0x0a,
     105,  101,   10,   10, 0x0a,
     134,  128,   10,   10, 0x0a,
     167,  162,   10,   10, 0x0a,
     227,   10,   10,   10, 0x0a,
     243,  162,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetSocket[] = {
    "NetSocket\0\0gotTimeout()\0gotEntropyTimeout()\0"
    "gotRouteTimeout()\0hostInfo\0"
    "lookedUp(QHostInfo)\0resetOL()\0msg\0"
    "gotSendPM(QVariantMap)\0share\0"
    "gotShareFiles(FileSharing*)\0pair\0"
    "gotReqToDownload(QPair<QString,QPair<QByteArray,QString> >)\0"
    "gotRetransmit()\0"
    "gotStartSearchFor(QPair<QString,quint32>)\0"
};

void NetSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetSocket *_t = static_cast<NetSocket *>(_o);
        switch (_id) {
        case 0: _t->gotTimeout(); break;
        case 1: _t->gotEntropyTimeout(); break;
        case 2: _t->gotRouteTimeout(); break;
        case 3: _t->lookedUp((*reinterpret_cast< QHostInfo(*)>(_a[1]))); break;
        case 4: _t->resetOL(); break;
        case 5: _t->gotSendPM((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 6: _t->gotShareFiles((*reinterpret_cast< FileSharing*(*)>(_a[1]))); break;
        case 7: _t->gotReqToDownload((*reinterpret_cast< QPair<QString,QPair<QByteArray,QString> >(*)>(_a[1]))); break;
        case 8: _t->gotRetransmit(); break;
        case 9: _t->gotStartSearchFor((*reinterpret_cast< QPair<QString,quint32>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetSocket::staticMetaObject = {
    { &QUdpSocket::staticMetaObject, qt_meta_stringdata_NetSocket,
      qt_meta_data_NetSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetSocket))
        return static_cast<void*>(const_cast< NetSocket*>(this));
    return QUdpSocket::qt_metacast(_clname);
}

int NetSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QUdpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
static const uint qt_meta_data_ChatDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x05,
      69,   11,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     108,   11,   11,   11, 0x0a,
     127,   11,   11,   11, 0x0a,
     154,  142,   11,   11, 0x0a,
     183,   11,   11,   11, 0x0a,
     208,  193,   11,   11, 0x0a,
     252,  245,   11,   11, 0x0a,
     275,   11,   11,   11, 0x0a,
     287,   11,   11,   11, 0x0a,
     309,  304,   11,   11, 0x0a,
     352,   11,   11,   11, 0x0a,
     373,  369,   11,   11, 0x0a,
     403,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatDialog[] = {
    "ChatDialog\0\0"
    "reqToDownload(QPair<QString,QPair<QByteArray,QString> >)\0"
    "startSearchFor(QPair<QString,quint32>)\0"
    "gotReturnPressed()\0gotPortInput()\0"
    "sender,text\0displayText(QString,QString)\0"
    "readMsg()\0msg,senderPeer\0"
    "processMsgOrRoute(QVariantMap,Peer*)\0"
    "origin\0newPrivateMsg(QString)\0shareFile()\0"
    "gotDownloadReq()\0item\0"
    "gotDownloadReqFromSearch(QListWidgetItem*)\0"
    "gotSearchInput()\0msg\0processSearchRep(QVariantMap)\0"
    "increaseBudget()\0"
};

void ChatDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatDialog *_t = static_cast<ChatDialog *>(_o);
        switch (_id) {
        case 0: _t->reqToDownload((*reinterpret_cast< QPair<QString,QPair<QByteArray,QString> >(*)>(_a[1]))); break;
        case 1: _t->startSearchFor((*reinterpret_cast< QPair<QString,quint32>(*)>(_a[1]))); break;
        case 2: _t->gotReturnPressed(); break;
        case 3: _t->gotPortInput(); break;
        case 4: _t->displayText((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 5: _t->readMsg(); break;
        case 6: _t->processMsgOrRoute((*reinterpret_cast< QVariantMap(*)>(_a[1])),(*reinterpret_cast< Peer*(*)>(_a[2]))); break;
        case 7: _t->newPrivateMsg((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->shareFile(); break;
        case 9: _t->gotDownloadReq(); break;
        case 10: _t->gotDownloadReqFromSearch((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 11: _t->gotSearchInput(); break;
        case 12: _t->processSearchRep((*reinterpret_cast< QVariantMap(*)>(_a[1]))); break;
        case 13: _t->increaseBudget(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChatDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChatDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ChatDialog,
      qt_meta_data_ChatDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChatDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChatDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChatDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChatDialog))
        return static_cast<void*>(const_cast< ChatDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ChatDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void ChatDialog::reqToDownload(QPair<QString,QPair<QByteArray,QString> > _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ChatDialog::startSearchFor(QPair<QString,quint32> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
