// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    using qt_size_type = qsizetype;
    using ListContentType = QAbstractTextureImage;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *textureImage) {
        Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
        if (self)
            self->parentTexture()->addTextureImage(textureImage);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DTextureExtension *self = qobject_cast<Quick3DTextureExtension *>(list->object);
        if (self)
            return self->parentTexture()->textureImages().size();
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
