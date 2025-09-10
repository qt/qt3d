// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_QQMLASPECTENGINE_H
#define QT3D_QUICK_QQMLASPECTENGINE_H

#include <Qt3DCore/QAspectEngine>
#include <Qt3DQuick/qt3dquick_global.h>
#include <QtQml/QQmlEngine>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class QQmlAspectEnginePrivate;

class Q_3DQUICKSHARED_EXPORT QQmlAspectEngine: public QObject
{
    Q_OBJECT
public:
    enum Status { Null, Ready, Loading, Error };

    explicit QQmlAspectEngine(QObject *parent = nullptr);
    ~QQmlAspectEngine();

    Status status() const;
    void setSource(const QUrl &source);

    QQmlEngine *qmlEngine() const;
    QAspectEngine *aspectEngine() const;

Q_SIGNALS:
    void statusChanged(Status);
    void sceneCreated(QObject* rootObject);

private:
    Q_DECLARE_PRIVATE(QQmlAspectEngine)
    Q_PRIVATE_SLOT(d_func(), void _q_continueExecute())
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QQMLASPECTENGINE_H
