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

#include <QtTest/QtTest>
#include <Qt3DCore/private/qobserverinterface_p.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include <Qt3DCore/qscenepropertychange.h>
#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DCore/qscene.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qsceneobserverinterface.h>
#include <Qt3DCore/private/qnode_p.h>

class tst_QChangeArbiter : public QObject
{
    Q_OBJECT
public:

#if 0
private slots:
    void registerObservers();
    void registerSceneObserver();
    void unregisterObservers();
    void unregisterSceneObservers();
    void distributeFrontendChanges();
    void distributeBackendChanges();
#endif
};

#if 0
class tst_Node : public Qt3D::QNode
{
public:
    explicit tst_Node(Qt3D::QNode *parent = 0) : Qt3D::QNode(parent)
    {}

    void sendNodeAddedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::NodeAdded, this));
        e->setPropertyName(QByteArrayLiteral("NodeAdded"));
//        d_func()->notifyObservers(e);
    }

    void sendNodeRemovedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::NodeRemoved, this));
        e->setPropertyName(QByteArrayLiteral("NodeRemoved"));
//        d->notifyObservers(e);
    }

    void sendNodeUpdatedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::NodeUpdated, this));
        e->setPropertyName(QByteArrayLiteral("NodeUpdated"));
//        d->notifyObservers(e);
    }

    void sendComponentAddedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::ComponentAdded, this));
        e->setPropertyName(QByteArrayLiteral("ComponentAdded"));
//        d->notifyObservers(e);
    }

    void sendComponentRemovedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::ComponentRemoved, this));
        e->setPropertyName(QByteArrayLiteral("ComponentRemoved"));
//        d->notifyObservers(e);
    }

    void sendComponentUpdatedNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::ComponentUpdated, this));
        e->setPropertyName(QByteArrayLiteral("ComponentUpdated"));
//        d->notifyObservers(e);
    }

    void sendAllChangesNotification()
    {
        Qt3D::QScenePropertyChangePtr e(new Qt3D::QScenePropertyChange(Qt3D::AllChanges, this));
        e->setPropertyName(QByteArrayLiteral("AllChanges"));
//        d->notifyObservers(e);
    }

    void sceneChangeEvent(const Qt3D::QSceneChangePtr &change) Q_DECL_OVERRIDE
    {
        QVERIFY(!change.isNull());
        m_lastChanges << change;
    }

    Qt3D::QSceneChangePtr lastChange() const
    {
        if (m_lastChanges.isEmpty())
            return Qt3D::QSceneChangePtr();
        return m_lastChanges.last();
    }

    QList<Qt3D::QSceneChangePtr> lastChanges() const
    {
        return m_lastChanges;
    }

    // QNode interface
protected:
    Qt3D::QNode *doClone() const
    {
        tst_Node *clone = new tst_Node();
        clone->copy(this);
        return clone;
    }

private:
    QList<Qt3D::QSceneChangePtr> m_lastChanges;
};

class tst_SimpleObserver : public Qt3D::QObserverInterface
{
public:

    // QObserverInterface interface
    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        QVERIFY(!e.isNull());
        m_lastChanges.append(e);
    }

    Qt3D::QSceneChangePtr lastChange() const
    {
        if (m_lastChanges.isEmpty())
            return Qt3D::QSceneChangePtr();
        return m_lastChanges.last();
    }

    QList<Qt3D::QSceneChangePtr> lastChanges() const
    {
        return m_lastChanges;
    }

private:
    QList<Qt3D::QSceneChangePtr> m_lastChanges;
};

class tst_ObserverObservable : public Qt3D::QObservableInterface, public Qt3D::QObserverInterface
{
public:

    tst_ObserverObservable() : m_arbiter(Q_NULLPTR)
    {}

