// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qqmlaspectengine_p.h"

#include <Qt3DCore/qentity.h>
#include <QtCore/QDebug>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

/*!
    \namespace Qt3DCore::Quick
    \inmodule Qt3DCore
    \ingroup qt3d-namespaces

    \brief Contains classes used for implementing QML functionality into Qt3D
    applications.
*/

QQmlAspectEnginePrivate::QQmlAspectEnginePrivate()
    : QObjectPrivate()
    , m_qmlEngine(new QQmlEngine())
    , m_aspectEngine(new QAspectEngine())
    , m_component(nullptr)
{
}

/*!
 * \class Qt3DCore::Quick::QQmlAspectEngine
 * \inheaderfile Qt3DQuick/QQmlAspectEngine
 * \inmodule Qt3DCore
 *
 * \brief The QQmlAspectEngine provides an environment for the QAspectEngine and
 * a method for instantiating QML components.
 */

/*!
 * \enum Qt3DCore::Quick::QQmlAspectEngine::Status
 *
 * The status of the engine.
 *
 * \value Null
 * \value Ready
 * \value Loading
 * \value Error
 */

/*!
 * \fn void Qt3DCore::Quick::QQmlAspectEngine::statusChanged(Status status)
 *
 * This signal is emitted with \a status when the status of the engine changes.
 */

/*!
 * \fn void Qt3DCore::Quick::QQmlAspectEngine::sceneCreated(QObject* rootObject)
 *
 * This signal is emitted with \a rootObject when the scene has been instantiated. This provides a
 * chance to manipulate the scene before passing it over to the aspect engine. Useful for
 * convenience window classes to set up cameras and surfaces on the framegraph and event sources
 * for the input aspect etc.
 */

/*!
 * Constructs a new QQmlAspectEngine with \a parent.
 */
QQmlAspectEngine::QQmlAspectEngine(QObject *parent)
    : QObject(*new QQmlAspectEnginePrivate, parent)
{
}

/*! \internal */
QQmlAspectEngine::~QQmlAspectEngine()
{
}

/*!
 * \return the status.
 */
QQmlAspectEngine::Status QQmlAspectEngine::status() const
{
    Q_D(const QQmlAspectEngine);

    if (!d->m_component)
        return Null;

    return Status(d->m_component->status());
}

/*!
 * Sets \a source as a source for the QML component to be created.
 */
void QQmlAspectEngine::setSource(const QUrl &source)
{
    Q_D(QQmlAspectEngine);

    if (d->m_component) {
        d->m_aspectEngine->setRootEntity(QEntityPtr());
        d->m_component = nullptr;
    }

    if (!source.isEmpty()) {
        d->m_component = new QQmlComponent(d->m_qmlEngine.data(), source);
        if (!d->m_component->isLoading()) {
            d->_q_continueExecute();
        } else {
            QObject::connect(d->m_component, SIGNAL(statusChanged(QQmlComponent::Status)),
                             this, SLOT(_q_continueExecute()));
        }
    }
}

/*!
 * \return the engine.
 */
QQmlEngine *QQmlAspectEngine::qmlEngine() const
{
    Q_D(const QQmlAspectEngine);
    return d->m_qmlEngine.data();
}

/*!
 * \return the aspectEngine.
 */
QAspectEngine *QQmlAspectEngine::aspectEngine() const
{
    Q_D(const QQmlAspectEngine);
    return d->m_aspectEngine.data();
}

void QQmlAspectEnginePrivate::_q_continueExecute()
{
    Q_Q(QQmlAspectEngine);

    QObject::disconnect(m_component, SIGNAL(statusChanged(QQmlComponent::Status)),
                        q, SLOT(_q_continueExecute()));

    if (m_component->isError()) {
        const auto errors = m_component->errors();
        for (const QQmlError &error : errors) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit q->statusChanged(q->status());
        return;
    }

    QObject* obj = m_component->create();

    if (m_component->isError()) {
        const auto errors = m_component->errors();
        for (const QQmlError &error : errors) {
            QMessageLogger(error.url().toString().toLatin1().constData(), error.line(), 0).warning()
                << error;
        }
        emit q->statusChanged(q->status());
        return;
    }

    // Let users know we have loaded the QML file, and the scene has been instantiated.
    // This gives a chance to manipulate the scene before passing it over to the
    // aspect engine. Useful for convenience window classes to set up cameras and surfaces
    // on the framegraph and event sources for the input aspect etc.
    emit q->sceneCreated(obj);
    m_aspectEngine->setRootEntity(QEntityPtr(qobject_cast<QEntity *>(obj)));
    emit q->statusChanged(q->status());
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#include "moc_qqmlaspectengine.cpp"
