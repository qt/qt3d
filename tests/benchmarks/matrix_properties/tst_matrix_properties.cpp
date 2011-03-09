/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
// Application private dir is default search path for files, so SRCDIR can be set to empty
#define SRCDIR "."
#endif

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
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("matrix_component.qml"));
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
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("matrix_component.qml"));
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
    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, TEST_FILE("matrix_component.qml"));
    QObject *item = component.create();

    QVERIFY(item != 0);

    QByteArray propertyNameByteArray = propertyName.toUtf8().constData();
    qreal value = 1.0;

    QBENCHMARK {
        item->setProperty(propertyNameByteArray.constData(), (value += 0.1));
    }

    delete item;
}

QTEST_MAIN(tst_matrix_properties)

#include "tst_matrix_properties.moc"
