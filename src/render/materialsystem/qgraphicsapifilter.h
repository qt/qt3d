// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QGRAPHICSAPIFILTER_H
#define QT3DRENDER_QGRAPHICSAPIFILTER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <Qt3DRender/qt3drender_global.h>
#include <QtGui/QSurfaceFormat>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QGraphicsApiFilterPrivate;

class Q_3DRENDERSHARED_EXPORT QGraphicsApiFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QGraphicsApiFilter::Api api READ api WRITE setApi NOTIFY apiChanged)
    Q_PROPERTY(Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(int minorVersion READ minorVersion WRITE setMinorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(QStringList extensions READ extensions WRITE setExtensions NOTIFY extensionsChanged)
    Q_PROPERTY(QString vendor READ vendor WRITE setVendor NOTIFY vendorChanged)

public:

    enum Api {
        OpenGLES = QSurfaceFormat::OpenGLES, // 2
        OpenGL = QSurfaceFormat::OpenGL,     // 1
        Vulkan = 3,                          // 3
        DirectX,                             // 4
        RHI,                                 // 5
    };
    Q_ENUM(Api) // LCOV_EXCL_LINE

    enum OpenGLProfile {
        NoProfile = QSurfaceFormat::NoProfile,
        CoreProfile = QSurfaceFormat::CoreProfile,
        CompatibilityProfile = QSurfaceFormat::CompatibilityProfile
    };
    Q_ENUM(OpenGLProfile) // LCOV_EXCL_LINE

    explicit QGraphicsApiFilter(QObject *parent = nullptr);
    ~QGraphicsApiFilter();

    Api api() const;
    OpenGLProfile profile() const;
    int minorVersion() const;
    int majorVersion() const;
    QStringList extensions() const;
    QString vendor() const;

public Q_SLOTS:
    void setApi(Api api);
    void setProfile(OpenGLProfile profile);
    void setMinorVersion(int minorVersion);
    void setMajorVersion(int majorVersion);
    void setExtensions(const QStringList &extensions);
    void setVendor(const QString &vendor);

Q_SIGNALS:
    void apiChanged(Qt3DRender::QGraphicsApiFilter::Api api);
    void profileChanged(Qt3DRender::QGraphicsApiFilter::OpenGLProfile profile);
    void minorVersionChanged(int minorVersion);
    void majorVersionChanged(int majorVersion);
    void extensionsChanged(const QStringList &extensions);
    void vendorChanged(const QString &vendor);
    void graphicsApiFilterChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsApiFilter)
};

Q_AUTOTEST_EXPORT bool operator ==(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample);
Q_AUTOTEST_EXPORT bool operator !=(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample);

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGRAPHICSAPIFILTER_H
