// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPRITEGRID_P_H
#define QT3DEXTRAS_QSPRITEGRID_P_H

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

class QSpriteGrid;

class QSpriteGridPrivate : public QAbstractSpriteSheetPrivate
{
    QSpriteGridPrivate();

    int maxIndex() const override;
    void updateSizes() override;
    void updateTransform() override;

    int m_numColumns;
    int m_numRows;
    QSizeF m_cellSize;

    Q_DECLARE_PUBLIC(QSpriteGrid)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITEGRID_P_H

