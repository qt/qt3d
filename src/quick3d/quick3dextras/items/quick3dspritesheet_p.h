// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DEXTRAS_EXTRAS_QUICK_QUICK3DSPRITESHEET_P_H
#define QT3DEXTRAS_EXTRAS_QUICK_QUICK3DSPRITESHEET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DQuickExtras/qt3dquickextras_global.h>
#include <Qt3DExtras/qspritesheet.h>
#include <QtQml/QQmlListProperty>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {
namespace Extras {
namespace Quick {

class Q_3DQUICKEXTRASSHARED_EXPORT Quick3DSpriteSheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DExtras::QSpriteSheetItem> sprites READ sprites CONSTANT)
    Q_CLASSINFO("DefaultProperty", "sprites")
public:
    explicit Quick3DSpriteSheet(QObject *parent = 0);
    ~Quick3DSpriteSheet();

    QQmlListProperty<Qt3DExtras::QSpriteSheetItem> sprites();
    inline QSpriteSheet *parentSpriteSheet() const { return qobject_cast<QSpriteSheet *>(parent()); }
};

} // namespace Quick
} // namespace Extras
} // namespace Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_EXTRAS_QUICK_QUICK3DSPRITESHEET_P_H
