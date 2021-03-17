/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaspectengine.h"
#include "qaspectengine_p.h"

#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qnode.h>
#include <QtCore/QMetaObject>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qaspectmanager_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qeventfilterservice_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qpostman_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p.h>
#include <Qt3DCore/qt3dcore-config.h>

QT_BEGIN_NAMESPACE

namespace{

QVector<Qt3DCore::QNode *> getNodesForCreation(Qt3DCore::QNode *root)
{
    using namespace Qt3DCore;

    QVector<QNode *> nodes;
    QNodeVisitor visitor;
    visitor.traverse(root, [&nodes](QNode *node) {
        nodes.append(node);

        // Store the metaobject of the node in the QNode so that we have it available
        // to us during destruction in the QNode destructor. This allows us to send
        // the QNodeId and the metaobject as typeinfo to the backend aspects so they
        // in turn can find the correct QBackendNodeMapper object to handle the destruction
        // of the corresponding backend nodes.
        QNodePrivate *d = QNodePrivate::get(node);
        d->m_typeInfo = const_cast<QMetaObject*>(QNodePrivate::findStaticMetaObject(node->metaObject()));

        // Mark this node as having been handled for creation so that it is picked up
        d->m_hasBackendNode = true;
    });

    return nodes;
}

QVector<Qt3DCore::QNode *> getNodesForRemoval(Qt3DCore::QNode *root)
{
    using namespace Qt3DCore;

    QVector<QNode *> nodes;
    QNodeVisitor visitor;
    visitor.traverse(root, [&nodes](QNode *node) {
        nodes.append(node);

        // Mark this node as having been handled for destruction so we don't
        // repeat it unnecessarily in an O(n^2) manner
        QNodePrivate::get(node)->m_hasBackendNode = false;
    });

    return nodes;
}

}

