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
****************************************************************************/

#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

/*!
 * \class Qt3DCore::QNodeId
 * \inheaderfile Qt3DCore/QNodeId
 * \inmodule Qt3DCore
 * \brief Uniquely identifies a QNode.
 */

/*!
 * \typedef Qt3DCore::QNodeIdVector
 * \relates Qt3DCore::QNodeId
 *
 * A vector of \l {QNodeId}s.
 */

/*!
 * \fn bool Qt3DCore::QNodeId::isNull() const
 * \return \TODO
 */

/*!
 * \fn bool Qt3DCore::QNodeId::operator ==(QNodeId other) const
 * \return \c true if \c this == \a other.
 */

/*!
 * \fn bool Qt3DCore::QNodeId::operator !=(QNodeId other) const
 * \return \c true if \c this != \a other.
 */

/*!
 * \fn bool Qt3DCore::QNodeId::operator <(QNodeId other) const
 * \return \c true if \c this < \a other.
 */

/*!
 * \fn bool Qt3DCore::QNodeId::operator >(QNodeId other) const
 * \return \c true if \c this > \a other.
 */

/*!
 * \fn quint64 Qt3DCore::QNodeId::id() const
 * \return \TODO
 */

/*!
 * \fn Qt3DCore::QNodeId::operator bool() const
 * Returns \TODO
 */

/*!
 * \fn uint Qt3DCore::qHash(QNodeId id, uint seed = 0)
 * \relates Qt3DCore::QNodeId
 * \return hash of node with \a id and optional \a seed.
 */

/*!
 * \return node id.
 */
QNodeId QNodeId::createId() Q_DECL_NOTHROW
{
    typedef
#if defined(Q_ATOMIC_INT64_IS_SUPPORTED)
        quint64
#else
        quint32
#endif
        UIntType;
    static QBasicAtomicInteger<UIntType> next = Q_BASIC_ATOMIC_INITIALIZER(0);
    return QNodeId(next.fetchAndAddRelaxed(1) + 1);
}

#ifndef QT_NO_DEBUG_STREAM
/*!
 * << with \a d and \a id.
 * \return QDebug.
 */
QDebug operator<<(QDebug d, QNodeId id)
{
    d << id.id();
    return d;
}
#endif

}

QT_END_NAMESPACE
