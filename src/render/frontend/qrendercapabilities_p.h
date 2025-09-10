// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERCAPABILITIES_P_H
#define QT3DRENDER_QRENDERCAPABILITIES_P_H

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

#include <QtCore/private/qobject_p.h>
#include <Qt3DRender/qrendercapabilities.h>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderCapabilitiesPrivate : public QObjectPrivate
{
public:
    QRenderCapabilitiesPrivate();

    Q_DECLARE_PUBLIC(QRenderCapabilities)
    static const QRenderCapabilitiesPrivate *get(const QRenderCapabilities *q);

    bool m_valid;
    QRenderCapabilities::API m_api = QRenderCapabilities::OpenGL;
    QRenderCapabilities::Profile m_profile = QRenderCapabilities::NoProfile;
    int m_majorVersion = 0;
    int m_minorVersion = 0;
    QStringList m_extensions;
    QString m_vendor;
    QString m_renderer;
    QString m_version;
    QString m_glslVersion;
    int m_maxSamples = 0;
    int m_maxTextureSize = 0;
    int m_maxTextureUnits = 0;
    int m_maxTextureLayers = 0;
    bool m_supportsUBO = false;
    int m_maxUBOSize = 0;
    int m_maxUBOBindings = 0;
    bool m_supportsSSBO = false;
    int m_maxSSBOSize = 0;
    int m_maxSSBOBindings = 0;
    bool m_supportsImageStore = false;
    int m_maxImageUnits = 0;
    bool m_supportCompute = false;
    int m_maxWorkGroupCount[3] = { 0, 0, 0 };
    int m_maxWorkGroupSize[3] = { 0, 0, 0 };
    int m_maxComputeInvocations = 0;
    int m_maxComputeSharedMemorySize = 0;

    QString toString() const;
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERCAPABILITIES_P_H
