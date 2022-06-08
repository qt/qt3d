// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
