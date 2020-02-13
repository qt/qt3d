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

#include "qaspectmanager_p.h"

#include <Qt3DCore/qabstractaspect.h>
#include <Qt3DCore/qentity.h>
#include <QtCore/QAbstractEventDispatcher>
#include <QtCore/QEventLoop>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtGui/QSurface>

#include <Qt3DCore/private/corelogging_p.h>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/private/qabstractaspectjobmanager_p.h>
#include <Qt3DCore/private/qabstractframeadvanceservice_p.h>
// TODO Make the kind of job manager configurable (e.g. ThreadWeaver vs Intel TBB)
#include <Qt3DCore/private/qaspectjobmanager_p.h>
#include <Qt3DCore/private/qaspectjob_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/private/qscheduler_p.h>
#include <Qt3DCore/private/qservicelocator_p.h>
#include <Qt3DCore/private/qsysteminformationservice_p_p.h>
#include <Qt3DCore/private/qthreadpooler_p.h>
#include <Qt3DCore/private/qtickclock_p.h>
#include <Qt3DCore/private/qtickclockservice_p.h>
#include <Qt3DCore/private/qnodevisitor_p.h>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QAbstractAnimation>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

#if QT_CONFIG(animation)
class RequestFrameAnimation final : public QAbstractAnimation
{
public:
    RequestFrameAnimation(QObject *parent)
        : QAbstractAnimation(parent)
    {
    }

    ~RequestFrameAnimation() override;

    int duration() const override { return 1; }
    void updateCurrentTime(int currentTime) override { Q_UNUSED(currentTime) }
};

RequestFrameAnimation::~RequestFrameAnimation() = default;
#else
namespace  {

class RequestFrameEvent : public QEvent
{
public:
    RequestFrameEvent()
        : QEvent(static_cast<QEvent::Type>(RequestFrameEvent::requestEventType))
    {}

    static int eventType() { return RequestFrameEvent::requestEventType; }

private:
    static int requestEventType;
};

int RequestFrameEvent::requestEventType = QEvent::registerEventType();

} // anonymous
#endif

/*!
    \class Qt3DCore::QAspectManager
    \internal
*/
QAspectManager::QAspectManager(QAspectEngine *parent)
    : QObject(parent)
    , m_engine(parent)
    , m_root(nullptr)
    , m_scheduler(new QScheduler(this))
    , m_jobManager(new QAspectJobManager(this))
    , m_changeArbiter(new QChangeArbiter(this))
    , m_serviceLocator(new QServiceLocator(parent))
    , m_simulationLoopRunning(false)
    , m_driveMode(QAspectEngine::Automatic)
    , m_postConstructorInit(nullptr)
#if QT_CONFIG(animation)
    , m_simulationAnimation(nullptr)
#endif
    , m_jobsInLastFrame(0)
    , m_dumpJobs(false)
{
    qRegisterMetaType<QSurface *>("QSurface*");
    qCDebug(Aspects) << Q_FUNC_INFO;
}

QAspectManager::~QAspectManager()
{
    delete m_changeArbiter;
    delete m_jobManager;
    delete m_scheduler;
}

void QAspectManager::setRunMode(QAspectEngine::RunMode mode)
{
    qCDebug(Aspects) << Q_FUNC_INFO << "Running Loop Drive Mode set to" << mode;
    m_driveMode = mode;
}

// Main thread (called by QAspectEngine)
void QAspectManager::enterSimulationLoop()
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_simulationLoopRunning = true;

    // Retrieve the frame advance service. Defaults to timer based if there is no renderer.
    QAbstractFrameAdvanceService *frameAdvanceService =
            m_serviceLocator->service<QAbstractFrameAdvanceService>(QServiceLocator::FrameAdvanceService);

    // Start the frameAdvanceService
    frameAdvanceService->start();

    // We are about to enter the simulation loop. Give aspects a chance to do any last
    // pieces of initialization
    qCDebug(Aspects) << "Calling onEngineStartup() for each aspect";
    for (QAbstractAspect *aspect : qAsConst(m_aspects)) {
        qCDebug(Aspects) << "\t" << aspect->objectName();
        aspect->onEngineStartup();
    }
    qCDebug(Aspects) << "Done calling onEngineStartup() for each aspect";

    // Start running loop if Qt3D is in charge of driving it
    if (m_driveMode == QAspectEngine::Automatic) {
#if QT_CONFIG(animation)
        if (!m_simulationAnimation) {
            m_simulationAnimation = new RequestFrameAnimation(this);
            connect(m_simulationAnimation, &QAbstractAnimation::finished, this, [this]() {
                processFrame();
                if (m_simulationLoopRunning && m_driveMode == QAspectEngine::Automatic)
                    requestNextFrame();
            });
        }
#endif
        requestNextFrame();
    }
}

