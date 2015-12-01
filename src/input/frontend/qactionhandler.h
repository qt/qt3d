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

#ifndef QT3DINPUT_QACTIONHANDLER_H
#define QT3DINPUT_QACTIONHANDLER_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QActionHandlerPrivate;
class QLogicalDevice;

class QT3DINPUTSHARED_EXPORT QActionHandler : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QLogicalDevice* logicalDevice READ logicalDevice WRITE setLogicalDevice NOTIFY logicalDeviceChanged)

public:
    QActionHandler(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QActionHandler();

    Qt3DInput::QLogicalDevice *logicalDevice() const;

public Q_SLOTS:
    void setLogicalDevice(Qt3DInput::QLogicalDevice *logicalDevice);

Q_SIGNALS:
    void logicalDeviceChanged(Qt3DInput::QLogicalDevice *logicalDevice);

protected:
    Q_DECLARE_PRIVATE(QActionHandler)
    QActionHandler(QActionHandlerPrivate &dd, Qt3DCore::QNode *parent = 0);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    QT3D_CLONEABLE(QActionHandler)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTIONHANDLER_H
