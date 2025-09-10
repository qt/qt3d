// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DLOGIC_QFRAMEACTION_H
#define QT3DLOGIC_QFRAMEACTION_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DLogic/qt3dlogic_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

class QLogicAspect;
class QFrameActionPrivate;

namespace Logic {
class Executor;
}

class Q_3DLOGICSHARED_EXPORT QFrameAction : public Qt3DCore::QComponent
{
    Q_OBJECT

public:
    explicit QFrameAction(Qt3DCore::QNode *parent = nullptr);
    ~QFrameAction();

protected:
    QFrameAction(QFrameActionPrivate &dd, QNode *parent = nullptr);

Q_SIGNALS:
    void triggered(float dt);

private:
    Q_DECLARE_PRIVATE(QFrameAction)
    void onTriggered(float dt);

    friend class Logic::Executor;
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QFRAMEACTION_H
