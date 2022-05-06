/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    using qt_size_type = qsizetype;
#else
    using qt_size_type = int;
#endif

    using ListContentType = Qt3DExtras::QSpriteSheetItem;
    auto appendFunction = [](QQmlListProperty<ListContentType> *list, ListContentType *sprite) {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        spritesheet->parentSpriteSheet()->addSprite(sprite);
    };
    auto countFunction = [](QQmlListProperty<ListContentType> *list) -> qt_size_type {
        Quick3DSpriteSheet *spritesheet = qobject_cast<Quick3DSpriteSheet *>(list->object);
        return spritesheet->parentSpriteSheet()->sprites().count();
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

