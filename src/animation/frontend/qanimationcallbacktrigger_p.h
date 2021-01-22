/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DANIMATION_QANIMATIONCALLBACKTRIGGER_P_H
#define QT3DANIMATION_QANIMATIONCALLBACKTRIGGER_P_H

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

#include <Qt3DAnimation/qt3danimation_global.h>
#include <Qt3DAnimation/qanimationcallback.h>
#include <Qt3DCore/qscenechange.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class Q_AUTOTEST_EXPORT QAnimationCallbackTrigger : public Qt3DCore::QSceneChange
{
public:
    QAnimationCallbackTrigger(Qt3DCore::QNodeId subjectId);

    void setCallback(QAnimationCallback *callback) { m_callback = callback; }
    QAnimationCallback *callback() const { return m_callback; }

    void setValue(const QVariant &value) { m_value = value; }
    QVariant value() const { return m_value; }

private:
    QAnimationCallback *m_callback;
    QVariant m_value;
};

typedef QSharedPointer<QAnimationCallbackTrigger> QAnimationCallbackTriggerPtr;

} // namespace Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCALLBACKTRIGGER_P_H
