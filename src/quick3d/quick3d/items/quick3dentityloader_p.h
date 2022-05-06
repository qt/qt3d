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

#ifndef QT3D_QUICK_QUICK3DENTITYLOADER_P_H
#define QT3D_QUICK_QUICK3DENTITYLOADER_P_H

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

#include <Qt3DCore/QEntity>
#include <QtCore/QObject>
#include <QtCore/QUrl>

#include <Qt3DQuick/private/qt3dquick_global_p.h>


QT_BEGIN_NAMESPACE

class QQmlComponent;

namespace Qt3DCore {

class QEntity;

namespace Quick {

class Quick3DEntityLoaderPrivate;

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DEntityLoader : public QEntity
{
    Q_OBJECT
    Q_PROPERTY(QObject *entity READ entity NOTIFY entityChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QQmlComponent *sourceComponent READ sourceComponent WRITE setSourceComponent NOTIFY sourceComponentChanged QT3D_PROPERTY_REVISION(2, 12))
    QML_NAMED_ELEMENT(EntityLoader)
    QML_ADDED_IN_VERSION(2, 0)
public:
    enum Status {
        Null = 0,
        Loading,
        Ready,
        Error
    };
    Q_ENUM(Status)

    explicit Quick3DEntityLoader(QNode *parent = 0);
    ~Quick3DEntityLoader();

    QObject *entity() const;

    QUrl source() const;
    void setSource(const QUrl &url);

    QQmlComponent *sourceComponent() const;
    void setSourceComponent(QQmlComponent *components);

    Status status() const;

Q_SIGNALS:
    void entityChanged();
    void sourceChanged();
    void sourceComponentChanged();
    void statusChanged(Status status);

private:
    Q_DECLARE_PRIVATE(Quick3DEntityLoader)
    Q_PRIVATE_SLOT(d_func(), void _q_componentStatusChanged(QQmlComponent::Status))
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_QUICK3DENTITYLOADER_P_H