// Main thread (called by QAspectEngine)
void QAspectManager::exitSimulationLoop()
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    // If this fails, simulation loop is already exited so nothing to do
    if (!m_simulationLoopRunning) {
        qCDebug(Aspects) << "Simulation loop was not running. Nothing to do";
        return;
    }

#if QT_CONFIG(animation)
    if (m_simulationAnimation)
        m_simulationAnimation->stop();
#endif

    QAbstractFrameAdvanceService *frameAdvanceService =
            m_serviceLocator->service<QAbstractFrameAdvanceService>(QServiceLocator::FrameAdvanceService);
    if (frameAdvanceService)
        frameAdvanceService->stop();

    // Give any aspects a chance to unqueue any asynchronous work they
    // may have scheduled that would otherwise potentially deadlock or
    // cause races. For example, the QLogicAspect queues up a vector of
    // QNodeIds to be processed by a callback on the main thread. However,
    // if we don't unqueue this work and release its semaphore, the logic
    // aspect would cause a deadlock when trying to exit the inner loop.
    // This is because we call this function from the main thread and the
    // logic aspect is waiting for the main thread to execute the
    // QLogicComponent::onFrameUpdate() callback.
    for (QAbstractAspect *aspect : qAsConst(m_aspects))
        aspect->d_func()->onEngineAboutToShutdown();

    // Process any pending changes from the frontend before we shut the aspects down
    m_changeArbiter->syncChanges();

    // Give aspects a chance to perform any shutdown actions. This may include unqueuing
    // any blocking work on the main thread that could potentially deadlock during shutdown.
    qCDebug(Aspects) << "Calling onEngineShutdown() for each aspect";
    for (QAbstractAspect *aspect : qAsConst(m_aspects)) {
        qCDebug(Aspects) << "\t" << aspect->objectName();
        aspect->onEngineShutdown();
    }
    qCDebug(Aspects) << "Done calling onEngineShutdown() for each aspect";

    m_simulationLoopRunning = false;
    qCDebug(Aspects) << "exitSimulationLoop completed";
}

bool QAspectManager::isShuttingDown() const
{
    return !m_simulationLoopRunning;
}

/*!
    \internal

    Called by the QAspectThread's run() method immediately after the manager
    has been created
*/
void QAspectManager::initialize()
{
    qCDebug(Aspects) << Q_FUNC_INFO;
    m_jobManager->initialize();
    m_scheduler->setAspectManager(this);
    m_changeArbiter->initialize(m_jobManager);
}

/*!
    \internal

    Called by the QAspectThread's run() method immediately after the manager's
    exec() function has returned.
*/
void QAspectManager::shutdown()
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    // Aspects must be deleted in the Thread they were created in
}

// MainThread called by QAspectEngine::setRootEntity
void QAspectManager::setRootEntity(Qt3DCore::QEntity *root, const QVector<QNode *> &nodes)
{
    qCDebug(Aspects) << Q_FUNC_INFO;

    if (root == m_root)
        return;

    if (m_root) {
        // TODO: Delete all backend nodes. This is to be symmetric with how
        // we create them below in the call to setRootAndCreateNodes
    }

    m_root = root;

    if (m_root) {

        QVector<NodeTreeChange> nodeTreeChanges;
        nodeTreeChanges.reserve(nodes.size());

        for (QNode *n : nodes) {
            nodeTreeChanges.push_back({
                                          n->id(),
                                          QNodePrivate::get(n)->m_typeInfo,
                                          NodeTreeChange::Added,
                                          n
                                      });
        }

        for (QAbstractAspect *aspect : qAsConst(m_aspects))
            aspect->d_func()->setRootAndCreateNodes(m_root, nodeTreeChanges);
    }
}


