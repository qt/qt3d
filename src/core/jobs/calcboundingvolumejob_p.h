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
******************************************************************************/

#ifndef QT3DCORE_CALCBOUNDINGVOLUMEJOB_H
#define QT3DCORE_CALCBOUNDINGVOLUMEJOB_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

#include <QtCore/QSharedPointer>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QCoreAspect;
class CalculateBoundingVolumeJobPrivate;
class QEntity;
class QAttribute;
class QBoundingVolume;
class QGeometryView;

struct Q_3DCORE_PRIVATE_EXPORT BoundingVolumeComputeResult {
    QEntity *entity = nullptr;
    QBoundingVolume *provider = nullptr;
    QAttribute *positionAttribute = nullptr;
    QAttribute *indexAttribute = nullptr;
    QVector3D m_min;
    QVector3D m_max;
    QVector3D m_center;
    float m_radius = -1.f;

    bool valid() const { return m_radius >= 0.f; }
};

struct Q_3DCORE_PRIVATE_EXPORT BoundingVolumeComputeData {
    QEntity *entity = nullptr;
    QBoundingVolume *provider = nullptr;
    QAttribute *positionAttribute = nullptr;
    QAttribute *indexAttribute = nullptr;
    int vertexCount = 0;

    static BoundingVolumeComputeData fromView(QGeometryView *view);

    bool valid() const { return positionAttribute != nullptr; }
    BoundingVolumeComputeResult compute() const;
};

class Q_3DCORE_PRIVATE_EXPORT BoundingVolumeJobProcessor
{
public:
    virtual ~BoundingVolumeJobProcessor() { }

    virtual void process(const BoundingVolumeComputeResult &result, bool computedResult) = 0;
};

class Q_3DCORE_PRIVATE_EXPORT CalculateBoundingVolumeJob : public Qt3DCore::QAspectJob
{
public:
    explicit CalculateBoundingVolumeJob(QCoreAspect *aspect);

    void setRoot(QEntity *root) { m_root = root; }
    bool isRequired() override;
    void run() override;
    void postFrame(QAspectEngine *aspectEngine) override;

    void addWatcher(QWeakPointer<BoundingVolumeJobProcessor> watcher);
    void removeWatcher(QWeakPointer<BoundingVolumeJobProcessor> watcher);

private:
    Q_DECLARE_PRIVATE(CalculateBoundingVolumeJob)
    QCoreAspect *m_aspect;
    QEntity *m_root;
    std::vector<BoundingVolumeComputeResult> m_results;
    std::vector<QWeakPointer<BoundingVolumeJobProcessor>> m_watchers;
};

typedef QSharedPointer<CalculateBoundingVolumeJob> CalculateBoundingVolumeJobPtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_CALCBOUNDINGVOLUMEJOB_H
