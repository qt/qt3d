// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qgraphicsapifilter.h"
#include "qgraphicsapifilter_p.h"
#include "private/qobject_p.h"
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

GraphicsApiFilterData::GraphicsApiFilterData()
    : m_api(QOpenGLContext::openGLModuleType() == QOpenGLContext::LibGL ? QGraphicsApiFilter::OpenGL : QGraphicsApiFilter::OpenGLES)
    , m_profile(QGraphicsApiFilter::NoProfile) // matches all (no profile, core, compat)
    , m_minor(0)
    , m_major(0)
{}

QString GraphicsApiFilterData::toString() const
{
    QLatin1String api;
    switch (m_api) {
    case QGraphicsApiFilter::OpenGL: api = QLatin1String("OpenGL"); break;
    case QGraphicsApiFilter::OpenGLES: api = QLatin1String("OpenGL"); break;
    case QGraphicsApiFilter::Vulkan: api = QLatin1String("Vulkan"); break;
    case QGraphicsApiFilter::DirectX: api = QLatin1String("DirectX"); break;
    case QGraphicsApiFilter::RHI: api = QLatin1String("RHI"); break;
    default: Q_UNREACHABLE();
    }

    QLatin1String profile;
    switch (m_profile) {
    case QGraphicsApiFilter::CoreProfile: profile = QLatin1String(" (Core Profile)"); break;
    case QGraphicsApiFilter::CompatibilityProfile: profile = QLatin1String(" (Compatibility Profile)"); break;
    default: break;
    }

    return QString(QLatin1String("%1 %2.%3%4 (%5)").arg(api, QString::number(m_major), QString::number(m_minor), profile, m_vendor));
}

bool GraphicsApiFilterData::operator ==(const GraphicsApiFilterData &other) const
{
    // Check API
    if (other.m_api != m_api)
        return false;

    // Check versions
    const bool versionsCompatible = other.m_major < m_major
            || (other.m_major == m_major && other.m_minor <= m_minor);
    if (!versionsCompatible)
        return false;

    // Check profiles if requiring OpenGL (profiles not relevant for OpenGL ES)
    if (other.m_api == QGraphicsApiFilter::OpenGL) {
        const bool profilesCompatible = m_profile != QGraphicsApiFilter::CoreProfile
                || other.m_profile == m_profile;
        if (!profilesCompatible)
            return false;
    }

    // Check extensions
    for (const QString &neededExt : other.m_extensions) {
        if (!m_extensions.contains(neededExt))
            return false;
    }

    // Check vendor
    if (!other.m_vendor.isEmpty())
        return (other.m_vendor == m_vendor);

    // If we get here everything looks good :)
    return true;
}

bool GraphicsApiFilterData::operator <(const GraphicsApiFilterData &other) const
{
    if (this->m_major > other.m_major)
        return false;
    if (this->m_major == other.m_major &&
        this->m_minor > other.m_minor)
        return false;
    return true;
}

bool GraphicsApiFilterData::operator !=(const GraphicsApiFilterData &other) const
{
    return !(*this == other);
}

QGraphicsApiFilterPrivate *QGraphicsApiFilterPrivate::get(QGraphicsApiFilter *q)
{
    return q->d_func();
}

const QGraphicsApiFilterPrivate *QGraphicsApiFilterPrivate::get(const QGraphicsApiFilter *q)
{
    return q->d_func();
}

/*!
    \class Qt3DRender::QGraphicsApiFilter
    \inmodule Qt3DRender
    \since 5.5
    \brief The QGraphicsApiFilter class identifies the API required for the attached QTechnique.
*/

/*!
    \qmltype GraphicsApiFilter
    \nativetype Qt3DRender::QGraphicsApiFilter
    \inherits QtObject
    \inqmlmodule Qt3D.Render
    \since 5.5
    \brief For OpenGL identifies the API required for the attached technique.
*/

/*! \fn Qt3DRender::QGraphicsApiFilter::QGraphicsApiFilter(QObject *parent)
  Constructs a new QGraphicsApiFilter with the specified \a parent.
 */
QGraphicsApiFilter::QGraphicsApiFilter(QObject *parent)
    : QObject(*new QGraphicsApiFilterPrivate, parent)
{
}

/*! \internal */
QGraphicsApiFilter::~QGraphicsApiFilter()
{
}

/*!
  \enum Qt3DRender::QGraphicsApiFilter::Api

  \value OpenGLES QSurfaceFormat::OpenGLES
  \value OpenGL QSurfaceFormat::OpenGL
  \value Vulkan Vulkan
  \value DirectX DirectX
  \value RHI RHI
*/

/*!
  \enum Qt3DRender::QGraphicsApiFilter::OpenGLProfile

  This enum identifies the type of profile required.

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
    return d->m_data.m_api;
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

QGraphicsApiFilter::OpenGLProfile QGraphicsApiFilter::profile() const
{
    Q_D(const QGraphicsApiFilter);
    return d->m_data.m_profile;
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
    return d->m_data.m_minor;
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
    return d->m_data.m_major;
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
    return d->m_data.m_extensions;
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
    return d->m_data.m_vendor;
}

void QGraphicsApiFilter::setApi(QGraphicsApiFilter::Api api)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_data.m_api != api) {
        d->m_data.m_api = api;
        emit apiChanged(api);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setProfile(QGraphicsApiFilter::OpenGLProfile profile)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_data.m_profile != profile) {
        d->m_data.m_profile = profile;
        emit profileChanged(profile);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setMinorVersion(int minorVersion)
{
    Q_D(QGraphicsApiFilter);
    if (minorVersion != d->m_data.m_minor) {
        d->m_data.m_minor = minorVersion;
        emit minorVersionChanged(minorVersion);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setMajorVersion(int majorVersion)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_data.m_major != majorVersion) {
        d->m_data.m_major = majorVersion;
        emit majorVersionChanged(majorVersion);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setExtensions(const QStringList &extensions)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_data.m_extensions != extensions) {
        d->m_data.m_extensions = extensions;
        emit extensionsChanged(extensions);
        emit graphicsApiFilterChanged();
    }
}

void QGraphicsApiFilter::setVendor(const QString &vendor)
{
    Q_D(QGraphicsApiFilter);
    if (d->m_data.m_vendor != vendor) {
        d->m_data.m_vendor = vendor;
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
    return QGraphicsApiFilterPrivate::get(const_cast<QGraphicsApiFilter *>(&reference))->m_data ==
            QGraphicsApiFilterPrivate::get(const_cast<QGraphicsApiFilter *>(&sample))->m_data;
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

#include "moc_qgraphicsapifilter.cpp"
