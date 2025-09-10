// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QASPECTENGINE_H
#define QT3DCORE_QASPECTENGINE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/QList>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractAspect;
class QAspectThread;
class QAspectEnginePrivate;
class QEntity;
class QNode;

typedef QSharedPointer<QEntity> QEntityPtr;

class Q_3DCORESHARED_EXPORT QAspectEngine : public QObject
{
    Q_OBJECT
public:
    enum RunMode {
        Manual = 0,
        Automatic
    };
    Q_ENUM(RunMode)

    explicit QAspectEngine(QObject *parent = nullptr);
    ~QAspectEngine();

    void setRootEntity(QEntityPtr root);
    QEntityPtr rootEntity() const;

    void setRunMode(RunMode mode);
    RunMode runMode() const;

    void registerAspect(QAbstractAspect *aspect);
    void registerAspect(const QString &name);
    void unregisterAspect(QAbstractAspect *aspect);
    void unregisterAspect(const QString &name);

    QList<QAbstractAspect*> aspects() const;
    QAbstractAspect *aspect(const QString &name) const;

    QVariant executeCommand(const QString &command);

    void processFrame();

    QNode *lookupNode(QNodeId id) const;
    QList<QNode *> lookupNodes(const QList<QNodeId> &ids) const;

private:
    Q_DECLARE_PRIVATE(QAspectEngine)
};

} // namespace Qt3DCore

QT_END_NAMESPACE


#endif // QT3DCORE_QASPECTENGINE_H
