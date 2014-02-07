/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#include <QQmlEngine>
#include <QQmlComponent>

class tst_matrix_properties : public QObject
{
    Q_OBJECT
public:
    tst_matrix_properties() {}

private slots:
    void initTestCase();

    void create_data();
    void create();

    void modify_data();
    void modify();

    void boundVariableChange_data();
    void boundVariableChange();

//    TODO:
//    void assign();

private:
};

inline QUrl TEST_FILE(const char *filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + QLatin1String(filename));
}

void tst_matrix_properties::initTestCase()
{
}

void tst_matrix_properties::create_data()
{
    QTest::addColumn<QString>("methodName");
    QTest::newRow("createMatrix4x4WithConstants") << "createMatrix4x4WithConstants()";
    QTest::newRow("createMatrix4x4WithVariables") << "createMatrix4x4WithVariables()";
    QTest::newRow("createVariantListWithConstants") << "createVariantListWithConstants()";
    QTest::newRow("createVariantListWithVariables") << "createVariantListWithVariables()";
}

void tst_matrix_properties::create()
{
    QFETCH(QString, methodName);
    QQmlEngine engine;
    QQmlComponent component(&engine, TEST_FILE("matrix_component.qml"));
    QObject *item = component.create();

    QVERIFY(item != 0);

    int index = item->metaObject()->indexOfMethod(methodName.toUtf8().constData());
    QVERIFY(index != -1);
    QMetaMethod method = item->metaObject()->method(index);

    QBENCHMARK {
        method.invoke(item, Qt::DirectConnection);
    }

    delete item;
}

void tst_matrix_properties::modify_data()
{
    QTest::addColumn<QString>("initMethodName");
    QTest::addColumn<QString>("methodName");
    QTest::newRow("modifyMatrix4x4") << "createOneMatrix4x4WithConstants()"
            << "modifyMatrix4x4()";
    QTest::newRow("modifyVariantList") << "createOneVariantListWithConstants()"
            << "modifyVariantList()";
}

void tst_matrix_properties::modify()
{
    QFETCH(QString, initMethodName);
    QFETCH(QString, methodName);
    QQmlEngine engine;
    QQmlComponent component(&engine, TEST_FILE("matrix_component.qml"));
    QObject *item = component.create();

    QVERIFY(item != 0);

    // Set the test property to a matrix or variantlist as appropriate.
    int index = item->metaObject()->indexOfMethod(initMethodName.toUtf8().constData());
    QVERIFY(index != -1);
    QMetaMethod method = item->metaObject()->method(index);
    method.invoke(item, Qt::DirectConnection);

    index = item->metaObject()->indexOfMethod(methodName.toUtf8().constData());
    QVERIFY(index != -1);
    method = item->metaObject()->method(index);

    QBENCHMARK {
        method.invoke(item, Qt::DirectConnection);
    }

    delete item;
}

void tst_matrix_properties::boundVariableChange_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::newRow("UnboundVariable")
            << "unboundReal";
    QTest::newRow("BoundOnMatrix")
            << "variableBoundToMatrix";
    QTest::newRow("BoundOnVariantList")
            << "variableBoundToVariantList";
}

void tst_matrix_properties::boundVariableChange()
{
    QFETCH(QString, propertyName);
    QQmlEngine engine;
    QQmlComponent component(&engine, TEST_FILE("matrix_component.qml"));
    QObject *item = component.create();

    QVERIFY(item != 0);

    QByteArray propertyNameByteArray = propertyName.toUtf8().constData();
    float value = 1.0f;

    QBENCHMARK {
        item->setProperty(propertyNameByteArray.constData(), (value += 0.1f));
    }

    delete item;
}

QTEST_MAIN(tst_matrix_properties)

#include "tst_matrix_properties.moc"
