// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPRITESHEET_P_H
#define QT3DEXTRAS_QSPRITESHEET_P_H
#include <QList>

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

#include <Qt3DExtras/private/qabstractspritesheet_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSpriteSheetItem;
class QSpriteSheet;

class QSpriteSheetPrivate : public QAbstractSpriteSheetPrivate
{
    QSpriteSheetPrivate();

    int maxIndex() const override;
    void updateSizes() override;
    void updateTransform() override;

    QList<QSpriteSheetItem *> m_sprites;

    Q_DECLARE_PUBLIC(QSpriteSheet)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITESHEET_P_H

