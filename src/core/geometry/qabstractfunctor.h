/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
******************************************************************************/

#ifndef QT3DCORE_QABSTRACTFUNCTOR_H
#define QT3DCORE_QABSTRACTFUNCTOR_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

// This will generate a unique id() function per type
// <=> 1 unique function address per type
template<class T>
struct FunctorType
{
    static qintptr id()
    {
        // The MSVC linker can under some cases optimize all the template
        // functions into a single function. The code below is there to ensure
        // that the linker won't collapse all these distincts functions into one
        static T *t = nullptr;
        return reinterpret_cast<qintptr>(t);
    }
};

template<class T>
qintptr functorTypeId()
{
    return reinterpret_cast<qintptr>(&FunctorType<T>::id);
}

#define QT3D_FUNCTOR(Class)                     \
   qintptr id() const override {         \
        return Qt3DCore::functorTypeId<Class>();    \
   }


class Q_3DCORESHARED_EXPORT QAbstractFunctor
{
public:
    QAbstractFunctor() = default;
    virtual ~QAbstractFunctor();
    virtual qintptr id() const = 0;

    // TODO: Remove when moving a copy of this to Qt3DCore
    template<class T>
    const T *functor_cast(const QAbstractFunctor *other) const
    {
        if (other->id() == functorTypeId<T>())
            return static_cast<const T *>(other);
        return nullptr;
    }
private:
    Q_DISABLE_COPY(QAbstractFunctor)
};

template<class T>
const T *functor_cast(const QAbstractFunctor *other)
{
    if (other->id() == functorTypeId<T>())
        return static_cast<const T *>(other);
    return nullptr;
}

} // Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTFUNCTOR_H
