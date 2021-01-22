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

#ifndef QT3DLOGIC_LOGIC_CALLBACKJOB_P_H
#define QT3DLOGIC_LOGIC_CALLBACKJOB_P_H

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
#include <Qt3DCore/private/qaspectjob_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {
namespace Logic {

class Manager;

class CallbackJob;

class CallbackJobPrivate : public Qt3DCore::QAspectJobPrivate
{
public:
    explicit CallbackJobPrivate(CallbackJob *q);

    bool isRequired() const override;
    void postFrame(Qt3DCore::QAspectManager *manager) override;

    Q_DECLARE_PUBLIC(CallbackJob)
private:
    CallbackJob *q_ptr;
};

class CallbackJob : public Qt3DCore::QAspectJob
{
public:
    CallbackJob();
    void setManager(Manager *manager);

    void run() override;

private:
    Manager *m_logicManager;
    Q_DECLARE_PRIVATE(CallbackJob)
};

} // namespace Logic
} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_LOGIC_CALLBACKJOB_P_H
