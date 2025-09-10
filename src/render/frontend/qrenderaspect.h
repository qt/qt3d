// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QRENDERASPECT_H
#define QT3DRENDER_QRENDERASPECT_H

#include <Qt3DRender/qt3drender_global.h>
#include <Qt3DCore/qabstractaspect.h>

QT_BEGIN_NAMESPACE


class QOpenGLContext;

namespace Qt3DRender {

#if defined(QT_BUILD_INTERNAL)
class TestAspect;
#endif

namespace Render {
class Renderer;
class QRenderPlugin;
}

class QRenderAspectPrivate;

#if defined(QT_BUILD_INTERNAL)
class QRenderAspectTester;
#endif

class Q_3DRENDERSHARED_EXPORT QRenderAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    enum SubmissionType {
        Automatic = 0,
        Manual
    };

    explicit QRenderAspect(QObject *parent = nullptr);
    explicit QRenderAspect(SubmissionType submissionType, QObject *parent = nullptr);
    ~QRenderAspect();

protected:
    QRenderAspect(QRenderAspectPrivate &dd, QObject *parent);
    Q_DECLARE_PRIVATE(QRenderAspect)

private:
    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;

    QVariant executeCommand(const QStringList &args) override;

    void onRegistered() override;
    void onUnregistered() override;

    void onEngineStartup() override;

    QStringList dependencies() const override;

    friend class Render::Renderer;
    friend class Render::QRenderPlugin;
#if defined(QT_BUILD_INTERNAL)
    friend class QRenderAspectTester;
    friend class TestAspect;
#endif
};

}

QT_END_NAMESPACE

#endif // QT3DRENDER_QRENDERASPECT_H
