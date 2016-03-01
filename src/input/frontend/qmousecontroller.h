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

#ifndef QT3DINPUT_QMOUSECONTROLLER_H
#define QT3DINPUT_QMOUSECONTROLLER_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qabstractphysicaldevice.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QMouseControllerPrivate;
class QMouseInput;

class QT3DINPUTSHARED_EXPORT QMouseController : public Qt3DInput::QAbstractPhysicalDevice
{
    Q_OBJECT
    Q_PROPERTY(float sensitivity READ sensitivity WRITE setSensitivity NOTIFY sensitivityChanged)
public:
    explicit QMouseController(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~QMouseController();

    enum Axis {
        X,
        Y
    };
    Q_ENUM(Axis)

    enum Button {
        Left,
        Center,
        Right
    };
    Q_ENUM(Button)

    int axisCount() const Q_DECL_FINAL;
    int buttonCount() const Q_DECL_FINAL;
    QStringList axisNames() const Q_DECL_FINAL;
    QStringList buttonNames() const Q_DECL_FINAL;
    int axisIdentifier(const QString &name) const Q_DECL_FINAL;
    int buttonIdentifier(const QString &name) const Q_DECL_FINAL;

    float sensitivity() const;

public Q_SLOTS:
    void setSensitivity(float value);

Q_SIGNALS:
    void sensitivityChanged(float value);

protected:
    QMouseController(QMouseControllerPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QMouseController)
    QT3D_CLONEABLE(QMouseController)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DInput::QMouseController*)

#endif // QT3DINPUT_QMOUSECONTROLLER_H
