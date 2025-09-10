// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
