// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DWINDOW_P_H
#define QT3DWINDOW_P_H

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

#include <QtGui/private/qwindow_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DExtras {

class Qt3DWindowPrivate : public QWindowPrivate
{
public:
    Qt3DWindowPrivate();

    Qt3DCore::QAspectEngine *m_aspectEngine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    // Renderer configuration
    Qt3DRender::QRenderSettings *m_renderSettings;
    Qt3DExtras::QForwardRenderer *m_forwardRenderer;
    Qt3DRender::QCamera *m_defaultCamera;

    // Input configuration
    Qt3DInput::QInputSettings *m_inputSettings;

    // Logic configuration

    // Scene
    Qt3DCore::QEntity *m_root;
    Qt3DCore::QEntity *m_userRoot;

    bool m_initialized;

    Q_DECLARE_PUBLIC(Qt3DWindow)
};

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DWINDOW_P_H
