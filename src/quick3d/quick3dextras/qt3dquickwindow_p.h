/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef QT3DQUICKWINDOW_P_H
#define QT3DQUICKWINDOW_P_H

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

namespace Quick {

class Qt3DQuickWindowPrivate : public QWindowPrivate
{
public:
    Qt3DQuickWindowPrivate();

    Qt3DCore::Quick::QQmlAspectEngine *m_engine;

    // Aspects
    Qt3DRender::QRenderAspect *m_renderAspect;
    Qt3DInput::QInputAspect *m_inputAspect;
    Qt3DLogic::QLogicAspect *m_logicAspect;

    QUrl m_source;
    bool m_initialized;
    QPointer<Qt3DRender::QCamera> m_camera;
    Qt3DQuickWindow::CameraAspectRatioMode m_cameraAspectRatioMode;
    QQmlIncubationController *m_incubationController;

    Q_DECLARE_PUBLIC(Qt3DQuickWindow)
};

} // Quick

} // Qt3DExtras

QT_END_NAMESPACE

#endif // QT3DQUICKWINDOW_P_H
