/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QOPENGLFILTER_H
#define QT3D_QOPENGLFILTER_H

#include <QObject>
#include <QStringList>
#include <Qt3DRenderer/qt3drenderer_global.h>
#include <QSurfaceFormat>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QOpenGLFilterPrivate;

class QT3DRENDERERSHARED_EXPORT QOpenGLFilter : public QObject
{
    Q_OBJECT
    Q_ENUMS(Api)
    Q_ENUMS(Profile)
    Q_PROPERTY(Qt3D::QOpenGLFilter::Api api READ api WRITE setApi NOTIFY apiChanged)
    Q_PROPERTY(Qt3D::QOpenGLFilter::Profile profile READ profile WRITE setProfile NOTIFY profileChanged)
    Q_PROPERTY(int minorVersion READ minorVersion WRITE setMinorVersion NOTIFY minorVersionChanged)
    Q_PROPERTY(int majorVersion READ majorVersion WRITE setMajorVersion NOTIFY majorVersionChanged)
    Q_PROPERTY(QStringList extensions READ extensions WRITE setExtensions NOTIFY extensionsChanged)
    Q_PROPERTY(QString vendor READ vendor WRITE setVendor NOTIFY vendorChanged)

public:

    enum Api
    {
        ES = QSurfaceFormat::OpenGLES,
        Desktop = QSurfaceFormat::OpenGL
    };

    enum Profile
    {
        None = QSurfaceFormat::NoProfile,
        Core = QSurfaceFormat::CoreProfile,
        Compatibility = QSurfaceFormat::CompatibilityProfile
    };

    explicit QOpenGLFilter(QObject *parent = 0);

    void copy(const QOpenGLFilter *ref);

    Api api() const;
    Profile profile() const;
    int minorVersion() const;
    int majorVersion() const;
    QStringList extensions() const;
    QString vendor() const;

    void setApi(Api api);
    void setProfile(Profile profile);
    void setMinorVersion(int minorVersion);
    void setMajorVersion(int majorVersion);
    void setExtensions(const QStringList &extensions);
    void setVendor(const QString &vendor);

Q_SIGNALS:
    void apiChanged();
    void profileChanged();
    void minorVersionChanged();
    void majorVersionChanged();
    void extensionsChanged();
    void vendorChanged();
    void openGLFilterChanged();

private:
    Q_DECLARE_PRIVATE(QOpenGLFilter)
};

bool operator ==(const QOpenGLFilter &reference, const QOpenGLFilter &sample);
bool operator !=(const QOpenGLFilter &reference, const QOpenGLFilter &sample);

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QOpenGLFilter *)

#endif // QT3D_QOPENGLFILTER_H
