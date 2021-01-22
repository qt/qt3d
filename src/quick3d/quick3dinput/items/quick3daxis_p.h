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
****************************************************************************/

#ifndef QT3DINPUT_INPUT_QUICK_QUICK3DAXIS_H
#define QT3DINPUT_INPUT_QUICK_QUICK3DAXIS_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DInput/QAbstractAxisInput>
#include <Qt3DInput/QAxis>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickInput/private/qt3dquickinput_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {
namespace Quick {

class Q_3DQUICKINPUTSHARED_PRIVATE_EXPORT Quick3DAxis : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DInput::QAbstractAxisInput> inputs READ qmlAxisInputs CONSTANT)
    Q_CLASSINFO("DefaultProperty", "inputs")
public:
    explicit Quick3DAxis(QObject *parent = nullptr);

    inline QAxis *parentAxis() const { return qobject_cast<QAxis *>(parent()); }
    QQmlListProperty<QAbstractAxisInput> qmlAxisInputs();

private:
    static void appendAxisInput(QQmlListProperty<QAbstractAxisInput> *list, QAbstractAxisInput *input);
    static QAbstractAxisInput *axisInputAt(QQmlListProperty<QAbstractAxisInput> *list, int index);
    static int axesInputCount(QQmlListProperty<QAbstractAxisInput> *list);
    static void clearAxisInputs(QQmlListProperty<QAbstractAxisInput> *list);
};

} // namespace Quick
} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE


#endif // QT3DINPUT_INPUT_QUICK_QUICK3DAXIS_H
