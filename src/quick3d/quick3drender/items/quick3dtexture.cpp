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
******************************************************************************/

#include <QtCore/QDebug>

#include <Qt3DQuickRender/private/quick3dtexture_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

Quick3DTextureExtension::Quick3DTextureExtension(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractTextureImage> Quick3DTextureExtension::textureImages()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = QAbstractTextureImage;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *textureImage) {
        Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
        if (self)
            self->parentTexture()->addTextureImage(textureImage);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
        if (self)
            return self->parentTexture()->textureImages().count();
        return 0;
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
        if (self)
            return self->parentTexture()->textureImages().at(index);
        return nullptr;
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        if (Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object)) {
            const auto images = self->parentTexture()->textureImages();
            for (QAbstractTextureImage *img : images)
                self->parentTexture()->removeTextureImage(img);
        }
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_quick3dtexture_p.cpp"
