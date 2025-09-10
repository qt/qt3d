// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DCORE_QABSTRACTASPECT_H
#define QT3DCORE_QABSTRACTASPECT_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnodeid.h>
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAspectEngine;
class QAspectJob;
class QAspectManager;
class QNode;
class QEntity;
class QAbstractAspectPrivate;
class QBackendNodeMapper;

typedef QSharedPointer<QAspectJob> QAspectJobPtr;
typedef QSharedPointer<QBackendNodeMapper> QBackendNodeMapperPtr;

class Q_3DCORESHARED_EXPORT QAbstractAspect : public QObject
{
    Q_OBJECT

public:
    explicit QAbstractAspect(QObject *parent = nullptr);
    ~QAbstractAspect();

    void scheduleSingleShotJob(const Qt3DCore::QAspectJobPtr &job);
    virtual QStringList dependencies() const;

protected:
    explicit QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent = nullptr);

    QNodeId rootEntityId() const noexcept;

    template<class Frontend>
    void registerBackendType(const QBackendNodeMapperPtr &functor);
    void registerBackendType(const QMetaObject &obj, const QBackendNodeMapperPtr &functor);
    template<class Frontend>
    void unregisterBackendType();
    void unregisterBackendType(const QMetaObject &);

private:
    virtual QVariant executeCommand(const QStringList &args);

    virtual std::vector<QAspectJobPtr> jobsToExecute(qint64 time);

    virtual void onRegistered();
    virtual void onUnregistered();

    virtual void onEngineStartup();
    virtual void onEngineShutdown();

    virtual void jobsDone();
    virtual void frameDone();

    Q_DECLARE_PRIVATE(QAbstractAspect)
    friend class QAspectEngine;
    friend class QAspectManager;
    friend class QScheduler;
};

template<class Frontend>
void QAbstractAspect::registerBackendType(const QBackendNodeMapperPtr &functor)
{
    registerBackendType(Frontend::staticMetaObject, functor);
}

template<class Frontend>
void QAbstractAspect::unregisterBackendType()
{
    unregisterBackendType(Frontend::staticMetaObject);
}

} // namespace Qt3DCore

QT_END_NAMESPACE

#define QT3D_REGISTER_NAMESPACED_ASPECT(name, AspectNamespace, AspectType) \
    QT_BEGIN_NAMESPACE \
    namespace Qt3DCore { \
        typedef QAbstractAspect *(*AspectCreateFunction)(QObject *); \
        Q_3DCORESHARED_EXPORT void qt3d_QAspectFactory_addDefaultFactory(const QLatin1String &, const QMetaObject *, AspectCreateFunction); \
    } \
    QT_END_NAMESPACE \
    namespace { \
    Qt3DCore::QAbstractAspect *qt3d_ ## AspectType ## _createFunction(QObject *parent) \
    { \
        using namespace AspectNamespace; \
        return new AspectType(parent); \
    } \
    \
    void qt3d_ ## AspectType ## _registerFunction() \
    { \
        using namespace AspectNamespace; \
        qt3d_QAspectFactory_addDefaultFactory(QLatin1String(name), &AspectType::staticMetaObject, qt3d_ ## AspectType ## _createFunction); \
    } \
    \
    Q_CONSTRUCTOR_FUNCTION(qt3d_ ## AspectType ## _registerFunction) \
    }

#define QT3D_REGISTER_ASPECT(name, AspectType) \
    QT3D_REGISTER_NAMESPACED_ASPECT(name, QT_PREPEND_NAMESPACE(Qt3DCore), AspectType)

#endif // QT3DCORE_ABSTRACTASPECT_H
