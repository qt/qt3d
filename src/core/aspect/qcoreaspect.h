/****************************************************************************
**
** Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
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
