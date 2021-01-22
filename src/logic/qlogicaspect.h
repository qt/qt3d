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

#ifndef QT3DLOGIC_QLOGICASPECT_H
#define QT3DLOGIC_QLOGICASPECT_H

#include <Qt3DLogic/qt3dlogic_global.h>
#include <Qt3DCore/qabstractaspect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DLogic {

class QLogicAspectPrivate;

class Q_3DLOGICSHARED_EXPORT QLogicAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit QLogicAspect(QObject *parent = nullptr);
    ~QLogicAspect();

private:
    QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    void onRegistered() override;
    void onEngineStartup() override;

    Q_DECLARE_PRIVATE(QLogicAspect)
    explicit QLogicAspect(QLogicAspectPrivate &dd, QObject *parent);
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QLOGICASPECT_H
