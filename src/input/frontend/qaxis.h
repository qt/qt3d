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
****************************************************************************/

#ifndef QT3DINPUT_QAXIS_H
#define QT3DINPUT_QAXIS_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAxisPrivate;
class QAbstractAxisInput;

class Q_3DINPUTSHARED_EXPORT QAxis : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(float value READ value NOTIFY valueChanged)
public:
    explicit QAxis(Qt3DCore::QNode *parent = nullptr);
    ~QAxis();

    void addInput(QAbstractAxisInput *input);
    void removeInput(QAbstractAxisInput *input);
    QVector<QAbstractAxisInput *> inputs() const;

    float value() const;

Q_SIGNALS:
    void valueChanged(float value);

protected:
    // TODO Unused remove in Qt6
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) override;

private:
    Q_DECLARE_PRIVATE(QAxis)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QAXIS_H
