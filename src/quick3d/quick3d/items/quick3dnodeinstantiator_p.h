// Copyright (C) 2013 Research In Motion.
// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3D_QUICK_NODEINSTANTIATOR_P_H
#define QT3D_QUICK_NODEINSTANTIATOR_P_H

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

#include <Qt3DCore/qnode.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlparserstatus.h>

#include <Qt3DQuick/private/qt3dquick_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

class Quick3DNodeInstantiatorPrivate;

class Q_3DQUICKSHARED_PRIVATE_EXPORT Quick3DNodeInstantiator : public QNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool asynchronous READ isAsync WRITE setAsync NOTIFY asynchronousChanged)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QObject *object READ object NOTIFY objectChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")
    QML_NAMED_ELEMENT(NodeInstantiator)
    QML_ADDED_IN_VERSION(2, 0)

public:
    Quick3DNodeInstantiator(QNode *parent = 0);

    bool isActive() const;
    void setActive(bool newVal);

    bool isAsync() const;
    void setAsync(bool newVal);

    int count() const;

    QQmlComponent *delegate();
    void setDelegate(QQmlComponent *c);

    QVariant model() const;
    void setModel(const QVariant &v);

    QObject *object() const;
    Q_INVOKABLE QObject *objectAt(int index) const;

    void classBegin() override;
    void componentComplete() override;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void countChanged();
    void objectChanged();
    void activeChanged();
    void asynchronousChanged();

    void objectAdded(int index, QObject *object);
    void objectRemoved(int index, QObject *object);

private slots:
    void onParentChanged(QObject *parent);

private:
    Q_DISABLE_COPY(Quick3DNodeInstantiator)
    Q_DECLARE_PRIVATE(Quick3DNodeInstantiator)
    Q_PRIVATE_SLOT(d_func(), void _q_createdItem(int, QObject *))
    Q_PRIVATE_SLOT(d_func(), void _q_modelUpdated(const QQmlChangeSet &, bool))
};

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3D_QUICK_NODEINSTANTIATOR_P_H
