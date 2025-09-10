// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DCORE_QHANDLE_P_H
#define QT3DCORE_QHANDLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QDebug>
#include <QtCore/qhashfunctions.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

template <typename T>
class QHandle
{
public:
    struct Data {
        union {
            quintptr counter;
            Data *nextFree;
        };
    };
    QHandle()
        : d(nullptr),
          counter(0)
    {}
    QHandle(Data *d)
        : d(d),
          counter(d->counter)
    {
    }
    QHandle(const QHandle &other)
        : d(other.d),
          counter(other.counter)
    {
    }
    QHandle &operator=(const QHandle &other)
    {
        d = other.d;
        counter = other.counter;
        return *this;
    }

    inline T *operator->() const;
    T *data() const;

    quintptr handle() const { return reinterpret_cast<quintptr>(d); }
    bool isNull() const { return !d; }

    Data *data_ptr() const { return d; }

    bool operator==(const QHandle &other) const { return d == other.d && counter == other.counter; }
    bool operator!=(const QHandle &other) const { return !operator==(other); }
private:
    Data *d;
    quintptr counter;
};


template <typename T>
QDebug operator<<(QDebug dbg, const QHandle<T> &h)
{
    QDebugStateSaver saver(dbg);
    QString binNumber = QString::number(h.handle(), 2).rightJustified(32, QChar::fromLatin1('0'));
    dbg.nospace() << " m_handle = " << h.handle()
                  << " = " << binNumber;
    return dbg;
}

template <typename T>
size_t qHash(const QHandle<T> &h, size_t seed)
{
    using QT_PREPEND_NAMESPACE(qHash);
    return qHash(h.handle(), seed);
}

template <typename T>
size_t qHash(const QHandle<T> &h)
{
    using QT_PREPEND_NAMESPACE(qHash);
    return qHash(h.handle());
}

} // Qt3DCore

// simpler than fighting the Q_DECLARE_TYPEINFO macro, use QString as a dummy to get movable semantics
template <typename T>
class QTypeInfo<Qt3DCore::QHandle<T> >
    : public QTypeInfoMerger<Qt3DCore::QHandle<T>, QString> {};

QT_END_NAMESPACE

#endif // QT3DCORE_QRHANDLE_H
