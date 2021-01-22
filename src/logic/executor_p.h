/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DLOGIC_LOGIC_EXECUTOR_P_H
#define QT3DLOGIC_LOGIC_EXECUTOR_P_H

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

#include <QtCore/qobject.h>
#include <QtCore/qcoreevent.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

class QSemaphore;

namespace Qt3DCore {
class QScene;
}

namespace Qt3DLogic {
namespace Logic {

class Executor : public QObject
{
    Q_OBJECT
public:
    explicit Executor(QObject *parent = 0);

    void setScene(Qt3DCore::QScene *scene) { m_scene = scene; }

public Q_SLOTS:
    void processLogicFrameUpdates(const QVector<Qt3DCore::QNodeId> &nodeIds, float dt);

private:
    Qt3DCore::QScene *m_scene;
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_EXECUTOR_P_H
