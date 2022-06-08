// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DRENDER_QUICK3DRENDER_SCENE2DEVENT_P_H
#define QT3DRENDER_QUICK3DRENDER_SCENE2DEVENT_P_H

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

#include <Qt3DQuickScene2D/qt3dquickscene2d_global.h>
#include <private/qglobal_p.h>

#include <QtCore/QEvent>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Quick {

class Scene2DEvent : public QEvent
{
public:
    enum Type {
        Initialize = QEvent::User + 1,
        Render,
        RenderSync,
        Prepare,
        Initialized,
        Rendered,
        Quit
    };
    Scene2DEvent(Type type)
        : QEvent(static_cast<QEvent::Type>(type))
    {
    }
};

} // namespace Quick
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QUICK3DRENDER_SCENE2DEVENT_P_H
