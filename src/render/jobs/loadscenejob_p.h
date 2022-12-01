// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_LOADSCENEJOB_H
#define QT3DRENDER_RENDER_LOADSCENEJOB_H

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

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DRender/qsceneloader.h>
#include <QSharedPointer>
#include <QUrl>
#include <functional>
#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSceneImporter;

namespace Render {

class Scene;
class NodeManagers;

class LoadSceneJob;

class Q_AUTOTEST_EXPORT LoadSceneJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    explicit LoadSceneJobPrivate(LoadSceneJob *q): q_ptr(q) {}
    ~LoadSceneJobPrivate() override {}

    void postFrame(Qt3DCore::QAspectManager *manager) override;

    std::unique_ptr<Qt3DCore::QEntity> m_sceneSubtree;
    QSceneLoader::Status m_status = QSceneLoader::None;

    Q_DECLARE_PUBLIC(LoadSceneJob)
private:
    LoadSceneJob *q_ptr;
};

class Q_3DRENDERSHARED_PRIVATE_EXPORT LoadSceneJob : public Qt3DCore::QAspectJob
{
public:
    explicit LoadSceneJob(const QUrl &source, Qt3DCore::QNodeId sceneComponent);
    void setData(const QByteArray &data);
    void setNodeManagers(NodeManagers *managers) { m_managers = managers; }
    void setSceneImporters(const QList<QSceneImporter *> sceneImporters) { m_sceneImporters = sceneImporters; }

    NodeManagers *nodeManagers() const;
    QList<QSceneImporter *> sceneImporters() const;
    QUrl source() const;
    Qt3DCore::QNodeId sceneComponentId() const;

    void run() override;

private:
    QUrl m_source;
    QByteArray m_data;
    Qt3DCore::QNodeId m_sceneComponent;
    NodeManagers *m_managers;
    QList<QSceneImporter *> m_sceneImporters;

    Qt3DCore::QEntity *tryLoadScene(QSceneLoader::Status &finalStatus,
                                    const QStringList &extensions,
                                    const std::function<void (QSceneImporter *)> &importerSetupFunc);
    Q_DECLARE_PRIVATE(LoadSceneJob)
};

typedef QSharedPointer<LoadSceneJob> LoadSceneJobPtr;

} // namespace Render

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // LOADSCENEJOB_H
