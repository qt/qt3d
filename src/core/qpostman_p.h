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
****************************************************************************/

#ifndef QT3DCORE_QPOSTMAN_P_H
#define QT3DCORE_QPOSTMAN_P_H

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

#include <Qt3DCore/private/qobserverinterface_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QScene;
class QPostmanPrivate;

class Q_3DCORE_PRIVATE_EXPORT QAbstractPostman : public QObserverInterface
{
public:
    virtual void setScene(QScene *sceneLookup) = 0;
    virtual void notifyBackend(const QSceneChangePtr &change) = 0;
    virtual bool shouldNotifyFrontend(const QSceneChangePtr &change) = 0;
};

class Q_3DCORE_PRIVATE_EXPORT QPostman final
        : public QObject
        , public QAbstractPostman
{
    Q_OBJECT
public:
    explicit QPostman(QObject *parent = nullptr);
    ~QPostman();

    void setScene(QScene *sceneLookup) final;
    void sceneChangeEvent(const QSceneChangePtr &e) final;
    void notifyBackend(const QSceneChangePtr &change) final;
    bool shouldNotifyFrontend(const QSceneChangePtr &e) final;

public Q_SLOTS:
    void submitChangeBatch();

private:
    Q_DECLARE_PRIVATE(QPostman)
    Q_INVOKABLE void notifyFrontendNode(const QSceneChangePtr &e);

};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DCore::QAbstractPostman*) // LCOV_EXCL_LINE

#endif // QT3DCORE_QPOSTMAN_P_H