namespace Qt3DCore {

QAspectEnginePrivate *QAspectEnginePrivate::get(QAspectEngine *q)
{
    return q->d_func();
}

QAspectEnginePrivate::QAspectEnginePrivate()
    : QObjectPrivate()
    , m_aspectManager(nullptr)
    , m_postman(nullptr)
    , m_scene(nullptr)
    , m_initialized(false)
    , m_runMode(QAspectEngine::Automatic)
{
    qRegisterMetaType<Qt3DCore::QAbstractAspect *>();
    qRegisterMetaType<Qt3DCore::QObserverInterface *>();
    qRegisterMetaType<Qt3DCore::QNode *>();
    qRegisterMetaType<Qt3DCore::QEntity *>();
    qRegisterMetaType<Qt3DCore::QScene *>();
    qRegisterMetaType<Qt3DCore::QAbstractPostman *>();
}

QAspectEnginePrivate::~QAspectEnginePrivate()
{
    qDeleteAll(m_aspects);
}

/*!
 * \internal
 *
 * Used to init the scene tree when the Qt3D aspect is first started. Basically,
 * sets the scene/change arbiter on the items and stores the entity - component
 * pairs in the scene
 */
void QAspectEnginePrivate::initNode(QNode *node)
{
    m_scene->addObservable(node);
    QNodePrivate::get(node)->setScene(m_scene);
}

void QAspectEnginePrivate::initEntity(QEntity *entity)
{
    const auto components = entity->components();
    for (QComponent *comp : components) {
        if (!m_scene->hasEntityForComponent(comp->id(), entity->id())) {
            if (!comp->isShareable() && !m_scene->entitiesForComponent(comp->id()).isEmpty())
                qWarning() << "Trying to assign a non shareable component to more than one Entity";
            m_scene->addEntityForComponent(comp->id(), entity->id());
        }
    }
}

void QAspectEnginePrivate::addNode(QNode *node)
{
    m_aspectManager->addNodes(getNodesForCreation(node));
}

void QAspectEnginePrivate::removeNode(QNode *node)
{
    m_aspectManager->removeNodes(getNodesForRemoval(node));
}

/*!
 * \class Qt3DCore::QAspectEngine
 * \inheaderfile Qt3DCore/QAspectEngine
 * \inherits QObject
 * \inmodule Qt3DCore
 *
 * \brief Responsible for handling all the QAbstractAspect subclasses that have
 * been registered with the scene.
 *
 * The Qt3D run loop is controlled by the Qt3DRender::QAspectEngine.
 *
 * Qt3DCore::QAbstractAspect subclasses can be registered by calling
 * Qt3DCore::QAspectEngine::registerAspect() which will take care of registering
 * the aspect and in turn that will call Qt3DCore::QAbstractAspect::onRegistered();
 *
 * The simulation loop is launched as soon as a root Qt3DCore::QEntity
 * is set on the Qt3DCore::QAspectEngine. This is followed by a call to
 * onEngineStartup() on each aspect so that they can start their simulation
 * work.
 *
 * The simulation loop is stopped when the root entity is set to
 * Qt3DCore::QEntityPtr(). This calls onEngineShutdown() on each aspect so
 * that they can stop performing their simulation work.
 *
 * Setting a new valid root entity would restart the simulation loop again.
 */

/*!
 * \internal
 * This loop is executed in a separate thread called the AspectThread in
 * Qt3DCore::QAspectThread. This thread is started when the
 * Qt3DCore::QAspectEngine is created and provides the
 * Qt3DCore::QAspectManager which lives in this thread for as long as it's
 * running.
 *
 * Once the AspectThread is running, it starts the run loop and waits for
 * aspects to be registered.
 *
 * Destroying the Qt3DCore::QAspectEngine instance stops the AspectThread and
 * properly terminates the run loop.
 */

/*!
 * \typedef Qt3DCore::QEntityPtr
 * \relates Qt3DCore::QAspectEngine
 *
 * A shared pointer for QEntity.
 */

/*!
 * Constructs a new QAspectEngine with \a parent.
 */
QAspectEngine::QAspectEngine(QObject *parent)
    : QObject(*new QAspectEnginePrivate, parent)
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    Q_D(QAspectEngine);
    d->m_scene = new QScene(this);
    d->m_postman = new QPostman(this);
    d->m_postman->setScene(d->m_scene);
    d->m_aspectManager = new QAspectManager(this);
}

/*!
 * Destroys the engine.
 */
QAspectEngine::~QAspectEngine()
{
    Q_D(QAspectEngine);

    // Shutdown the simulation loop by setting an empty scene
    // Note: this sets an atomic which allows the AspectThread to break out of
    // the inner simulation loop in the AspectThread::exec function
    setRootEntity(QEntityPtr());

    // Unregister all aspects and exit the main loop
    const auto aspects = d->m_aspects;
    for (auto aspect : aspects)
        unregisterAspect(aspect);

    delete d->m_postman;
    delete d->m_scene;
}

void QAspectEnginePrivate::initNodeTree(QNode *node)
{
    // Set the root entity on the scene
    m_scene->setRootNode(node);
    QNodeVisitor visitor;
    visitor.traverse(node, this, &QAspectEnginePrivate::initNode, &QAspectEnginePrivate::initEntity);
}

void QAspectEnginePrivate::initialize()
{
    m_aspectManager->initialize();
    QChangeArbiter *arbiter = m_aspectManager->changeArbiter();
    m_scene->setArbiter(arbiter);
    QChangeArbiter::createUnmanagedThreadLocalChangeQueue(arbiter);
    arbiter->setPostman(m_postman);
    arbiter->setScene(m_scene);
    m_initialized = true;
    m_aspectManager->setPostConstructorInit(m_scene->postConstructorInit());
}

/*!
 * \internal
 *
 * Called when we unset the root entity. Causes the QAspectManager's simulation
 * loop to be exited. The main loop should keep processing events ready
 * to start up the simulation again with a new root entity.
 */
