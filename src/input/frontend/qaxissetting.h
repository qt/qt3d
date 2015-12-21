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

#ifndef QT3DINPUT_QAXISSETTING_H
#define QT3DINPUT_QAXISSETTING_H

#include <Qt3DCore/qnode.h>
#include <Qt3DInput/qt3dinput_global.h>

#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisSettingPrivate;

class QT3DINPUTSHARED_EXPORT QAxisSetting : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float deadZone READ deadZone WRITE setDeadZone NOTIFY deadZoneChanged)
    Q_PROPERTY(QVariantList axes READ axes WRITE setAxes NOTIFY axesChanged)
    Q_PROPERTY(bool filter READ isFilterEnabled WRITE setFilterEnabled NOTIFY filterChanged)

public:
    explicit QAxisSetting(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QAxisSetting();

    float deadZone() const;
    QVariantList axes() const;
    bool isFilterEnabled() const;

public Q_SLOTS:
    void setDeadZone(float deadZone);
    void setAxes(const QVariantList &axes);
    void setFilterEnabled(bool enabled);

Q_SIGNALS:
    void deadZoneChanged(float deadZone);
    void axesChanged(QVariantList axes);
    void filterChanged(bool filter);

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QAxisSetting)
    QT3D_CLONEABLE(QAxisSetting)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXISSETTING_H
