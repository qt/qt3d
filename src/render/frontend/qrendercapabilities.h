/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#ifndef QT3DRENDER_QRENDERCAPABILITIES_H
#define QT3DRENDER_QRENDERCAPABILITIES_H

#include <QtCore/qobject.h>
#include <QtGui/qsurfaceformat.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QRenderCapabilitiesPrivate;

class Q_3DRENDERSHARED_EXPORT QRenderCapabilities : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(API api READ api CONSTANT)
    Q_PROPERTY(Profile profile READ profile CONSTANT)
    Q_PROPERTY(int majorVersion READ majorVersion CONSTANT)
    Q_PROPERTY(int minorVersion READ minorVersion CONSTANT)
    Q_PROPERTY(QStringList extensions READ extensions CONSTANT)
    Q_PROPERTY(QString vendor READ vendor CONSTANT)
    Q_PROPERTY(QString renderer READ renderer CONSTANT)
    Q_PROPERTY(QString driverVersion READ driverVersion CONSTANT)
    Q_PROPERTY(QString glslVersion READ glslVersion CONSTANT)
    Q_PROPERTY(int maxSamples READ maxSamples CONSTANT)
    Q_PROPERTY(int maxTextureSize READ maxTextureSize CONSTANT)
    Q_PROPERTY(int maxTextureUnits READ maxTextureUnits CONSTANT)
    Q_PROPERTY(int maxTextureLayers READ maxTextureLayers CONSTANT)
    Q_PROPERTY(bool supportsUBO READ supportsUBO CONSTANT)
    Q_PROPERTY(int maxUBOSize READ maxUBOSize CONSTANT)
    Q_PROPERTY(int maxUBOBindings READ maxUBOBindings CONSTANT)
    Q_PROPERTY(bool supportsSSBO READ supportsSSBO CONSTANT)
    Q_PROPERTY(int maxSSBOSize READ maxSSBOSize CONSTANT)
    Q_PROPERTY(int maxSSBOBindings READ maxSSBOBindings CONSTANT)
    Q_PROPERTY(bool supportsImageStore READ supportsImageStore CONSTANT)
    Q_PROPERTY(int maxImageUnits READ maxImageUnits CONSTANT)
    Q_PROPERTY(bool supportsCompute READ supportsCompute CONSTANT)
    Q_PROPERTY(int maxWorkGroupCountX READ maxWorkGroupCountX CONSTANT)
    Q_PROPERTY(int maxWorkGroupCountY READ maxWorkGroupCountY CONSTANT)
    Q_PROPERTY(int maxWorkGroupCountZ READ maxWorkGroupCountZ CONSTANT)
    Q_PROPERTY(int maxWorkGroupSizeX READ maxWorkGroupSizeX CONSTANT)
    Q_PROPERTY(int maxWorkGroupSizeY READ maxWorkGroupSizeY CONSTANT)
    Q_PROPERTY(int maxWorkGroupSizeZ READ maxWorkGroupSizeZ CONSTANT)
    Q_PROPERTY(int maxComputeInvocations READ maxComputeInvocations CONSTANT)
    Q_PROPERTY(int maxComputeSharedMemorySize READ maxComputeSharedMemorySize CONSTANT)
public:
    enum API {
        OpenGL = QSurfaceFormat::OpenGL,     // 1
        OpenGLES = QSurfaceFormat::OpenGLES, // 2
        Vulkan = 3,                          // 3
        DirectX,                             // 4
        RHI,                                 // 5
    };
    Q_ENUM(API)

    enum Profile {
        NoProfile = QSurfaceFormat::NoProfile,
        CoreProfile = QSurfaceFormat::CoreProfile,
        CompatibilityProfile = QSurfaceFormat::CompatibilityProfile
    };
    Q_ENUM(Profile)

    explicit QRenderCapabilities(QObject *parent = nullptr);
    ~QRenderCapabilities();

    bool isValid() const;
    API api() const;
    Profile profile() const;
    int majorVersion() const;
    int minorVersion() const;
    QStringList extensions() const;
    QString vendor() const;
    QString renderer() const;
    QString driverVersion() const;
    QString glslVersion() const;
    int maxSamples() const;
    int maxTextureSize() const;
    int maxTextureUnits() const;
    int maxTextureLayers() const;
    bool supportsUBO() const;
    int maxUBOSize() const;
    int maxUBOBindings() const;
    bool supportsSSBO() const;
    int maxSSBOSize() const;
    int maxSSBOBindings() const;
    bool supportsImageStore() const;
    int maxImageUnits() const;
    bool supportsCompute() const;
    int maxWorkGroupCountX() const;
    int maxWorkGroupCountY() const;
    int maxWorkGroupCountZ() const;
    int maxWorkGroupSizeX() const;
    int maxWorkGroupSizeY() const;
    int maxWorkGroupSizeZ() const;
    int maxComputeInvocations() const;
    int maxComputeSharedMemorySize() const;

protected:
    Q_DECLARE_PRIVATE(QRenderCapabilities)
};

} // namespace Qt3Drender

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERCAPABILITIES_H