void QAspectEnginePrivate::shutdown()
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    // Flush any change batch waiting in the postman that may contain node
    // destruction changes that the aspects should process before we exit
    // the simulation loop
    m_postman->submitChangeBatch();

    // Exit the simulation loop. Waits for this to be completed on the aspect
    // thread before returning
    exitSimulationLoop();

    // Cleanup the scene before quitting the backend
    m_scene->setArbiter(nullptr);
    QChangeArbiter *arbiter = m_aspectManager->changeArbiter();
    QChangeArbiter::destroyUnmanagedThreadLocalChangeQueue(arbiter);
    m_initialized = false;
}

void QAspectEnginePrivate::exitSimulationLoop()
{
    if (m_aspectManager != nullptr)
        m_aspectManager->exitSimulationLoop();
}

/*!
 Registers a new \a aspect to the AspectManager. The QAspectEngine takes
 ownership of the aspect and will delete it when the aspect is unregistered.
 //! Called in the main thread
 */
void QAspectEngine::registerAspect(QAbstractAspect *aspect)
{
    Q_D(QAspectEngine);
    // The aspect is moved to the AspectThread
    // AspectManager::registerAspect is called in the context
    // of the AspectThread. This is turns call aspect->onInitialize
    // still in the same AspectThread context
    d->m_aspects << aspect;
    d->m_aspectManager->registerAspect(aspect);
}

/*!
 * Registers a new aspect to the AspectManager based on its \a name
 * Uses the currently set aspect factory to create the actual aspect
 * instance.
 */
void QAspectEngine::registerAspect(const QString &name)
{
    Q_D(QAspectEngine);
    QAbstractAspect *aspect = d->m_factory.createAspect(QLatin1String(name.toUtf8()));
    if (aspect) {
        registerAspect(aspect);
        d->m_namedAspects.insert(name, aspect);
    }
}

/*!
 * Unregisters and deletes the given \a aspect.
 */
void QAspectEngine::unregisterAspect(QAbstractAspect *aspect)
{
    Q_D(QAspectEngine);
    if (!d->m_aspects.contains(aspect)) {
        qWarning() << "Attempting to unregister an aspect that is not registered";
        return;
    }

    // Cleanly shutdown this aspect by setting its root entity to null which
    // will cause its onEngineShutdown() virtual to be called to allow it to
    // cleanup any resources. Then remove it from the QAspectManager's list
    // of aspects.
    // TODO: Implement this once we are able to cleanly shutdown

    // Tell the aspect manager to give the aspect a chance to do some cleanup
    // in its QAbstractAspect::onUnregistered() virtual
    d->m_aspectManager->unregisterAspect(aspect);

    // Remove from our collection of named aspects (if present)
    const auto it = std::find_if(d->m_namedAspects.begin(), d->m_namedAspects.end(),
                                 [aspect](QAbstractAspect *v) { return v == aspect; });
    if (it != d->m_namedAspects.end())
        d->m_namedAspects.erase(it);

    // Finally, scheduly the aspect for deletion. Do this via the event loop
    // in case we are unregistering the aspect in response to a signal from it.
    aspect->deleteLater();
    d->m_aspects.removeOne(aspect);
}

/*!
 * Unregisters and deletes the aspect with the given \a name.
 */
void QAspectEngine::unregisterAspect(const QString &name)
{
    Q_D(QAspectEngine);
    if (!d->m_namedAspects.contains(name)) {
        qWarning() << "Attempting to unregister an aspect that is not registered";
        return;
    }

    // Delegate unregistering and removal to the overload
    QAbstractAspect *aspect = d->m_namedAspects.value(name);
    unregisterAspect(aspect);
}

/*!
 * \return the aspects owned by the aspect engine.
 */
QVector<QAbstractAspect *> QAspectEngine::aspects() const
{
    Q_D(const QAspectEngine);
    return d->m_aspects;
}

