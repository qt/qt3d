// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSCENELOADER_H
#define QT3DRENDER_QSCENELOADER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSceneLoaderPrivate;

class Q_3DRENDERSHARED_EXPORT QSceneLoader : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    explicit QSceneLoader(Qt3DCore::QNode *parent = nullptr);
    ~QSceneLoader();

    enum Status {
        None = 0,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status) // LCOV_EXCL_LINE

    enum ComponentType {
        UnknownComponent = 0,
        GeometryRendererComponent,
        TransformComponent,
        MaterialComponent,
        LightComponent,
        CameraLensComponent
    };
    Q_ENUM(ComponentType)

    QUrl source() const;
    Status status() const;

    Q_REVISION(2, 9) Q_INVOKABLE Qt3DCore::QEntity *entity(const QString &entityName) const;
    Q_REVISION(2, 9) Q_INVOKABLE QStringList entityNames() const;
    Q_REVISION(2, 9) Q_INVOKABLE Qt3DCore::QComponent *component(const QString &entityName,
                                                              ComponentType componentType) const;

public Q_SLOTS:
    void setSource(const QUrl &arg);

Q_SIGNALS:
    void sourceChanged(const QUrl &source);
    void statusChanged(Status status);

protected:
    explicit QSceneLoader(QSceneLoaderPrivate &dd, Qt3DCore::QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QSceneLoader)
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCENELOADER_H
