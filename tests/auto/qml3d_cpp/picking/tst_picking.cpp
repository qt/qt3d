/***************************************************************************
**
** Copyright (C) 2011 - 2013 Research In Motion
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

#include <QtTest/QtTest>
#include <QtQuick/qquickview.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QQuickItem>
#include <qpa/qwindowsysteminterface.h>

class tst_Picking : public QObject
{
    Q_OBJECT
public:
    tst_Picking() {}
    ~tst_Picking() {}

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testWithNavigation();
    void testWithoutNavigation();
    void testMove();

private:
    void testMouse();

private:
    QQuickItem *rootObject;
    QQuickView *window;
};

void tst_Picking::initTestCase()
{
    window = new QQuickView(0);
    window->setSource(QUrl::fromLocalFile(QFINDTESTDATA("data/tst_picking.qml")));
    window->setGeometry(0,0,480,480);
    window->show();
    QTest::qWaitForWindowExposed(window);
    rootObject = window->rootObject();
}

void tst_Picking::cleanupTestCase()
{
    delete window;
}

void tst_Picking::testWithNavigation()
{
    rootObject->setProperty("navigation", true);
    testMouse();
}

void tst_Picking::testWithoutNavigation()
{
    rootObject->setProperty("navigation", false);
    testMouse();
}

void tst_Picking::testMouse()
{
    QTest::mousePress(window, Qt::LeftButton, 0, QPoint(240, 90));
    QTest::mouseRelease(window, Qt::LeftButton, 0, QPoint(240, 90), 300);

    QTRY_COMPARE(rootObject->property("pickedObjectPressed").toString(), QString("first"));
    QTRY_COMPARE(rootObject->property("pickedObjectClicked").toString(), QString("first"));
    QTRY_COMPARE(rootObject->property("pickedObjectReleased").toString(), QString("first"));

    QTest::mousePress(window, Qt::LeftButton, 0, QPoint(240, 320));
    QTest::mouseRelease(window, Qt::LeftButton, 0, QPoint(240, 240),300);

    QTRY_COMPARE(rootObject->property("pickedObjectPressed").toString(), QString("second"));
    QTRY_COMPARE(rootObject->property("pickedObjectClicked").toString(), QString("first"));
    QTRY_COMPARE(rootObject->property("pickedObjectReleased").toString(), QString("second"));

    QTest::mousePress(window, Qt::LeftButton, 0, QPoint(20, 100));
    QTest::mouseRelease(window, Qt::LeftButton, 0, QPoint(20, 100), 300);

    QTRY_COMPARE(rootObject->property("pickedObjectPressed").toString(), QString("second"));
    QTRY_COMPARE(rootObject->property("pickedObjectClicked").toString(), QString("first"));
    QTRY_COMPARE(rootObject->property("pickedObjectReleased").toString(), QString("second"));
}

void tst_Picking::testMove()
{
    rootObject->setProperty("hoveredObject", "");
    QTest::mouseMove(window, QPoint(240, 90));
    QTRY_COMPARE(rootObject->property("hoveredObject").toString(), QString("first"));

    QTest::mouseMove(window, QPoint(240, 200));
    QTRY_COMPARE(rootObject->property("hoveredObject").toString(), QString(""));
}

QTEST_MAIN(tst_Picking)

#include "tst_picking.moc"