    // QObserverInterface interface
    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        QVERIFY(!e.isNull());
        m_lastChanges << e;
        // Sends reply to frontend
        Qt3D::QBackendScenePropertyChangePtr change(new Qt3D::QBackendScenePropertyChange(Qt3D::NodeUpdated, this));
        change->setTargetNode(e->subject().m_node->id());
        change->setPropertyName(QByteArrayLiteral("Reply"));
        notifyObservers(change);
    }

    // QObservableInterface interface
    void registerObserver(Qt3D::QObserverInterface *observer) Q_DECL_OVERRIDE
    {
        m_arbiter = dynamic_cast<Qt3D::QChangeArbiter *>(observer);
        QVERIFY(m_arbiter != Q_NULLPTR);
    }

    void unregisterObserver(Qt3D::QObserverInterface *observer) Q_DECL_OVERRIDE
    {
        QVERIFY(m_arbiter == observer);
    }

    Qt3D::QSceneChangePtr lastChange() const
    {
        if (m_lastChanges.isEmpty())
            return Qt3D::QSceneChangePtr();
        return m_lastChanges.last();
    }

    QList<Qt3D::QSceneChangePtr> lastChanges() const
    {
        return m_lastChanges;
    }

protected:
    void notifyObservers(const Qt3D::QSceneChangePtr &e) Q_DECL_OVERRIDE
    {
        QVERIFY(m_arbiter != Q_NULLPTR);
        m_arbiter->sceneChangeEventWithLock(e);
    }

private:
    Qt3D::QChangeArbiter *m_arbiter;
    QList<Qt3D::QSceneChangePtr> m_lastChanges;

};

class tst_PostManObserver : public Qt3D::QObserverInterface
{
public:

    tst_PostManObserver() : m_sceneInterface(Q_NULLPTR)
    {}

    void setScene(Qt3D::QSceneInterface *scene)
    {
        m_sceneInterface = scene;
    }

    // QObserverInterface interface
    void sceneChangeEvent(const Qt3D::QSceneChangePtr &e)
    {
        QVERIFY(!e.isNull());
        Qt3D::QBackendScenePropertyChangePtr change = qSharedPointerDynamicCast<Qt3D::QBackendScenePropertyChange>(e);
        QVERIFY(!change.isNull());
        Qt3D::QNode *targetNode = m_sceneInterface->lookupNode(change->targetNode());
//        if (targetNode != Q_NULLPTR)
//            targetNode->sceneChangeEvent(e);
    }

private:
    Qt3D::QSceneInterface *m_sceneInterface;
};

class tst_SceneObserver : public Qt3D::QSceneObserverInterface
{
    // QSceneObserverInterface interface
public:
    void sceneNodeAdded(Qt3D::QSceneChangePtr &e)
    {
        QVERIFY(!e.isNull());
        QVERIFY(e->type() == Qt3D::NodeCreated);
        m_lastChange = e;
    }

    void sceneNodeRemoved(Qt3D::QSceneChangePtr &e)
    {
        QVERIFY(!e.isNull());
        QVERIFY((e->type() == Qt3D::NodeDeleted || e->type() == Qt3D::NodeAboutToBeDeleted));
        m_lastChange = e;
    }

    void sceneNodeUpdated(Qt3D::QSceneChangePtr &e)
    {
        m_lastChange = e;
    }

    Qt3D::QSceneChangePtr lastChange() const
    {
        return m_lastChange;
    }

private:
    Qt3D::QSceneChangePtr m_lastChange;
};

void tst_QChangeArbiter::registerObservers()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    arbiter->setPostman(new tst_PostManObserver());
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    Qt3D::QNode *root = new tst_Node();
    Qt3D::QNode *child = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    QList<tst_SimpleObserver *> observers;
    for (int i = 0; i < 5; i++) {
        tst_SimpleObserver *s = new tst_SimpleObserver();
        arbiter->registerObserver(s, root->id());
        observers << s;
    }

    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers)
        QVERIFY(o->lastChange().isNull());

    root->addChild(child);

    arbiter->syncChanges();
    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }
}

