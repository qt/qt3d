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
******************************************************************************/

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
