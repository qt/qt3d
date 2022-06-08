// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DCORE_QSERVICELOCATOR_H
#define QT3DCORE_QSERVICELOCATOR_H

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
#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <private/qglobal_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QAbstractServiceProviderPrivate;

class Q_3DCORESHARED_EXPORT QAbstractServiceProvider : public QObject
{
    Q_OBJECT
public:
    virtual ~QAbstractServiceProvider();

    int type() const;
    QString description() const;

protected:
    explicit QAbstractServiceProvider(int type, const QString &description, QObject* parent = nullptr);
    explicit QAbstractServiceProvider(QAbstractServiceProviderPrivate &dd, QObject* parent = nullptr);

private:
    Q_DISABLE_COPY(QAbstractServiceProvider)
    Q_DECLARE_PRIVATE(QAbstractServiceProvider)
};

class QAbstractFrameAdvanceService;
class QOpenGLInformationService;
class QSystemInformationService;
class QServiceLocatorPrivate;
class QEventFilterService;
class QDownloadHelperService;
class QAspectEngine;

class Q_3DCORESHARED_EXPORT QServiceLocator
{
public:
    QServiceLocator(QAspectEngine *aspectEngine = nullptr);
    ~QServiceLocator();

    enum ServiceType {
        SystemInformation,
        OpenGLInformation,
        CollisionService,
        FrameAdvanceService,
        EventFilterService,
        DownloadHelperService,
#if !defined(Q_QDOC)
        DefaultServiceCount, // Add additional default services before here
#endif
        UserService = 256
    };

    void registerServiceProvider(int serviceType, QAbstractServiceProvider *provider);
    void unregisterServiceProvider(int serviceType);

    int serviceCount() const;

    template<class T>
    T *service(int serviceType)
    {
        QAbstractServiceProvider *service_ = _q_getServiceHelper(serviceType);
        return static_cast<T *>(service_);
    }

    // Convenience accessors for Qt3D provided services
    QSystemInformationService *systemInformation();
    QOpenGLInformationService *openGLInformation();
    QAbstractFrameAdvanceService *frameAdvanceService();
    QEventFilterService *eventFilterService();
    QDownloadHelperService *downloadHelperService();

private:
    Q_DISABLE_COPY(QServiceLocator)
    Q_DECLARE_PRIVATE(QServiceLocator)
    QScopedPointer<QServiceLocatorPrivate> d_ptr;

    QAbstractServiceProvider *_q_getServiceHelper(int type);
};

}

QT_END_NAMESPACE

#endif // QT3DCORE_QSERVICELOCATOR_H
