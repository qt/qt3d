// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DEXTRAS_QSPRITESHEETITEM_P_H
#define QT3DEXTRAS_QSPRITESHEETITEM_P_H

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

#include <Qt3DCore/private/qnode_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class QSpriteSheetItem;

class QSpriteSheetItemPrivate: public Qt3DCore::QNodePrivate
{
    QSpriteSheetItemPrivate();

    int m_x;
    int m_y;
    int m_width;
    int m_height;

    Q_DECLARE_PUBLIC(QSpriteSheetItem)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DEXTRAS_QSPRITESHEETITEM_P_H

