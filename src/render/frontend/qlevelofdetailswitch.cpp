// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qlevelofdetailswitch.h"
#include "qlevelofdetailswitch_p.h"
#include "qlevelofdetail_p.h"
#include "qglobal.h"
#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QLevelOfDetailSwitchPrivate::QLevelOfDetailSwitchPrivate()
    : QLevelOfDetailPrivate()
{

}

void QLevelOfDetailSwitchPrivate::setCurrentIndex(int currentIndex)
{
    Q_Q(QLevelOfDetailSwitch);

    bool changed = m_currentIndex != currentIndex;
    QLevelOfDetailPrivate::setCurrentIndex(currentIndex);

    if (!changed)
        return;

    int entityIndex = 0;
    const auto entities = q->entities();
    for (Qt3DCore::QEntity *entity : entities) {
        const auto childNodes = entity->childNodes();
        for (Qt3DCore::QNode *childNode : childNodes) {
            Qt3DCore::QEntity *childEntity = qobject_cast<Qt3DCore::QEntity *>(childNode);
            if (childEntity) {
                childEntity->setEnabled(entityIndex == currentIndex);
                entityIndex++;
            }
        }

        break; // only work on the first entity, LOD should not be shared
    }
}

/*!
    \class Qt3DRender::QLevelOfDetailSwitch
    \inmodule Qt3DRender
    \inherits Qt3DRender::QLevelOfDetail
    \since 5.9
    \brief Provides a way of enabling child entities based on distance or screen size.

    This component is assigned to an entity. When the entity changes distance relative
    to the camera, the QLevelOfDetailSwitch will disable all the child entities except
    the one matching index Qt3DRender::QLevelOfDetailSwitch::currentIndex.
*/

/*!
    \qmltype LevelOfDetailSwitch
    \instantiates Qt3DRender::QLevelOfDetailSwitch
    \inherits Component3D
    \inqmlmodule Qt3D.Render
    \since 5.9
    \brief Provides a way of enabling child entities based on distance or screen size.

    This component is assigned to an entity. When the entity changes distance relative
    to the camera, the LevelOfDetailSwitch will disable all the child entities except
    the one matching index \l currentIndex.

    \sa LevelOfDetail
*/

/*!
    \qmlproperty int LevelOfDetailSwitch::currentIndex

    The index of the presently selected child entity.
*/

/*! \fn Qt3DRender::QLevelOfDetailSwitch::QLevelOfDetailSwitch(Qt3DCore::QNode *parent)
  Constructs a new QLevelOfDetailSwitch with the specified \a parent.
 */
QLevelOfDetailSwitch::QLevelOfDetailSwitch(QNode *parent)
    : QLevelOfDetail(*new QLevelOfDetailSwitchPrivate(), parent)
{
    Q_D(QLevelOfDetailSwitch);
    d->m_currentIndex = -1;
}

/*! \internal */
QLevelOfDetailSwitch::~QLevelOfDetailSwitch()
{
}

/*! \internal */
QLevelOfDetailSwitch::QLevelOfDetailSwitch(QLevelOfDetailPrivate &dd, QNode *parent)
    : QLevelOfDetail(dd, parent)
{
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qlevelofdetailswitch.cpp"
