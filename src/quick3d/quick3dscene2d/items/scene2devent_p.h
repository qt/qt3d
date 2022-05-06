/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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