void tst_QChangeArbiter::registerSceneObserver()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    arbiter->setPostman(new tst_PostManObserver());
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    tst_Node *root = new tst_Node();
    Qt3D::QNode *child = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    QList<tst_SimpleObserver *> observers;
    for (int i = 0; i < 5; i++) {
        tst_SimpleObserver *s = new tst_SimpleObserver();
        arbiter->registerObserver(s, root->id());
        observers << s;
    }

    QList<tst_SceneObserver *> sceneObservers;
    for (int i = 0; i < 5; i++) {
        tst_SceneObserver *s = new tst_SceneObserver();
        arbiter->registerSceneObserver(s);
        sceneObservers << s;
    }

    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers)
        QVERIFY(o->lastChange().isNull());
    Q_FOREACH (tst_SceneObserver *s, sceneObservers)
        QVERIFY(s->lastChange().isNull());

    root->addChild(child);

    arbiter->syncChanges();
    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeCreated);
    }

    root->sendComponentAddedNotification();
    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::ComponentAdded);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeCreated);
    }
}

void tst_QChangeArbiter::unregisterObservers()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    arbiter->setPostman(new tst_PostManObserver());
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    tst_Node *root = new tst_Node();
    Qt3D::QNode *child = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    QList<tst_SimpleObserver *> observers;
    for (int i = 0; i < 5; i++) {
        tst_SimpleObserver *s = new tst_SimpleObserver();
        arbiter->registerObserver(s, root->id());
        observers << s;
    }

    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers)
        QVERIFY(o->lastChange().isNull());

    root->addChild(child);

    arbiter->syncChanges();
    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }

    Q_FOREACH (tst_SimpleObserver *o, observers)
        arbiter->unregisterObserver(o, root->id());

    root->sendAllChangesNotification();
    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }
}

void tst_QChangeArbiter::unregisterSceneObservers()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    arbiter->setPostman(new tst_PostManObserver());
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    tst_Node *root = new tst_Node();
    Qt3D::QNode *child = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    QList<tst_SimpleObserver *> observers;
    for (int i = 0; i < 5; i++) {
        tst_SimpleObserver *s = new tst_SimpleObserver();
        arbiter->registerObserver(s, root->id());
        observers << s;
    }

    QList<tst_SceneObserver *> sceneObservers;
    for (int i = 0; i < 5; i++) {
        tst_SceneObserver *s = new tst_SceneObserver();
        arbiter->registerSceneObserver(s);
        sceneObservers << s;
    }

    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers)
        QVERIFY(o->lastChange().isNull());
    Q_FOREACH (tst_SceneObserver *s, sceneObservers)
        QVERIFY(s->lastChange().isNull());

    root->addChild(child);

    arbiter->syncChanges();
    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeCreated);
    }

    root->sendComponentAddedNotification();
    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::ComponentAdded);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeCreated);
    }

    root->removeAllChildren();
    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeAboutToBeDeleted);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeAboutToBeDeleted);
    }

    Q_FOREACH (tst_SceneObserver *s, sceneObservers)
        arbiter->unregisterSceneObserver(s);

    root->addChild(child);
    arbiter->syncChanges();

    Q_FOREACH (tst_SimpleObserver *o, observers) {
        QVERIFY(!o->lastChange().isNull());
        QVERIFY(o->lastChange()->type() == Qt3D::NodeCreated);
    }
    Q_FOREACH (tst_SceneObserver *s, sceneObservers) {
        QVERIFY(!s->lastChange().isNull());
        QVERIFY(s->lastChange()->type() == Qt3D::NodeAboutToBeDeleted);
    }
}

