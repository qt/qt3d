// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
 * \fn [nodeid-qhash] constexpr size_t Qt3DCore::qHash(QNodeId id, uint seed)
 * \relates Qt3DCore::QNodeId
 * \return hash of node with \a id and optional \a seed.
 */

/*!
 * \return node id.
 */
QNodeId QNodeId::createId() noexcept
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
