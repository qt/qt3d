// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
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
#include <private/qglobal_p.h>

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
    void processLogicFrameUpdates(const QList<Qt3DCore::QNodeId> &nodeIds, float dt);

private:
    Qt3DCore::QScene *m_scene;
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_EXECUTOR_P_H
