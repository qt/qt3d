// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "quick3dspritesheet_p.h"
#include <Qt3DExtras/qspritesheetitem.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {
namespace Extras {
namespace Quick {

Quick3DSpriteSheet::Quick3DSpriteSheet(QObject *parent)
    : QObject(parent)
{
}

Quick3DSpriteSheet::~Quick3DSpriteSheet()
{
}

QQmlListProperty<Qt3DExtras::QSpriteSheetItem> Quick3DSpriteSheet::sprites()
{
    using qt_size_type = qsizetype;
    using ListContentType = Qt3DExtras::QSpriteSheetItem;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *sprite) {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        spritesheet->parentSpriteSheet()->addSprite(sprite);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        return spritesheet->parentSpriteSheet()->sprites().size();
    };
    auto atFunction = [](QQmlListProperty<ListContentType> *list, qt_size_type index) -> ListContentType * {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        return spritesheet->parentSpriteSheet()->sprites().at(index);
    };
    auto clearFunction = [](QQmlListProperty<ListContentType> *list) {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        const auto sprites = spritesheet->parentSpriteSheet()->sprites();
        for (QSpriteSheetItem *sprite : sprites)
            spritesheet->parentSpriteSheet()->removeSprite(sprite);
    };

    return QQmlListProperty<ListContentType>(this, nullptr, appendFunction, countFunction, atFunction, clearFunction);
}

} // namespace Quick
} // namespace Extras
} // namespace Qt3DExtras

QT_END_NAMESPACE

#include "moc_quick3dspritesheet_p.cpp"

