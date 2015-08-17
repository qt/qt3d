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
#include <Qt3DCore/qaspectengine.h>
#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

using namespace Qt3D;

class tst_QAspectEngine : public QObject
{
    Q_OBJECT
public:
    tst_QAspectEngine() : QObject() {}
    ~tst_QAspectEngine() {}

private Q_SLOTS:
    void constructionDestruction();
    void setRootEntity();
    void shouldNotCrashOnShutdownWhenComponentIsCreatedWithParentBeforeItsEntity();

    // TODO: Add more QAspectEngine tests
};

void tst_QAspectEngine::constructionDestruction()
{
    QAspectEngine *engine = new QAspectEngine;
    QVERIFY(engine->rootEntity() == Q_NULLPTR);
    delete engine;
}

void tst_QAspectEngine::setRootEntity()
{
    QAspectEngine *engine = new QAspectEngine;

    QEntity *e = new QEntity;
    e->setObjectName("root");
    engine->setRootEntity(e);

    QSharedPointer<QEntity> root = engine->rootEntity();
    QVERIFY(root == e);
    QVERIFY(root->objectName() == "root");
    root = QSharedPointer<QEntity>();
    QVERIFY(engine->rootEntity()->objectName() == "root");

    delete engine;
}

void tst_QAspectEngine::shouldNotCrashOnShutdownWhenComponentIsCreatedWithParentBeforeItsEntity()
{
    // GIVEN
    QEntity *root = new QEntity;
    // A component parented to an entity...
    QComponent *component = new Qt3D::QTransform(root);
    // ... created *before* the entity it will be added to.
    QEntity *entity = new QEntity(root);
    entity->addComponent(component);

    // An initialized engine (so that the arbiter has been fed)
    QAspectEngine engine;
    engine.initialize();

    // WHEN
    engine.setRootEntity(root);

    // THEN
    // Nothing particular happen on exit, especially no crash
}

QTEST_MAIN(tst_QAspectEngine)

#include "tst_qaspectengine.moc"
