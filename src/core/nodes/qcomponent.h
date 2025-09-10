// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_COMPONENT_H
#define QT3DCORE_COMPONENT_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QEntity;
class QComponentPrivate;

class Q_3DCORESHARED_EXPORT QComponent : public QNode
{
    Q_OBJECT
    Q_PROPERTY(bool isShareable READ isShareable WRITE setShareable NOTIFY shareableChanged)

public:
    explicit QComponent(QNode *parent = nullptr);
    ~QComponent();

    bool isShareable() const;

    QList<QEntity *> entities() const;

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
