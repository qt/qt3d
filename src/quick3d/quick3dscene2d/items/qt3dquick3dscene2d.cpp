/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DCore::QEntity;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *entity) {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d)
            scene2d->parentScene2D()->addEntity(entity);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        QQuick3DScene2D *scene2d = qobject_cast<QQuick3DScene2D *>(list->object);
        if (scene2d)
            return scene2d->parentScene2D()->entities().count();
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
