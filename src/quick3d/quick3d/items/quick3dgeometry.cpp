// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DQuick/private/quick3dgeometry_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

Quick3DGeometry::Quick3DGeometry(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DCore::QAttribute> Quick3DGeometry::attributeList()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DCore::QAttribute;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *attribute) {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        geometry->m_managedAttributes.append(attribute);
        geometry->parentGeometry()->addAttribute(attribute);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        return geometry->parentGeometry()->attributes().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        return geometry->parentGeometry()->attributes().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        for (Qt3DCore::QAttribute *attribute : std::as_const(geometry->m_managedAttributes))
            geometry->parentGeometry()->removeAttribute(attribute);
        geometry->m_managedAttributes.clear();
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3dgeometry_p.cpp"
