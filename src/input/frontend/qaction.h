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
******************************************************************************/

#ifndef QT3DINPUT_QACTION_H
#define QT3DINPUT_QACTION_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QActionPrivate;
class QAbstractActionInput;

class Q_3DINPUTSHARED_EXPORT QAction : public Qt3DCore::QNode
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
public:
    explicit QAction(Qt3DCore::QNode *parent = nullptr);
    ~QAction();

    bool isActive() const;

    void addInput(QAbstractActionInput *input);
    void removeInput(QAbstractActionInput *input);
    QList<QAbstractActionInput *> inputs() const;

Q_SIGNALS:
    void activeChanged(bool isActive);

private:
    Q_DECLARE_PRIVATE(QAction)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QACTION_H
