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

#include "qglinfo.h"

#include <QtGui/qopenglframebufferobject.h>
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/qglpixelbuffer.h>
#include <QtOpenGL/QGLShaderProgram>

#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>

#include <QtCore/qsettings.h>

#if !defined(QT_NO_EGL) && defined(QT_BUILD_INTERNAL)
#include <QtGui/private/qegl_p.h>
#include <QtGui/private/qeglproperties_p.h>
#endif

QGLInfo::QGLInfo(QObject *parent)
    : QObject(parent)
{
    QTimer::singleShot(0, this, SLOT(initialize()));
}

static QString nice(const QString &s)
{
    QString r(s);
    r.replace(QLatin1String("\n"), QLatin1String("<br>\n"));
    r.replace(QLatin1String("true"), QLatin1String("<span style=\"color: green\">true</span>"));
    r.replace(QLatin1String("false"), QLatin1String("<span style=\"color: red\">false</span>"));
    return r;
}

void QGLInfo::initialize()
{
    QWidget *win = qobject_cast<QGLWidget *>(parent());
    // We need some kind of GL context to do the querying.
    QGLWidget *glWidget = new QGLWidget(win);
    glWidget->makeCurrent();
    m_qtGLVersionInfo = reportQtGLVersionInfo();
    m_qtGLFeatures = reportQtGLFeatures();
    m_glVersionInfo = reportGLVersionInfo();
    m_glExtensionInfo = reportGLExtensionInfo();
    m_eglVersionInfo = reportEGLVersionInfo();
    m_eglExtensionInfo = reportEGLExtensionInfo();
    m_eglConfigInfo = reportEGLConfigInfo();
    glWidget->doneCurrent();
    delete glWidget;

    QString welcome;
    {
        QSettings freshStart;
        if (!freshStart.contains(QLatin1String("new_install")))
        {
            welcome = QLatin1String("<h1>Welcome to Qt3D!</h1>"
                                    "<p>Try running the FPS test from the "
                                    "View menu above to confirm that Qt3D "
                                    "is installed correctly.</p><hr>");
        }
        freshStart.setValue(QLatin1String("new_install"), true);
    }

    QString html = tr("<h1>Qt GL Info Report</h1>"
                      "<p>Generated at: %1</p>"
                      "<h2>Qt GL Version Info</h2>"
                      "<p>%2</p>"
                      "<h2>Qt GL Features</h2>"
                      "<p>%3</p>"
                      "<h2>GL Version Info</h2>"
                      "<p>%4</p>"
                      "<h2>GL Extension Info</h2>"
                      "<p>%5</p>")
            .arg(QDateTime::currentDateTime().toString())
            .arg(nice(m_qtGLVersionInfo))
            .arg(nice(m_qtGLFeatures))
            .arg(nice(m_glVersionInfo))
            .arg(nice(m_glExtensionInfo));
    if (!welcome.isEmpty())
        html.prepend(welcome);

#if !defined(QT_NO_EGL)
    html += tr("<h2>EGL Version Info</h2>"
               "<p>%1</p>"
               "<h2>EGL Extension Info</h2>"
               "<p>%2</p>"
               "<h2>EGL Configurations</h2>"
               "<p>%3</p>")
            .arg(nice(m_eglVersionInfo))
            .arg(nice(m_eglExtensionInfo))
            .arg(nice(m_eglConfigInfo));
#endif
    emit reportHtml(html);
}

QString QGLInfo::report() const
{
    QString report;
    report += m_qtGLVersionInfo;
    report += QLatin1Char('\n');
    report += m_qtGLFeatures;
    report += m_glVersionInfo;
    report += tr("OpenGL extensions:\n");
    report += m_glExtensionInfo;
#if !defined(QT_NO_EGL)
    report += m_eglVersionInfo;
    report += tr("EGL extensions:\n");
    report += m_eglExtensionInfo;
    report += tr("EGL configurations:\n");
    report += m_eglConfigInfo;
#endif
    return report;
}

QString QGLInfo::reportQtGLVersionInfo() const
{
    // Dump what Qt thinks the version is.
    QGLFormat::OpenGLVersionFlags flags = QGLFormat::openGLVersionFlags();
    QByteArray version;
    if ((flags & QGLFormat::OpenGL_Version_1_1) != 0)
        version += "1.1 ";
    if ((flags & QGLFormat::OpenGL_Version_1_2) != 0)
        version += "1.2 ";
    if ((flags & QGLFormat::OpenGL_Version_1_3) != 0)
        version += "1.3 ";
    if ((flags & QGLFormat::OpenGL_Version_1_4) != 0)
        version += "1.4 ";
    if ((flags & QGLFormat::OpenGL_Version_1_5) != 0)
        version += "1.5 ";
    if ((flags & QGLFormat::OpenGL_Version_2_0) != 0)
        version += "2.0 ";
    if ((flags & QGLFormat::OpenGL_Version_2_1) != 0)
        version += "2.1 ";
    if ((flags & QGLFormat::OpenGL_Version_3_0) != 0)
        version += "3.0 ";
    if ((flags & QGLFormat::OpenGL_Version_3_1) != 0)
        version += "3.1 ";
    if ((flags & QGLFormat::OpenGL_Version_3_2) != 0)
        version += "3.2 ";
    if ((flags & QGLFormat::OpenGL_Version_3_3) != 0)
        version += "3.3 ";
    if ((flags & QGLFormat::OpenGL_Version_4_0) != 0)
        version += "4.0 ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_0) != 0)
        version += "ES/1.0 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_0) != 0)
        version += "ES/1.0CL ";
    if ((flags & QGLFormat::OpenGL_ES_Common_Version_1_1) != 0)
        version += "ES/1.1 ";
    if ((flags & QGLFormat::OpenGL_ES_CommonLite_Version_1_1) != 0)
        version += "ES/1.1CL ";
    if ((flags & QGLFormat::OpenGL_ES_Version_2_0) != 0)
        version += "ES/2.0 ";
    flags &= ~(QGLFormat::OpenGL_Version_1_1 |
               QGLFormat::OpenGL_Version_1_2 |
               QGLFormat::OpenGL_Version_1_3 |
               QGLFormat::OpenGL_Version_1_4 |
               QGLFormat::OpenGL_Version_1_5 |
               QGLFormat::OpenGL_Version_2_0 |
               QGLFormat::OpenGL_Version_2_1 |
               QGLFormat::OpenGL_Version_3_0 |
               QGLFormat::OpenGL_Version_3_1 |
               QGLFormat::OpenGL_Version_3_2 |
               QGLFormat::OpenGL_Version_3_3 |
               QGLFormat::OpenGL_Version_4_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_0 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_0 |
               QGLFormat::OpenGL_ES_Common_Version_1_1 |
               QGLFormat::OpenGL_ES_CommonLite_Version_1_1 |
               QGLFormat::OpenGL_ES_Version_2_0);
    if (flags != 0)
        version += "Other=0x" + QByteArray::number(int(flags), 16);
    return QLatin1String("QGLFormat::openGLVersionFlags: ") +
           QString::fromLatin1(version.constData());
}

static QString printBool(const char *text, bool value)
{
    return QLatin1String(text) + (value ? QLatin1String("true\n") : QLatin1String("false\n"));
}

QString QGLInfo::reportQtGLFeatures() const
{
    QString d;
    d += printBool("QGLFormat::hasOpenGL: ", QGLFormat::hasOpenGL());
    d += printBool("QGLFormat::hasOpenGLOverlays: ", QGLFormat::hasOpenGLOverlays());
    d += printBool("QGLPixelBuffer::hasOpenGLPbuffers: ", QGLPixelBuffer::hasOpenGLPbuffers());
    d += printBool("QOpenGLFramebufferObject::hasOpenGLFramebufferObjects: ",
              QOpenGLFramebufferObject::hasOpenGLFramebufferObjects());
    d += printBool("QOpenGLFramebufferObject::hasOpenGLFramebufferBlit: ",
              QOpenGLFramebufferObject::hasOpenGLFramebufferBlit());
    d += printBool("QGLShaderProgram::hasOpenGLShaderPrograms: ",
              QGLShaderProgram::hasOpenGLShaderPrograms());
    return d;
}

QString QGLInfo::reportGLVersionInfo() const
{
    QString d;
    d += QLatin1String("OpenGL vendor string: ");
    d += QLatin1String(reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
    d += QLatin1String("\n");
    d += QLatin1String("OpenGL renderer string: ");
    d += QLatin1String(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    d += QLatin1String("\n");
    d += QLatin1String("OpenGL version string: ");
    d += QLatin1String(reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    d += QLatin1String("\n");
    return d;
}

QString QGLInfo::reportGLExtensionInfo() const
{
    QByteArray extString
        (reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
    return formatExtensions(extString);
}

QString QGLInfo::formatExtensions(const QByteArray& extString) const
{
    QString d;
    QList<QByteArray> extns = extString.split(' ');
    qSort(extns);
    QByteArray line;
    foreach (QByteArray extn, extns) {
        if (extn.isEmpty())
            continue;
        if (!line.isEmpty() && (line.size() + extn.size() + 1) > 70) {
            d += QLatin1String("    ") +
                 QString::fromLatin1(line.constData()) +
                 QLatin1String("\n");
            line = QByteArray();
        }
        line += extn;
        line += char(' ');
    }
    if (!line.isEmpty()) {
        d += QLatin1String("    ") +
             QString::fromLatin1(line.constData()) +
             QLatin1String("\n");
    }
    return d;
}

QString QGLInfo::reportEGLVersionInfo() const
{
#if !defined(QT_NO_EGL) && defined(QT_BUILD_INTERNAL)
    QString d;
    EGLDisplay dpy = eglGetCurrentDisplay();
    d += "EGL vendor string: ";
    d += reinterpret_cast<const char *>(eglQueryString(dpy, EGL_VENDOR));
    d += "\n";
    d += "EGL version string: ";
    d += reinterpret_cast<const char *>(eglQueryString(dpy, EGL_VERSION));
    d += "\n";
#ifdef EGL_CLIENT_APIS
    d += "EGL client API's: ";
    d += reinterpret_cast<const char *>(eglQueryString(dpy, EGL_CLIENT_APIS));
    d += "\n";
#endif
    return d;
#else
    return QString();
#endif
}

QString QGLInfo::reportEGLExtensionInfo() const
{
#if !defined(QT_NO_EGL) && defined(QT_BUILD_INTERNAL)
    EGLDisplay dpy = eglGetCurrentDisplay();
    QByteArray extString
        (reinterpret_cast<const char *>(eglQueryString(dpy, EGL_EXTENSIONS)));
    return formatExtensions(extString);
#else
    return QString();
#endif
}

QString QGLInfo::reportEGLConfigInfo() const
{
#if !defined(QT_NO_EGL) && defined(QT_BUILD_INTERNAL)
    QString d;
    QEglProperties props;
    EGLint count = 0;
    EGLDisplay dpy = eglGetCurrentDisplay();
    EGLContext ctx = eglGetCurrentContext();
    EGLint cfgnum = 0;
    if (eglQueryContext(dpy, ctx, EGL_CONFIG_ID, &cfgnum)) {
        d += QLatin1String("Window configuration in use: ") + QString::number(cfgnum) +
             QLatin1String("\n\n");
    }
    if (!eglGetConfigs(dpy, 0, 0, &count) || count < 1)
        return d;
    EGLConfig *configs = new EGLConfig [count];
    eglGetConfigs(dpy, configs, count, &count);
    for (EGLint index = 0; index < count; ++index) {
        props = QEglProperties(configs[index]);
        d += props.toString() + QLatin1String("\n\n");
    }
    delete [] configs;
    return d;
#else
    return QString();
#endif
}
