// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3dentity_p.h"

#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

/*!
    \qmltype Entity
    \nativetype Qt3DCore::QEntity
    \inherits Node
    \inqmlmodule Qt3D.Core
    \since 5.5

    \brief Entity is a \l Node subclass that can aggregate several
    \l Component3D instances that will specify its behavior.

    By itself a Entity is an empty shell. The behavior of a Entity
    object is defined by the \l Component3D objects it references. Each Qt3D
    backend aspect will be able to interpret and process an Entity by
    recognizing which components it is made up of. One aspect may decide to only
    process entities composed of a single \l Transform component whilst
    another may focus on \l MouseHandler.

    \sa Qt3D.Core::Component3D, Qt3D.Core::Transform
 */

/*!
    \qmlproperty list<Component3D> Entity::components
    Holds the list of \l Component3D instances, which define the behavior
    of the entity.
    \readonly
 */

Quick3DEntity::Quick3DEntity(QObject *parent)
    : QObject(parent)
{
}


QQmlListProperty<QComponent> Quick3DEntity::componentList()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DCore::QComponent;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *comp) {
        if (comp == nullptr)
            return;
        Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
        self->m_managedComponents.push_back(comp);
        self->parentEntity()->addComponent(comp);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
        return self->parentEntity()->components().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
        return self->parentEntity()->components().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DEntity *self = static_cast<Quick3DEntity *>(list->object);
        for (QComponent *comp : std::as_const(self->m_managedComponents))
            self->parentEntity()->removeComponent(comp);
        self->m_managedComponents.clear();
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}


} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3dentity_p.cpp"
