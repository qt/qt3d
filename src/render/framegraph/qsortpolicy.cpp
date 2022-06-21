// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsortpolicy_p.h"

QT_BEGIN_NAMESPACE

using namespace Qt3DCore;

namespace Qt3DRender {

QSortPolicyPrivate::QSortPolicyPrivate()
    : QFrameGraphNodePrivate()
{
}

/*!
    \class Qt3DRender::QSortPolicy
    \inmodule Qt3DRender
    \brief Provides storage for the sort types to be used.
    \since 5.7

    \inherits Qt3DRender::QFrameGraphNode

    A Qt3DRender::QSortPolicy class stores the sorting type used by the FrameGraph.
    The sort types determine how drawable entities are sorted before drawing to
    determine the drawing order. When QSortPolicy is present in the FrameGraph,
    the sorting mechanism is determined by the sortTypes list. Multiple sort types
    can be used simultaneously. If QSortPolicy is not present in the FrameGraph,
    entities are drawn in the order they appear in the entity hierarchy.
 */

/*!
    \qmltype SortPolicy
    \inqmlmodule Qt3D.Render
    \since 5.7
    \instantiates Qt3DRender::QSortPolicy
    \inherits FrameGraphNode
    \brief Provides storage for the sort types to be used.

    A SortPolicy class stores the sorting type used by the FrameGraph.
    The sort types determine how drawable entities are sorted before drawing to
    determine the drawing order. When SortPolicy is present in the FrameGraph,
    the sorting mechanism is determined by the sortTypes list. Multiple sort
    types can be used simultaneously. If SortPolicy is not present in the FrameGraph,
    entities are drawn in the order they appear in the entity hierarchy.
 */

/*!
    \enum QSortPolicy::SortType

    This enum type describes the available sort types.

    \value StateChangeCost sort the objects so as to minimize the cost of
    changing from the currently rendered state

    \value BackToFront sort the objects from back to front based on inverted z
    order. More accurately, the sorting key is the z component of the
    projection of the camera-to-object-center vector onto the camera's view
    vector.

    \value Material sort the objects based on their material (shader) value.

    \value FrontToBack sort the objects from front to back. The opposite of
    BackToFront.

    \value [since 5.14] Texture sort the objects to minimize texture changes.

    \value [since 5.15] Uniform sort the objects to minimize uniform changes.
*/

/*!
    \property QSortPolicy::sortTypes
    Specifies the sorting types to be used.
*/

/*!
    \qmlproperty list<int> SortPolicy::sortTypes
    Specifies the sorting types to be used.

    This list can include the following values:
    \list
    \li StateChangeCost - sort the objects so as to minimize the cost of
        changing from the currently rendered state
    \li BackToFront - sort the objects from back to front based on inverted z
        order. More accurately, the sorting key is the z component of the
        projection of the camera-to-object-center vector onto the camera's view
        vector.
    \li Material - sort the objects based on their material (shader) value.
    \li FrontToBack - sort the objects from front to back. The opposite of
        BackToFront.
    \li [since 5.14] Texture - sort the objects to minimize texture changes.
    \li [since 5.15] Uniform - sort the objects to minimize uniform changes.
    \endlist
*/

/*!
    Constructs QSortPolicy with given \a parent.
 */
QSortPolicy::QSortPolicy(QNode *parent)
    : QFrameGraphNode(*new QSortPolicyPrivate, parent)
{
}

/*! \internal */
QSortPolicy::~QSortPolicy()
{
}

/*! \internal */
QSortPolicy::QSortPolicy(QSortPolicyPrivate &dd, QNode *parent)
    : QFrameGraphNode(dd, parent)
{
}

/*!
    \return the current sort types in use
 */
QList<QSortPolicy::SortType> QSortPolicy::sortTypes() const
{
    Q_D(const QSortPolicy);
    return d->m_sortTypes;
}

QList<int> QSortPolicy::sortTypesInt() const
{
    Q_D(const QSortPolicy);
    QList<int> sortTypesInt;
    transformVector(d->m_sortTypes, sortTypesInt);
    return sortTypesInt;
}

void QSortPolicy::setSortTypes(const QList<SortType> &sortTypes)
{
    Q_D(QSortPolicy);
    if (sortTypes != d->m_sortTypes) {
        d->m_sortTypes = sortTypes;
        emit sortTypesChanged(sortTypes);

        const bool wasBlocked = blockNotifications(true);
        emit sortTypesChanged(sortTypesInt());
        blockNotifications(wasBlocked);
    }
}

void QSortPolicy::setSortTypes(const QList<int> &sortTypesInt)
{
    QList<SortType> sortTypes;
    transformVector(sortTypesInt, sortTypes);
    setSortTypes(sortTypes);
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qsortpolicy.cpp"