void tst_QChangeArbiter::distributeFrontendChanges()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    arbiter->setPostman(new tst_PostManObserver());
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    tst_Node *root = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    tst_SimpleObserver *backendAllChangedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendNodeAddedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendNodeRemovedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendNodeUpdatedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendComponentAddedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendComponentUpdatedObserver = new tst_SimpleObserver();
    tst_SimpleObserver *backendComponentRemovedObserver = new tst_SimpleObserver();

    arbiter->registerObserver(backendAllChangedObserver, root->id());
    arbiter->registerObserver(backendNodeAddedObserver, root->id(), Qt3D::NodeAdded);
    arbiter->registerObserver(backendNodeUpdatedObserver, root->id(), Qt3D::NodeUpdated);
    arbiter->registerObserver(backendNodeRemovedObserver, root->id(), Qt3D::NodeRemoved);
    arbiter->registerObserver(backendComponentAddedObserver, root->id(), Qt3D::ComponentAdded);
    arbiter->registerObserver(backendComponentUpdatedObserver, root->id(), Qt3D::ComponentUpdated);
    arbiter->registerObserver(backendComponentRemovedObserver, root->id(), Qt3D::ComponentRemoved);

    arbiter->syncChanges();

    QVERIFY(backendAllChangedObserver->lastChange().isNull());
    QVERIFY(backendNodeAddedObserver->lastChange().isNull());
    QVERIFY(backendNodeUpdatedObserver->lastChange().isNull());
    QVERIFY(backendNodeRemovedObserver->lastChange().isNull());
    QVERIFY(backendComponentAddedObserver->lastChange().isNull());
    QVERIFY(backendComponentUpdatedObserver->lastChange().isNull());
    QVERIFY(backendComponentRemovedObserver->lastChange().isNull());

    root->sendNodeAddedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 0);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 0);

    root->sendNodeUpdatedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 2);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 0);


    root->sendNodeRemovedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 3);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 0);

    root->sendComponentAddedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 4);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 0);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 0);

    root->sendComponentUpdatedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 5);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 0);

    root->sendComponentRemovedNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 6);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 1);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 1);

    root->sendAllChangesNotification();
    arbiter->syncChanges();

    QCOMPARE(backendAllChangedObserver->lastChanges().count(), 7);
    QCOMPARE(backendNodeAddedObserver->lastChanges().count(), 2);
    QCOMPARE(backendNodeUpdatedObserver->lastChanges().count(), 2);
    QCOMPARE(backendNodeRemovedObserver->lastChanges().count(), 2);
    QCOMPARE(backendComponentAddedObserver->lastChanges().count(), 2);
    QCOMPARE(backendComponentUpdatedObserver->lastChanges().count(), 2);
    QCOMPARE(backendComponentRemovedObserver->lastChanges().count(), 2);
}

void tst_QChangeArbiter::distributeBackendChanges()
{
    Qt3D::QChangeArbiter *arbiter = new Qt3D::QChangeArbiter();
    Qt3D::QScene *scene = new Qt3D::QScene();
    tst_PostManObserver *postMan = new tst_PostManObserver();

    arbiter->setPostman(postMan);
    arbiter->setScene(scene);
    scene->setArbiter(arbiter);
    postMan->setScene(scene);
    // Replace initialize as we have not JobManager in this case
    Qt3D::QChangeArbiter::createThreadLocalChangeQueue(arbiter);

    tst_Node *root = new tst_Node();
    root->setScene(scene);
    scene->addObservable(root);

    tst_ObserverObservable *backenObserverObservable = new tst_ObserverObservable();
    arbiter->registerObserver(backenObserverObservable, root->id());
    arbiter->scene()->addObservable(backenObserverObservable, root->id());

    arbiter->syncChanges();
    QVERIFY(root->lastChange().isNull());
    QVERIFY(backenObserverObservable->lastChange().isNull());

    root->sendAllChangesNotification();
    arbiter->syncChanges();

    QCOMPARE(root->lastChanges().count(), 0);
    QCOMPARE(backenObserverObservable->lastChanges().count(), 1);

    // To send backendObservableReply
    arbiter->syncChanges();
    QCOMPARE(root->lastChanges().count(), 1);
    QCOMPARE(backenObserverObservable->lastChanges().count(), 2);
    Qt3D::QBackendScenePropertyChangePtr c = qSharedPointerDynamicCast<Qt3D::QBackendScenePropertyChange>(root->lastChange());
    QVERIFY(!c.isNull());
    QVERIFY(c->targetNode() == root->id());
    QVERIFY(c->propertyName() == QByteArrayLiteral("Reply"));
    QVERIFY(c->type() == Qt3D::NodeUpdated);
}
#endif

QTEST_APPLESS_MAIN(tst_QChangeArbiter)

#include "tst_qchangearbiter.moc"
