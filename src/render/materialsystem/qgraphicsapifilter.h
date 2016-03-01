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

#ifndef QT3DRENDER_QGRAPHICSAPIFILTER_H
#define QT3DRENDER_QGRAPHICSAPIFILTER_H

#include <QObject>
#include <QStringList>
#include <Qt3DRender/qt3drender_global.h>
#include <QSurfaceFormat>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QGraphicsApiFilterPrivate;

class QT3DRENDERSHARED_EXPORT QGraphicsApiFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QGraphicsApiFilter::Api api READ api WRITE setApi NOTIFY apiChanged)
    Q_PROPERTY(Qt3DRender::QGraphicsApiFilter::Profile profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(int minorVersion READ minorVersion WRITE setMinorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(QStringList extensions READ extensions WRITE setExtensions NOTIFY extensionsChanged)
    Q_PROPERTY(QString vendor READ vendor WRITE setVendor NOTIFY vendorChanged)

public:

    enum Api
    {
        OpenGLES = QSurfaceFormat::OpenGLES,
        OpenGL = QSurfaceFormat::OpenGL
    };
    Q_ENUM(Api)

    enum Profile
    {
        NoProfile = QSurfaceFormat::NoProfile,
        CoreProfile = QSurfaceFormat::CoreProfile,
        CompatibilityProfile = QSurfaceFormat::CompatibilityProfile
    };
    Q_ENUM(Profile)

    explicit QGraphicsApiFilter(QObject *parent = Q_NULLPTR);

    void copy(const QGraphicsApiFilter &ref);

    Api api() const;
    Profile profile() const;
    int minorVersion() const;
    int majorVersion() const;
    QStringList extensions() const;
    QString vendor() const;

public Q_SLOTS:
    void setApi(Api api);
    void setProfile(Profile profile);
    void setMinorVersion(int minorVersion);
    void setMajorVersion(int majorVersion);
    void setExtensions(const QStringList &extensions);
    void setVendor(const QString &vendor);

Q_SIGNALS:
    void apiChanged(Api api);
    void profileChanged(Profile profile);
    void minorVersionChanged(int minorVersion);
    void majorVersionChanged(int majorVersion);
    void extensionsChanged(const QStringList &extensions);
    void vendorChanged(const QString &vendor);
    void graphicsApiFilterChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsApiFilter)
};

bool operator ==(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample);
bool operator !=(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample);

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGRAPHICSAPIFILTER_H
