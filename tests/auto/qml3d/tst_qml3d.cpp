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

#include <QApplication>
#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtOpenGL/qgl.h>
#include <QtCore/qurl.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>
#include <QtCore/qeventloop.h>
#include <QtGui/qtextdocument.h>
#include <QtTest/qtestcase.h>
#include <stdio.h>

class QuitObject : public QObject
{
    Q_OBJECT
public:
    QuitObject(QObject *parent = 0) : QObject(parent), hasQuit(false) {}

    bool hasQuit;

private Q_SLOTS:
    void quit() { hasQuit = true; }
};

static bool xmlOutput = false;
static int passed = 0;
static int failed = 0;
static int skipped = 0;
static FILE *stream = 0;

// A utility object to trigger the "clicked()" signal on a DeclarativeItem3d.
class Item3DClicker : public QObject
{
        Q_OBJECT
public:
        Item3DClicker(QObject *parent = 0) : QObject(parent) {}

public Q_SLOTS:
    Q_INVOKABLE bool click(QObject *target);
};

QML_DECLARE_TYPE(Item3DClicker)

bool Item3DClicker::click(QObject *target)
{
    if (target == 0)
        return false;
    const QMetaObject* metaTarget = target->metaObject();
    int clickedIndex = metaTarget->indexOfSignal("clicked()");
    if (clickedIndex == -1)
        return false;

    metaTarget->activate(target, clickedIndex, 0);
    return true;
}

class TestReport : public QObject
{
    Q_OBJECT
public:
    TestReport(QObject *parent = 0) : QObject(parent) {}

public Q_SLOTS:
    void report(int pass, int fail, int skip);
    void log_fail(const QString &testCase, const QString &message);
    void log_expect_fail
        (const QString &testCase, const QString &message);
    void log_expect_fail_pass(const QString &testCase);
    void log_skip(const QString &testCase, const QString &message);
    void log_pass(const QString &testCase);
    void log_message(const QString &message);

private:
    void log_incident(const char *type, const QString &testCase,
                      const QString &message);
};

QML_DECLARE_TYPE(TestReport)

void TestReport::report(int pass, int fail, int skip)
{
    passed += pass;
    failed += fail;
    skipped += skip;
}

