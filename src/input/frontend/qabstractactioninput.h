// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DINPUT_QABSTRACTACTIONINPUT_H
#define QT3DINPUT_QABSTRACTACTIONINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractActionInputPrivate;

class Q_3DINPUTSHARED_EXPORT QAbstractActionInput : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~QAbstractActionInput();

protected:
    explicit QAbstractActionInput(QAbstractActionInputPrivate &dd, Qt3DCore::QNode *parent = nullptr);
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QABSTRACTACTIONINPUT_H
