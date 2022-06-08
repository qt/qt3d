// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    void onRegistered() override;
    void onEngineStartup() override;

    Q_DECLARE_PRIVATE(QLogicAspect)
    explicit QLogicAspect(QLogicAspectPrivate &dd, QObject *parent);
};

} // namespace Qt3DLogic

QT_END_NAMESPACE

#endif // QT3DLOGIC_QLOGICASPECT_H
