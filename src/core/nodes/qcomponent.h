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

#ifndef QT3DCORE_COMPONENT_H
#define QT3DCORE_COMPONENT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QComponentPrivate;

class Q_3DCORESHARED_EXPORT QComponent : public QNode
{
    Q_OBJECT
    Q_PROPERTY(bool isShareable READ isShareable WRITE setShareable NOTIFY shareableChanged)

public:
    explicit QComponent(QNode *parent = nullptr);
    ~QComponent();

    bool isShareable() const;

    QVector<QEntity *> entities() const;

public Q_SLOTS:
    void setShareable(bool isShareable);

Q_SIGNALS:
    void shareableChanged(bool isShareable);
    void addedToEntity(QEntity *entity);
    void removedFromEntity(QEntity *entity);

protected:
    explicit QComponent(QComponentPrivate &dd, QNode *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QComponent)
};

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_COMPONENT_H
