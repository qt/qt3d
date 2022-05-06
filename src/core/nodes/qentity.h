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

#ifndef QT3DCORE_QENTITY_H
#define QT3DCORE_QENTITY_H

#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QMetaType>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QComponent;
class QEntityPrivate;

using QComponentVector = QList<QComponent*>;

class Q_3DCORESHARED_EXPORT QEntity : public QNode
{
    Q_OBJECT
public:
    explicit QEntity(QNode *parent = nullptr);
    virtual ~QEntity();

    QComponentVector components() const;

    template<class T>
    QList<T *> componentsOfType() const
    {
        QList<T*> matchComponents;
        const QComponentVector comps = this->components();
        for (QComponent *component : comps) {
            T *typedComponent = qobject_cast<T*>(component);
            if (typedComponent != nullptr)
                matchComponents.append(typedComponent);
        }
        return matchComponents;
    }

    void addComponent(QComponent *comp);
    void removeComponent(QComponent *comp);

    QEntity *parentEntity() const;

protected:
    explicit QEntity(QEntityPrivate &dd, QNode *parent = nullptr);

private Q_SLOTS:
    void onParentChanged(QObject *);

private:
    Q_DECLARE_PRIVATE(QEntity)
};

typedef QSharedPointer<QEntity> QEntityPtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QENTITY_H
