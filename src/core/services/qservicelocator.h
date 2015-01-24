/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QSERVICELOCATOR_H
#define QT3D_QSERVICELOCATOR_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractServiceProviderPrivate;

class QT3DCORESHARED_EXPORT QAbstractServiceProvider
{
public:
    virtual ~QAbstractServiceProvider();

    int type() const;
    QString description() const;

protected:
    QAbstractServiceProvider(int type, const QString &description);
    QAbstractServiceProvider(QAbstractServiceProviderPrivate &dd);
    QScopedPointer<QAbstractServiceProviderPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractServiceProvider)
    Q_DECLARE_PRIVATE(QAbstractServiceProvider)
};

class QOpenGLInformationService;
class QSystemInformationService;
class QServiceLocatorPrivate;

class QT3DCORESHARED_EXPORT QServiceLocator
{
public:
    QServiceLocator();
    ~QServiceLocator();

    enum ServiceType {
        SystemInformation,
        OpenGLInformation,
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
        QAbstractServiceProvider *service = _q_getServiceHelper(serviceType);
        return static_cast<T *>(service);
    }

    // Convenience accessors for Qt3D provided services
    QSystemInformationService *systemInformation();
    QOpenGLInformationService *openGLInformation();

private:
    Q_DISABLE_COPY(QServiceLocator)
    Q_DECLARE_PRIVATE(QServiceLocator)
    QScopedPointer<QServiceLocatorPrivate> d_ptr;

    QAbstractServiceProvider *_q_getServiceHelper(int type);
};

}

QT_END_NAMESPACE

#endif // QT3D_QSERVICELOCATOR_H
