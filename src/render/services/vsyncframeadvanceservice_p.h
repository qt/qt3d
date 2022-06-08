// Copyright (C) 2015 Paul Lemire
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_VSYNCFRAMEADVANCESERVICE_H
#define QT3DRENDER_RENDER_VSYNCFRAMEADVANCESERVICE_H

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

#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

namespace Render {

class VSyncFrameAdvanceServicePrivate;

class Q_3DRENDERSHARED_PRIVATE_EXPORT VSyncFrameAdvanceService final : public Qt3DCore::QAbstractFrameAdvanceService
{
public:
    explicit VSyncFrameAdvanceService(bool drivenByRenderThread);
    ~VSyncFrameAdvanceService();

    qint64 waitForNextFrame() final;
    void start() final;
    void stop() final;

    void proceedToNextFrame();

private:
    Q_DECLARE_PRIVATE(VSyncFrameAdvanceService)
};

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_VSYNCFRAMEADVANCESERVICE_H