void TestReport::log_fail(const QString &testCase, const QString &message)
{
    if (xmlOutput) {
        log_incident("fail", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "FAIL!  : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "FAIL!  : %s\n", testCase.toLatin1().constData());
    }
}

void TestReport::log_expect_fail
    (const QString &testCase, const QString &message)
{
    if (xmlOutput) {
        log_incident("xfail", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "XFAIL  : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "XFAIL  : %s\n", testCase.toLatin1().constData());
    }
}

void TestReport::log_expect_fail_pass(const QString &testCase)
{
    if (xmlOutput)
        log_incident("xpass", testCase, QString());
    else
        fprintf(stream, "XPASS  : %s\n", testCase.toLatin1().constData());
}

void TestReport::log_skip(const QString &testCase, const QString &message)
{
    if (xmlOutput) {
        log_incident("skip", testCase, message);
    } else if (!message.isEmpty()) {
        fprintf(stream, "SKIP   : %s %s\n",
                testCase.toLatin1().constData(),
                message.toLatin1().constData());
    } else {
        fprintf(stream, "SKIP   : %s\n", testCase.toLatin1().constData());
    }
}

void TestReport::log_pass(const QString &testCase)
{
    if (xmlOutput)
        log_incident("pass", testCase, QString());
    else
        fprintf(stream, "PASS   : %s\n", testCase.toLatin1().constData());
}

void TestReport::log_message(const QString &message)
{
    if (!xmlOutput)
        fprintf(stream, "%s\n", message.toLatin1().constData());
}

void TestReport::log_incident
    (const char *type, const QString &testCase, const QString &message)
{
    QString name(testCase);
    QString tag;
    name.replace(QLatin1String("()"), QLatin1String(""));
    name.replace(QLatin1String("::"), QLatin1String("__"));
    int tagIndex = name.indexOf(QLatin1String(" ["));
    if (tagIndex >= 0) {
        tag = name.mid(tagIndex + 2);
        if (tag.endsWith(QLatin1String("]")))
            tag = tag.left(tag.length() - 1);
        name = name.left(tagIndex);
    }
    fprintf(stream, "<TestFunction name=\"%s\">\n",
            Qt::escape(name).toLatin1().constData());
    if (message.isEmpty() && tag.isEmpty()) {
        fprintf(stream, "<Incident type=\"%s\" file=\"\" line=\"0\" />\n", type);
    } else {
        fprintf(stream, "<Incident type=\"%s\" file=\"\" line=\"0\">\n", type);
        if (!tag.isEmpty()) {
            fprintf(stream, "    <DataTag>%s</DataTag>\n",
                    Qt::escape(tag).toLatin1().constData());
        }
        if (!message.isEmpty()) {
            fprintf(stream, "    <Description>%s</Description>\n",
                    Qt::escape(message).toLatin1().constData());
        }
        fprintf(stream, "</Incident>\n");
    }
    fprintf(stream, "</TestFunction>\n");
}

class tst_qml3d : public QObject
{
    Q_OBJECT
public:
private slots:
    void initTestCase()
    {
        QSKIP("Disabling qml tests due to invalid GL context!", SkipSingle);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    {
        QGLWidget checkWidget;
        if (!checkWidget.isValid())
        {
            tst_qml3d xf;
            return QTest::qExec(&xf, argc, argv);
        }
    }

    // Parse the command-line arguments.
    const char *filename = 0;
    for (int index = 1; index < argc; ++index) {
        QString arg = QString::fromLocal8Bit(argv[index]);
        if (arg == QLatin1String("-xml"))
            xmlOutput = true;
        else if (arg == QLatin1String("-o") && (index + 1) < argc)
            filename = argv[++index];
    }

    // Determine where to look for the test data.  On a device it will
    // typically be necessary to set QML3D_TEST_SOURCE_DIR.
    QString testPath = QString::fromLocal8Bit(qgetenv("QML3D_TEST_SOURCE_DIR"));
#ifdef QML3D_TEST_SOURCE_DIR
    if (testPath.isEmpty())
        testPath = QString::fromLocal8Bit(QML3D_TEST_SOURCE_DIR);
#endif
    if (testPath.isEmpty())
        testPath = QLatin1String(".");

    // Find the subdirectories that look like they may contain test cases.
    // The "QtQuickTest" directory contains the common QML and JS test logic.
    QDir dir(testPath);
    QStringList entries = dir.entryList(QDir::Dirs);
    entries.removeAll(QLatin1String("."));
    entries.removeAll(QLatin1String(".."));
    entries.removeAll(QLatin1String("QtQuickTest"));

    if (filename) {
        stream = fopen(filename, "w");
        if (!stream) {
            perror(filename);
            return 1;
        }
    } else {
        stream = stdout;
    }

    if (xmlOutput) {
        fprintf(stream, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                        "<TestCase name=\"tst_qml3d\">\n");
        fprintf(stream, "<Environment>\n"
                        "   <QtVersion>%s</QtVersion>\n"
                        "   <QTestVersion>%s</QTestVersion>\n"
                        "</Environment>\n", qVersion(), qVersion());
    } else {
        fprintf(stream, "********* Start testing of tst_qml3d *********\n");
    }

    qmlRegisterType<TestReport>("QtQuickTest", 1, 0, "TestReport");
    qmlRegisterType<Item3DClicker>("QtQuickTest", 1, 0, "Item3DClicker");

    // Scan through all of the "*.qml" files in the subdirectories
    // and run each of them in turn with a QDeclarativeView.
    QStringList filters;
    filters += QLatin1String("tst_*.qml");
    foreach (QString name, entries) {
        QDir subdir(testPath + QDir::separator() + name);
        QStringList files = subdir.entryList(filters, QDir::Files);
        foreach (QString file, files) {
            QString source = subdir.path() + QDir::separator() + file;
            QFileInfo fi(source);
            if (fi.exists()) {
                QDeclarativeView view;
                QuitObject quitobj;
                QEventLoop eventLoop;
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &quitobj, SLOT(quit()));
                QObject::connect(view.engine(), SIGNAL(quit()),
                                 &eventLoop, SLOT(quit()));
                view.setViewport(new QGLWidget());
                view.engine()->addImportPath(testPath);
                view.setSource(QUrl::fromLocalFile(fi.absoluteFilePath()));
                if (view.status() == QDeclarativeView::Error) {
                    // Error compiling the test - flag failure and continue.
                    ++failed;
                    continue;
                }
                if (!quitobj.hasQuit) {
                    // If the test already quit, then it was performed
                    // synchronously during setSource().  Otherwise it is
                    // an asynchronous test and we need to show the window
                    // and wait for the quit indication.
                    view.show();
                    eventLoop.exec();
                }
            }
        }
    }

    if (xmlOutput) {
        fprintf(stream, "</TestCase>\n");
    } else {
        fprintf(stream, "Totals: %d passed, %d failed, %d skipped\n",
                passed, failed, skipped);
        fprintf(stream, "********* Finished testing of tst_qml3d *********\n");
    }

    if (filename)
        fclose(stream);

    return failed != 0;
}

#include "tst_qml3d.moc"
