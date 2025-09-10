// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QCOREASPECT_H
#define QT3DCORE_QCOREASPECT_H

#include <Qt3DCore/qabstractaspect.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QCoreAspectPrivate;

class Q_3DCORESHARED_EXPORT QCoreAspect : public Qt3DCore::QAbstractAspect
{
    Q_OBJECT
public:
    explicit QCoreAspect(QObject *parent = nullptr);
    ~QCoreAspect();

    QAspectJobPtr calculateBoundingVolumeJob() const;

protected:
    Q_DECLARE_PRIVATE(QCoreAspect)

private:
    std::vector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) override;
    QVariant executeCommand(const QStringList &args) override;
    void onRegistered() override;
    void onUnregistered() override;
    void onEngineStartup() override;
    void frameDone() override;
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QCOREASPECT_H