// Main Thread -> immediately following node insertion
void QAspectManager::addNodes(const QVector<QNode *> &nodes)
{
    // We record the nodes added information, which we will actually use when
    // processFrame is called (later but within the same loop of the even loop
    // as this call) The idea is we want to avoid modifying the backend tree if
    // the Renderer hasn't allowed processFrame to continue yet

    QVector<NodeTreeChange> treeChanges;
    treeChanges.reserve(nodes.size());

    for (QNode *node : nodes) {
        treeChanges.push_back({ node->id(),
                                QNodePrivate::get(node)->m_typeInfo,
                                NodeTreeChange::Added,
                                node });
    }

    m_nodeTreeChanges += treeChanges;
}

// Main Thread -> immediately following node destruction (call from QNode dtor)
void QAspectManager::removeNodes(const QVector<QNode *> &nodes)
{
    // We record the nodes removed information, which we will actually use when
    // processFrame is called (later but within the same loop of the even loop
    // as this call) The idea is we want to avoid modifying the backend tree if
    // the Renderer hasn't allowed processFrame to continue yet The drawback is
    // that when processFrame is processed, the QNode* pointer might be invalid by
    // that point. Therefore we record all we need to remove the object.

    for (QNode *node : nodes) {
        // In addition, we check if we contain an Added change for a given node
        // that is now about to be destroyed. If so we remove the Added change
        // entirely

        m_nodeTreeChanges.erase(std::remove_if(m_nodeTreeChanges.begin(),
                                               m_nodeTreeChanges.end(),
                                               [&node] (const NodeTreeChange &change) { return change.id == node->id(); }),
                                m_nodeTreeChanges.end());

        m_nodeTreeChanges.push_back({ node->id(),
                                      QNodePrivate::get(node)->m_typeInfo,
                                      NodeTreeChange::Removed,
                                      nullptr });
    }
}

/*!
 * \internal
 *
 * Registers a new \a aspect.
 */
void QAspectManager::registerAspect(QAbstractAspect *aspect)
{
    qCDebug(Aspects) << "Registering aspect";

    if (aspect != nullptr) {
        m_aspects.append(aspect);
        QAbstractAspectPrivate::get(aspect)->m_aspectManager = this;
        QAbstractAspectPrivate::get(aspect)->m_jobManager = m_jobManager;
        QAbstractAspectPrivate::get(aspect)->m_arbiter = m_changeArbiter;

        // Allow the aspect to do some work now that it is registered
        aspect->onRegistered();
    }
    else {
        qCWarning(Aspects) << "Failed to register aspect";
    }
    qCDebug(Aspects) << "Completed registering aspect";
}

/*!
 * \internal
 *
 * Calls QAbstractAspect::onUnregistered(), unregisters the aspect from the
 * change arbiter and unsets the arbiter, job manager and aspect manager.
 * Operations are performed in the reverse order to registerAspect.
 */
void QAspectManager::unregisterAspect(Qt3DCore::QAbstractAspect *aspect)
{
    qCDebug(Aspects) << "Unregistering aspect";
    Q_ASSERT(aspect);
    aspect->onUnregistered();
    QAbstractAspectPrivate::get(aspect)->m_arbiter = nullptr;
    QAbstractAspectPrivate::get(aspect)->m_jobManager = nullptr;
    QAbstractAspectPrivate::get(aspect)->m_aspectManager = nullptr;
    m_aspects.removeOne(aspect);
    qCDebug(Aspects) << "Completed unregistering aspect";
}

const QVector<QAbstractAspect *> &QAspectManager::aspects() const
{
    return m_aspects;
}

QAbstractAspectJobManager *QAspectManager::jobManager() const
{
    return m_jobManager;
}

QChangeArbiter *QAspectManager::changeArbiter() const
{
    return m_changeArbiter;
}

QServiceLocator *QAspectManager::serviceLocator() const
{
    return m_serviceLocator.data();
}

void QAspectManager::setPostConstructorInit(NodePostConstructorInit *postConstructorInit)
{
    m_postConstructorInit = postConstructorInit;
}

QNode *QAspectManager::lookupNode(QNodeId id) const
{
    if (!m_root)
        return nullptr;

    QNodePrivate *d = QNodePrivate::get(m_root);
    return d->m_scene ? d->m_scene->lookupNode(id) : nullptr;
}

QVector<QNode *> QAspectManager::lookupNodes(const QVector<QNodeId> &ids) const
{
    if (!m_root)
        return {};

    QNodePrivate *d = QNodePrivate::get(m_root);
    return d->m_scene ? d->m_scene->lookupNodes(ids) : QVector<QNode *>{};
}

