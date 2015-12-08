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

#include "qgraphicsapifilter.h"
#include "private/qobject_p.h"
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QGraphicsApiFilterPrivate : public QObjectPrivate
{
public:
    QGraphicsApiFilterPrivate()
        : QObjectPrivate()
        , m_api(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL ? QGraphicsApiFilter::OpenGL : QGraphicsApiFilter::OpenGLES)
        , m_profile(QGraphicsApiFilter::NoProfile) // matches all (no profile, core, compat)
        , m_minor(0)
        , m_major(0)
    {
    }

    Q_DECLARE_PUBLIC(QGraphicsApiFilter)
    QGraphicsApiFilter::Api m_api;
    QGraphicsApiFilter::Profile m_profile;
    int m_minor;
    int m_major;
    QStringList m_extensions;
    QString m_vendor;
};

/*!
    \class Qt3DRender::QGraphicsApiFilter
    \inmodule Qt3DRender
    \since 5.5
    \brief The QGraphicsApiFilter class provides ...
*/

/*!
    \qmltype GraphicsApiFilter
    \instantiates Qt3DRender::QGraphicsApiFilter
    \inherits QtObject
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For OpenGL ...
*/

/*! \fn Qt3DRender::QGraphicsApiFilter::QGraphicsApiFilter(QObject *parent)
  Constructs a new QGraphicsApiFilter with the specified \a parent.
 */
QGraphicsApiFilter::QGraphicsApiFilter(QObject *parent)
    : QObject(*new QGraphicsApiFilterPrivate, parent)
{
}

/*! \fn void Qt3DRender::QGraphicsApiFilter::copy(const QGraphicsApiFilter &ref)
  Copies the \a ref instance into this one.
 */
void QGraphicsApiFilter::copy(const QGraphicsApiFilter &ref)
{
    Q_D(QGraphicsApiFilter);
    d->m_api = ref.api();
    d->m_profile = ref.profile();
    d->m_major = ref.majorVersion();
    d->m_minor = ref.minorVersion();
    d->m_extensions = ref.extensions();
    d->m_vendor = ref.vendor();
}

/*!
  \enum Qt3DRender::QGraphicsApiFilter::Api

  \value OpenGLES QSurfaceFormat::OpenGLES
  \value OpenGL QSurfaceFormat::OpenGL

*/

/*!
  \enum Qt3DRender::QGraphicsApiFilter::Profile

  \value NoProfile QSurfaceFormat::NoProfile
  \value CoreProfile QSurfaceFormat::CoreProfile
  \value CompatibilityProfile QSurfaceFormat::CompatibilityProfile

*/

/*!
  \property Qt3DRender::QGraphicsApiFilter::api

*/

/*!
  \qmlproperty enumeration Qt3D.Render::GraphicsApiFilter::api


  \value OpenGLES QSurfaceFormat::OpenGLES
  \value OpenGL QSurfaceFormat::OpenGL
*/

QGraphicsApiFilter::Api QGraphicsApiFilter::api() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_api;
}

/*!
  \property Qt3DRender::QGraphicsApiFilter::profile

*/

/*!
  \qmlproperty enumeration Qt3D.Render::GraphicsApiFilter::profile

  \value NoProfile QSurfaceFormat::NoProfile
  \value CoreProfile QSurfaceFormat::CoreProfile
  \value CompatibilityProfile QSurfaceFormat::CompatibilityProfile
*/

QGraphicsApiFilter::Profile QGraphicsApiFilter::profile() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_profile;
}

/*!
  \property Qt3DRender::QGraphicsApiFilter::minorVersion

 */

/*!
  \qmlproperty int Qt3D.Render::GraphicsApiFilter::minorVersion

*/

int QGraphicsApiFilter::minorVersion() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_minor;
}

/*!
  \property Qt3DRender::QGraphicsApiFilter::majorVersion

 */

/*!
  \qmlproperty int Qt3D.Render::GraphicsApiFilter::majorVersion

*/

int QGraphicsApiFilter::majorVersion() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_major;
}

/*!
  \property Qt3DRender::QGraphicsApiFilter::extensions

 */

/*!
  \qmlproperty stringlist Qt3D.Render::GraphicsApiFilter::extensions

*/

QStringList QGraphicsApiFilter::extensions() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_extensions;
}

/*!
  \property Qt3DRender::QGraphicsApiFilter::vendor

 */

/*!
  \qmlproperty string Qt3D.Render::GraphicsApiFilter::vendor

*/

QString QGraphicsApiFilter::vendor() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_vendor;
}

void QGraphicsApiFilter::setApi(QGraphicsApiFilter::Api api)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_api != api) {
        d->m_api = api;
        emit apiChanged(api);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setProfile(QGraphicsApiFilter::Profile profile)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_profile != profile) {
        d->m_profile = profile;
        emit profileChanged(profile);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setMinorVersion(int minorVersion)
{
    Q_D(QGraphicsApiFilter);
    if (minorVersion != d->m_minor) {
        d->m_minor = minorVersion;
        emit minorVersionChanged(minorVersion);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setMajorVersion(int majorVersion)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_major != majorVersion) {
        d->m_major = majorVersion;
        emit majorVersionChanged(majorVersion);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setExtensions(const QStringList &extensions)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_extensions != extensions) {
        d->m_extensions = extensions;
        emit extensionsChanged(extensions);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setVendor(const QString &vendor)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_vendor != vendor) {
        d->m_vendor = vendor;
        emit vendorChanged(vendor);
        emit graphicsApiFilterChanged();
    }
}

/*! \fn bool Qt3DRender::operator ==(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample)
  \relates Qt3DRender::QGraphicsApiFilter

  Returns \c true if \a reference and \a sample are equivalent.
 */
bool operator ==(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample)
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

/*! \fn bool Qt3DRender::operator !=(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample)
  \relates Qt3DRender::QGraphicsApiFilter

  Returns \c true if \a reference and \a sample are different.
 */
bool operator !=(const QGraphicsApiFilter &reference, const QGraphicsApiFilter &sample)
{
    return !(reference == sample);
}

/*! \fn void Qt3DRender::QGraphicsApiFilter::graphicsApiFilterChanged()
  This signal is emitted when the value of any property is changed.
*/
} // namespace Qt3DRender

QT_END_NAMESPACE
