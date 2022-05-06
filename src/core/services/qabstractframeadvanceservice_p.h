/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire
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

#ifndef QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H
#define QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H

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

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QString>

#include <Qt3DCore/private/qservicelocator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractFrameAdvanceServicePrivate;

class Q_3DCORESHARED_EXPORT QAbstractFrameAdvanceService : public QAbstractServiceProvider
{
    Q_OBJECT
public:
    virtual qint64 waitForNextFrame() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

protected:
    QAbstractFrameAdvanceService(const QString &description = QString());
    QAbstractFrameAdvanceService(QAbstractFrameAdvanceServicePrivate &dd);
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QABSTRACTFRAMEADVANCESERVICE_P_H

