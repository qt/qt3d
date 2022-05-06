/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#include "qabstractfunctor.h"

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
    \class Qt3DCore::QAbstractFunctor
    \inmodule Qt3DCore
    \since 5.7
    \brief QAbstractFunctor is an abstract base class for all functors.

    The QAbstractFunctor is used as a base class for all functors and data
    generators in Qt3DCore module.

    When user defines a new functor or generator, they need to implement the
    \l QAbstractFunctor::id() method, which should be done using the \c {QT3D_FUNCTOR}
    macro in the class definition.
 */
/*!
    \fn qintptr Qt3DCore::QAbstractFunctor::id() const

    Returns a pointer to the id of the functor.
 */
/*!
   \fn qintptr Qt3DCore::functorTypeId()

   Returns a pointer to the type id of the functor.
*/
/*!
    \macro QT3D_FUNCTOR(Class)
    \relates Qt3DCore::QAbstractFunctor

    This macro assigns functor id to the \a Class, which is used by QAbstractFunctor::functor_cast
    to determine if the cast can be done.
 */

/*!
    \fn template<class T> const T * Qt3DCore::QAbstractFunctor::functor_cast(const QAbstractFunctor *other) const

    This method is used to cast functor \a other to type T if the other is of
    type T (or of subclass); otherwise returns 0. This method works similarly
    to \l [QtCore] {qobject_cast(const QObject *object)}{qobject_cast()},
    except with functors derived from QAbstractFunctor.

    \warning If T was not declared with \l QT3D_FUNCTOR macro, then the results are undefined.
  */

/*! Desctructor */
QAbstractFunctor::~QAbstractFunctor()
{

}

} // Qt3D

QT_END_NAMESPACE