/*!
 * Executes the given \a command on aspect engine. Valid commands are:
 * \list
 * \li "list aspects"
 * \endlist
 *
 * \return the reply for the command.
 */
QVariant QAspectEngine::executeCommand(const QString &command)
{
    Q_D(QAspectEngine);

    if (command == QLatin1String("list aspects")) {
        if (d->m_aspects.isEmpty())
            return QLatin1String("No loaded aspect");

        const QStringList names = d->m_aspectManager->serviceLocator()->systemInformation()->aspectNames();
        return names.join(QLatin1String("\n"));
    }
    if (command == QLatin1String("dump jobs")) {
        d->m_aspectManager->dumpJobsOnNextFrame();
        return QLatin1String("Dump in next frame in working directory");
    }

    QStringList args = command.split(QLatin1Char(' '));
    QString aspectName = args.takeFirst();

    for (QAbstractAspect *aspect : qAsConst(d->m_aspects)) {
        if (aspectName == d->m_factory.aspectName(aspect))
            return aspect->executeCommand(args);
    }

    return QVariant();
}

/*!
 * If using the manual run mode, this function executes the jobs for each aspect.
 * It is blocking and won't return until all jobs have been completed.
 *
 * If you are using the QRenderAspect,
 */
void QAspectEngine::processFrame()
{
    Q_D(QAspectEngine);
    Q_ASSERT(d->m_runMode == QAspectEngine::Manual);
    d->m_aspectManager->processFrame();
}

/*!
 * Sets the \a root entity for the aspect engine.
 */
void QAspectEngine::setRootEntity(QEntityPtr root)
{
    qCDebug(Aspects) << Q_FUNC_INFO << "root =" << root;
    Q_D(QAspectEngine);
    if (d->m_root == root)
        return;

    const bool shutdownNeeded = d->m_root && d->m_initialized;

    // Set the new root object. This will cause the old tree to be deleted
    // and the deletion of the old frontend tree will cause the backends to
    // free any related resources
    d->m_root = root;

    if (shutdownNeeded)
        d->shutdown();

    // Do we actually have a new scene?
    if (!d->m_root)
        return;

    // Set postman/scene/arbiter ...
    d->initialize();

    // The aspect engine takes ownership of the scene root. We also set the
    // parent of the scene root to be the engine
    static_cast<QObject *>(d->m_root.data())->setParent(this);

    // Prepare the frontend tree for use by giving each node a pointer to the
    // scene object and adding each node to the scene
    // TODO: We probably need a call symmetric to this one above in order to
    // deregister the nodes from the scene
    d->initNodeTree(root.data());

    const QVector<QNode *> nodes = getNodesForCreation(root.data());

    // Specify if the AspectManager should be driving the simulation loop or not
    d->m_aspectManager->setRunMode(d->m_runMode);

    // Finally, tell the aspects about the new scene object tree. This is done
    // in a blocking manner to allow the aspects to get synchronized before the
    // main thread starts triggering potentially more notifications

    // TODO: Pass the creation changes via the arbiter rather than relying upon
    // an invokeMethod call.
    qCDebug(Aspects) << "Begin setting scene root on aspect manager";
    d->m_aspectManager->setRootEntity(root.data(), nodes);
    qCDebug(Aspects) << "Done setting scene root on aspect manager";
    d->m_aspectManager->enterSimulationLoop();
}

/*!
 * \return the root entity of the aspect engine.
 */
QEntityPtr QAspectEngine::rootEntity() const
{
    Q_D(const QAspectEngine);
    return d->m_root;
}

void QAspectEngine::setRunMode(QAspectEngine::RunMode mode)
{
    Q_D(QAspectEngine);
    d->m_runMode = mode;
    if (d->m_aspectManager)
        d->m_aspectManager->setRunMode(mode);
}

QAspectEngine::RunMode QAspectEngine::runMode() const
{
    Q_D(const QAspectEngine);
    return d->m_runMode;
}

} // namespace Qt3DCore

QT_END_NAMESPACE
