/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QBACKENDNODE_P_H
#define QT3DCORE_QBACKENDNODE_P_H

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

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>

#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <Qt3DCore/private/qobserverinterface_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QNode;

class Q_3DCORE_PRIVATE_EXPORT QBackendNodePrivate
        : public QObserverInterface
        , public QObservableInterface
{
public:
    QBackendNodePrivate(QBackendNode::Mode mode);

    void setArbiter(QLockableObserverInterface *arbiter) override;
    void notifyObservers(const QSceneChangePtr &e) override;
    void sceneChangeEvent(const QSceneChangePtr &e) override;
    void setEnabled(bool enabled);

    static QBackendNodePrivate *get(QBackendNode *n);

    Q_DECLARE_PUBLIC(QBackendNode)
    QBackendNode *q_ptr;
    QBackendNode::Mode m_mode;

    QLockableObserverInterface *m_arbiter;
    QNodeId m_peerId;
    bool m_enabled;

    virtual void addedToEntity(QNode *frontend);
    virtual void removedFromEntity(QNode *frontend);
    virtual void componentAdded(QNode *frontend);
    virtual void componentRemoved(QNode *frontend);

private:
    Q_DISABLE_COPY(QBackendNodePrivate)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QBACKENDNODE_P_H
