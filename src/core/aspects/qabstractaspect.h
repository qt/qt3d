/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DCORE_QABSTRACTASPECT_H
#define QT3DCORE_QABSTRACTASPECT_H

#include <QObject>
#include <QSharedPointer>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectEngine;
class QAspectJob;
class QAspectManager;
class QNode;
class QEntity;
class QAbstractAspectPrivate;
class QBackendNodeFunctor;

typedef QSharedPointer<QAspectJob> QAspectJobPtr;
typedef QSharedPointer<QBackendNodeFunctor> QBackendNodeFunctorPtr;

class QT3DCORESHARED_EXPORT QAbstractAspect : public QObject
{
    Q_OBJECT

public:
    explicit QAbstractAspect(QObject *parent = Q_NULLPTR);

protected:
    QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent = Q_NULLPTR);

    template<class Frontend>
    void registerBackendType(const QBackendNodeFunctorPtr &functor);
    void registerBackendType(const QMetaObject &, const QBackendNodeFunctorPtr &functor);

private:
    virtual QVariant executeCommand(const QStringList &args);

    virtual QVector<QAspectJobPtr> jobsToExecute(qint64 time) = 0;

    virtual void onInitialize(const QVariantMap &data) = 0;
    virtual void onCleanup() = 0;

    virtual void onStartup();
    virtual void onShutdown();

    virtual void onRootEntityChanged(QEntity *rootEntity);

    Q_DECLARE_PRIVATE(QAbstractAspect)
    friend class QAspectEngine;
    friend class QAspectManager;
};

template<class Frontend>
void QAbstractAspect::registerBackendType(const QBackendNodeFunctorPtr &functor)
{
    registerBackendType(Frontend::staticMetaObject, functor);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#define QT3D_REGISTER_NAMESPACED_ASPECT(name, AspectNamespace, AspectType) \
    QT_BEGIN_NAMESPACE \
    namespace Qt3DCore { \
        typedef QAbstractAspect *(*AspectCreateFunction)(QObject *); \
        QT3DCORESHARED_EXPORT void qt3d_QAspectFactory_addDefaultFactory(const QString &, const QMetaObject *, AspectCreateFunction); \
    } \
    QT_END_NAMESPACE \
    namespace { \
    QAbstractAspect *qt3d_ ## AspectType ## _createFunction(QObject *parent) \
    { \
        using namespace AspectNamespace; \
        return new AspectType(parent); \
    } \
    \
    void qt3d_ ## AspectType ## _registerFunction() \
    { \
        using namespace AspectNamespace; \
        qt3d_QAspectFactory_addDefaultFactory(QStringLiteral(name), &AspectType::staticMetaObject, qt3d_ ## AspectType ## _createFunction); \
    } \
    \
    Q_CONSTRUCTOR_FUNCTION(qt3d_ ## AspectType ## _registerFunction) \
    }

#define QT3D_REGISTER_ASPECT(name, AspectType) \
    QT3D_REGISTER_NAMESPACED_ASPECT(name, QT_PREPEND_NAMESPACE(Qt3DCore), AspectType)

#endif // QT3DCORE_ABSTRACTASPECT_H