void QAspectManager::dumpJobsOnNextFrame()
{
    m_dumpJobs = true;
}

#if !QT_CONFIG(animation)
/*!
    \internal
    \brief Drives the Qt3D simulation loop in the main thread
 */
bool QAspectManager::event(QEvent *e)
{
    if (e->type() == RequestFrameEvent::eventType()) {

        // Process current frame
        processFrame();

        // Request next frame if we are still running and if Qt3D is driving
        // the loop
        if (m_simulationLoopRunning && m_driveMode == QAspectEngine::Automatic)
            requestNextFrame();

        return true;
    }

    return QObject::event(e);
}
#endif

void QAspectManager::requestNextFrame()
{
    qCDebug(Aspects) << "Requesting new Frame";
    // Post event in the event loop to force
    // next frame to be processed
#if QT_CONFIG(animation)
    m_simulationAnimation->start();
#else
    QCoreApplication::postEvent(this, new RequestFrameEvent());
#endif
}

void QAspectManager::processFrame()
{
    qCDebug(Aspects) << "Processing Frame";

    // Retrieve the frame advance service. Defaults to timer based if there is no renderer.
    QAbstractFrameAdvanceService *frameAdvanceService =
            m_serviceLocator->service<QAbstractFrameAdvanceService>(QServiceLocator::FrameAdvanceService);

    const qint64 t = frameAdvanceService->waitForNextFrame();
    if (t < 0)
        return;

    // Distribute accumulated changes. This includes changes sent from the frontend
    // to the backend nodes. We call this before the call to m_scheduler->update() to ensure
    // that any property changes do not set dirty flags in a data race with the renderer's
    // submission thread which may be looking for dirty flags, acting upon them and then
    // clearing the dirty flags.
    //
    // Doing this as the first call in the new frame ensures the lock free approach works
    // without any such data race.
    {
        // scope for QTaskLogger
        QTaskLogger logger(m_serviceLocator->systemInformation(), 4096, 0, QTaskLogger::AspectJob);

        // Tell the NodePostConstructorInit to process any pending nodes which will add them to our list of
        // tree changes
        m_postConstructorInit->processNodes();

        // Add and Remove Nodes
        const QVector<NodeTreeChange> nodeTreeChanges = std::move(m_nodeTreeChanges);
        for (const NodeTreeChange &change : nodeTreeChanges) {
            // Buckets ensure that even if we have intermingled node added / removed
            // buckets, we preserve the order of the sequences

            for (QAbstractAspect *aspect : qAsConst(m_aspects)) {
                switch (change.type) {
                case NodeTreeChange::Added:
                    aspect->d_func()->createBackendNode(change);
                    break;
                case NodeTreeChange::Removed:
                    aspect->d_func()->clearBackendNode(change);
                    break;
                }
            }
        }

        // Sync node / subnode relationship changes
        const auto dirtySubNodes = m_changeArbiter->takeDirtyFrontEndSubNodes();
        if (dirtySubNodes.size())
            for (QAbstractAspect *aspect : qAsConst(m_aspects))
                QAbstractAspectPrivate::get(aspect)->syncDirtyFrontEndSubNodes(dirtySubNodes);

        // Sync property updates
        const auto dirtyFrontEndNodes = m_changeArbiter->takeDirtyFrontEndNodes();
        if (dirtyFrontEndNodes.size())
            for (QAbstractAspect *aspect : qAsConst(m_aspects))
                QAbstractAspectPrivate::get(aspect)->syncDirtyFrontEndNodes(dirtyFrontEndNodes);

        // TO DO: Having this done in the main thread actually means aspects could just
        // as simply read info out of the Frontend classes without risk of introducing
        // races. This could therefore be removed for Qt 6.
        m_changeArbiter->syncChanges();
    }

    // For each Aspect
    // Ask them to launch set of jobs for the current frame
    // Updates matrices, bounding volumes, render bins ...
    m_jobsInLastFrame = m_scheduler->scheduleAndWaitForFrameAspectJobs(t, m_dumpJobs);
    m_dumpJobs = false;

    // Tell the aspect the frame is complete (except rendering)
    for (QAbstractAspect *aspect : qAsConst(m_aspects))
        QAbstractAspectPrivate::get(aspect)->frameDone();
}

} // namespace Qt3DCore

QT_END_NAMESPACE
