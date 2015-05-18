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

#include "qopenglfilter.h"
#include "private/qobject_p.h"
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QOpenGLFilterPrivate : public QObjectPrivate
{
public:
    QOpenGLFilterPrivate()
        : QObjectPrivate()
        , m_api(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL ? QOpenGLFilter::Desktop : QOpenGLFilter::ES)
        , m_profile(QOpenGLFilter::None) // matches all (no profile, core, compat)
        , m_minor(0)
        , m_major(0)
    {
    }

    Q_DECLARE_PUBLIC(QOpenGLFilter)
    QOpenGLFilter::Api m_api;
    QOpenGLFilter::Profile m_profile;
    int m_minor;
    int m_major;
    QStringList m_extensions;
    QString m_vendor;
};

QOpenGLFilter::QOpenGLFilter(QObject *parent)
    : QObject(*new QOpenGLFilterPrivate, parent)
{
}

void QOpenGLFilter::copy(const QOpenGLFilter &ref)
{
    Q_D(QOpenGLFilter);
    d->m_api = ref.api();
    d->m_profile = ref.profile();
    d->m_major = ref.majorVersion();
    d->m_minor = ref.minorVersion();
    d->m_extensions = ref.extensions();
    d->m_vendor = ref.vendor();
}

QOpenGLFilter::Api QOpenGLFilter::api() const
{
    Q_D(const QOpenGLFilter);
    return d->m_api;
}

QOpenGLFilter::Profile QOpenGLFilter::profile() const
{
    Q_D(const QOpenGLFilter);
    return d->m_profile;
}

int QOpenGLFilter::minorVersion() const
{
    Q_D(const QOpenGLFilter);
    return d->m_minor;
}

int QOpenGLFilter::majorVersion() const
{
    Q_D(const QOpenGLFilter);
    return d->m_major;
}

QStringList QOpenGLFilter::extensions() const
{
    Q_D(const QOpenGLFilter);
    return d->m_extensions;
}

QString QOpenGLFilter::vendor() const
{
    Q_D(const QOpenGLFilter);
    return d->m_vendor;
}

void QOpenGLFilter::setApi(QOpenGLFilter::Api api)
{
    Q_D(QOpenGLFilter);
    if (d->m_api != api) {
        d->m_api = api;
        emit apiChanged();
        emit openGLFilterChanged();
    }
}

void QOpenGLFilter::setProfile(QOpenGLFilter::Profile profile)
{
    Q_D(QOpenGLFilter);
    if (d->m_profile != profile) {
        d->m_profile = profile;
        emit profileChanged();
        emit openGLFilterChanged();
    }
}

void QOpenGLFilter::setMinorVersion(int minorVersion)
{
    Q_D(QOpenGLFilter);
    if (minorVersion != d->m_minor) {
        d->m_minor = minorVersion;
        emit minorVersionChanged();
        emit openGLFilterChanged();
    }
}

void QOpenGLFilter::setMajorVersion(int majorVersion)
{
    Q_D(QOpenGLFilter);
    if (d->m_major != majorVersion) {
        d->m_major = majorVersion;
        emit majorVersionChanged();
        emit openGLFilterChanged();
    }
}

void QOpenGLFilter::setExtensions(const QStringList &extensions)
{
    Q_D(QOpenGLFilter);
    if (d->m_extensions != extensions) {
        d->m_extensions = extensions;
        emit extensionsChanged();
        emit openGLFilterChanged();
    }
}

void QOpenGLFilter::setVendor(const QString &vendor)
{
    Q_D(QOpenGLFilter);
    if (d->m_vendor != vendor) {
        d->m_vendor = vendor;
        emit vendorChanged();
        emit openGLFilterChanged();
    }
}

bool operator ==(const QOpenGLFilter &reference, const QOpenGLFilter &sample)
{
    if (sample.api() == reference.api()
        && sample.profile() <= reference.profile()
        && (sample.majorVersion() < reference.majorVersion()
            || (sample.majorVersion() == reference.majorVersion() && sample.minorVersion() <= reference.minorVersion()))) {
        Q_FOREACH (const QString &neededExt, sample.extensions())
            if (!reference.extensions().contains(neededExt))
                return false;
        // If a vendor name was specified in sample, we perform comparison,
        // otherwise we assume the vendor name doesn't matter
        if (!sample.vendor().isEmpty())
            return (sample.vendor() == reference.vendor());
        return true;
    }
    return false;
}

bool operator !=(const QOpenGLFilter &reference, const QOpenGLFilter &sample)
{
    return !(reference == sample);
}

} // Qt3D

QT_END_NAMESPACE
