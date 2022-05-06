/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QUICK_QQMLASPECTENGINE_P_H
#define QT3D_QUICK_QQMLASPECTENGINE_P_H

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

#include <Qt3DQuick/qt3dquick_global.h>
#include <Qt3DQuick/qqmlaspectengine.h>
#include <QtCore/QScopedPointer>

#include <QtCore/private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class QQmlAspectEnginePrivate : public QObjectPrivate
{
public:
    QQmlAspectEnginePrivate();

    Q_DECLARE_PUBLIC(QQmlAspectEngine)

    QScopedPointer<QQmlEngine> m_qmlEngine;
    QScopedPointer<QAspectEngine> m_aspectEngine;
    QQmlComponent *m_component;

private:
    void _q_continueExecute();
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QQMLASPECTENGINE_P_H
