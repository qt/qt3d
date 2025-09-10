// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qt3dquick3dscene2d_p.h"
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

QQuick3DScene2D::QQuick3DScene2D(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<Qt3DCore::QEntity> QQuick3DScene2D::entities()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DCore::QEntity;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *entity) {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d)
            scene2d->parentScene2D()->addEntity(entity);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d)
            return scene2d->parentScene2D()->entities().size();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d) {
            return qobject_cast<Qt3DCore::QEntity *>(
                scene2d->parentScene2D()->entities().at(index));
        }
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d) {
            QList<Qt3DCore::QEntity*> entities = scene2d->parentScene2D()->entities();
            for (Qt3DCore::QEntity *e : entities)
                scene2d->parentScene2D()->removeEntity(e);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qt3dquick3dscene2d_p.cpp"
