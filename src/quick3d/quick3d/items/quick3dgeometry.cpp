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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DCore::QAttribute;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *attribute) {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        geometry->m_managedAttributes.append(attribute);
        geometry->parentGeometry()->addAttribute(attribute);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        return geometry->parentGeometry()->attributes().count();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        return geometry->parentGeometry()->attributes().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DGeometry *geometry = static_cast<Quick3DGeometry *>(list->object);
        for (Qt3DCore::QAttribute *attribute : qAsConst(geometry->m_managedAttributes))
            geometry->parentGeometry()->removeAttribute(attribute);
        geometry->m_managedAttributes.clear();
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_quick3dgeometry_p.cpp"
