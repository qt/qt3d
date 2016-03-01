/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DINPUT_QINPUTDEVICEINTEGRATION_H
#define QT3DINPUT_QINPUTDEVICEINTEGRATION_H

#include <QObject>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qabstractphysicaldevicebackendnode.h>

#include <Qt3DCore/qaspectjob.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
class QBackendNodeFunctor;
typedef QSharedPointer<QBackendNodeFunctor> QBackendNodeFunctorPtr;
}


namespace Qt3DInput {

class QInputAspect;
class QAbstractPhysicalDevice;
class QInputDeviceIntegrationPrivate;

class QT3DINPUTSHARED_EXPORT QInputDeviceIntegration : public QObject
{
    Q_OBJECT
protected:
    explicit QInputDeviceIntegration(QObject *parent = Q_NULLPTR);
    explicit QInputDeviceIntegration(QInputDeviceIntegrationPrivate &dd, QObject *parent = Q_NULLPTR);

    template<class Frontend>
    void registerBackendType(const Qt3DCore::QBackendNodeFunctorPtr &functor)
    {
        registerBackendType(Frontend::staticMetaObject, functor);
    }

    void registerBackendType(const QMetaObject &metaObject, const Qt3DCore::QBackendNodeFunctorPtr &functor);

public:
    void initialize(Qt3DInput::QInputAspect *aspect);

    virtual QVector<Qt3DCore::QAspectJobPtr> jobsToExecute(qint64 time) = 0;
    virtual QAbstractPhysicalDevice *createPhysicalDevice(const QString &name) = 0;
    virtual QVector<Qt3DCore::QNodeId> physicalDevices() const = 0;
    virtual QAbstractPhysicalDeviceBackendNode *physicalDevice(Qt3DCore::QNodeId id) const = 0;

protected:
    QInputAspect *inputAspect() const;

private:
    virtual void onInitialize() = 0;

    Q_DECLARE_PRIVATE(QInputDeviceIntegration)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTDEVICEINTEGRATION_H
