// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICK3DLOGICFOREIGN_P_H
#define QT3DQUICK3DLOGICFOREIGN_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DLogic/qframeaction.h>
#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE
class QFrameActionForeign
{
    Q_GADGET
    QML_FOREIGN(Qt3DLogic::QFrameAction)
    QML_NAMED_ELEMENT(FrameAction)
    QML_ADDED_IN_VERSION(2, 0);
};

QT_END_NAMESPACE

#endif // QT3DQUICK3DLOGICFOREIGN_P_H
