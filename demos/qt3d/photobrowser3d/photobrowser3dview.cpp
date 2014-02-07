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

#include "photobrowser3dview.h"
#include "imagemanager.h"
#include "imagedisplay.h"
#include "skybox.h"
#include "qglpicknode.h"
#include "qfocusadaptor.h"
#include "thumbnailableimage.h"
#include "qatlas.h"
#include "filescanner.h"
#include "buttons.h"
#include "qphotobrowser3dscene.h"
#include "pancontroller.h"
#include "thumbnailnode.h"

#include <QGuiApplication>
#include <QWheelEvent>
#include <QDir>
#include <QTimer>
#include <QTime>
#include <QStateMachine>
#include <QState>
#include <QFinalState>
#include <QSignalTransition>
#include <QPropertyAnimation>

PhotoBrowser3DView::PhotoBrowser3DView()
    : QGLView()
    , m_scene(0)
    , m_display(0)
    , m_images(0)
    , m_buttons(0)
    , m_skybox(0)
    , m_palette(new QGLMaterialCollection())
    , m_state(0)
    , m_app(0)
    , m_zoomed(0)
    , m_browse(0)
    , m_pan(0)
    , m_fa(0)
    , m_pc(0)
    , m_pickableDirty(true)
    , m_done(false)
    , m_closing(false)
{
    setOption(QGLView::ObjectPicking, true);
    // setOption(QGLView::ShowPicking, true);
    //setOption(QGLView::CameraNavigation, false);

    qRegisterMetaType<ThumbnailableImage>("ThumbnailableImage");

    QString path = ":/res";
    int ix = qApp->arguments().indexOf("--skybox");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected path/to/skybox/files after \"--skybox\" switch\n");
    }

    m_displaySize = 4.0;
    m_scene = new QPhotoBrowser3DScene(this);
    m_buttons = new Buttons(this, m_palette);
    m_scene->mainNode()->addNode(m_buttons);
    m_scene->setPickable(true);
    m_skybox = new SkyBox(this, path);
    m_display = new ImageDisplay(this, m_palette, m_displaySize);

    setupStates();

    // make sure this only gets created in the GUI thread
    QAtlas::instance();

    QTimer::singleShot(0, this, SLOT(initialise()));
}

PhotoBrowser3DView::~PhotoBrowser3DView()
{
    // nothing to be done here
}

void PhotoBrowser3DView::setupStates()
{
    m_state = new QStateMachine(this);
    m_app = new QState;
    m_zoomed = new QState(m_app);
    m_browse = new QState(m_app);
    m_pan = new QState(m_app);
    m_app->setInitialState(m_browse);
    m_state->addState(m_app);
    QFinalState *end_state = new QFinalState;
    m_app->addTransition(this, SIGNAL(done()), end_state);
    m_state->addState(end_state);
    connect(m_state, SIGNAL(finished()), this, SLOT(close()));

    m_fa = new QFocusAdaptor(this);
    m_browse->assignProperty(m_fa, "progress", 0.0);
    m_zoomed->assignProperty(m_fa, "progress", 1.0);

    m_pc = new PanController(this);
    m_pc->setMaxSpeed(m_displaySize / 1000.0f);
    m_browse->assignProperty(m_pc, "speed", 0.0);
    m_pan->assignProperty(m_pc, "speed", 1.0);

    m_state->setObjectName("StateMachine");
    m_app->setObjectName("Application");
    m_zoomed->setObjectName("Zoomed");
    m_browse->setObjectName("Browse");
    m_pan->setObjectName("Pan");
    end_state->setObjectName("EndState");

    QSignalTransition *transition = m_browse->addTransition(this, SIGNAL(zoom()), m_zoomed);
    QPropertyAnimation *a = new QPropertyAnimation(m_fa, "progress");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::OutQuad);
    transition->addAnimation(a);

    transition = m_zoomed->addTransition(this, SIGNAL(zoom()), m_browse);
    a = new QPropertyAnimation(m_fa, "progress");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::InQuad);
    transition->addAnimation(a);

    transition = m_browse->addTransition(this, SIGNAL(pan()), m_pan);
    a = new QPropertyAnimation(m_pc, "speed");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::OutQuad);
    transition->addAnimation(a);

    transition = m_pan->addTransition(this, SIGNAL(pan()), m_browse);
    a = new QPropertyAnimation(m_pc, "speed");
    a->setDuration(500);
    a->setEasingCurve(QEasingCurve::InQuad);
    transition->addAnimation(a);

    m_state->setInitialState(m_app);
    m_state->start();
}

void PhotoBrowser3DView::initialise()
{
    QString path = QDir::home().absoluteFilePath("Pictures");
    int ix = qApp->arguments().indexOf("--pictures");
    if (ix != -1)
    {
        if (qApp->arguments().size() > ix+1)
            path = qApp->arguments().at(ix+1);
        else
            qWarning("Expected /path/to/image/files after \"--pictures\" switch\n");
    }

    QUrl url;
    url.setScheme("file");
    url.setPath(path);

    bool atLeastOneImageFound = false;
    QFileInfo fi(path);
    if (fi.exists() && fi.isDir())
    {
        QDir pics(path);
        QStringList entries = pics.entryList(QDir::Files);
        for (int i = 0; i < entries.size() && !atLeastOneImageFound; ++i)
        {
            QImage im(pics.filePath(entries.at(i)));
            if (!im.isNull())
                atLeastOneImageFound = true;
        }
    }
    if (!atLeastOneImageFound)
    {
        qWarning("No pictures directory found at %s\n"
                 "using test images", qPrintable(path));

        url.setPath(":/pictures");
    }

#ifdef QT_NO_THREADED_FILE_LOAD
    nonThreadedFileLoad(url);
#else
    initialiseImageManager(url);
#endif
}

void PhotoBrowser3DView::initialiseImageManager(const QUrl &url)
{
    m_images = new ImageManager;

    connect(m_images, SIGNAL(imageUrl(QUrl)), m_display, SLOT(addThumbnailNode(QUrl)));
    connect(m_images, SIGNAL(finished()), this, SLOT(waitForExit()));

    connect(m_display, SIGNAL(framesChanged()), this, SLOT(pickableDirty()));
    connect(m_display, SIGNAL(framesChanged()), this, SLOT(update()));

    m_images->setImageBaseUrl(url);
    QThread::Priority p = QThread::idealThreadCount() < 2 ?
                QThread::IdlePriority : QThread::NormalPriority;
    m_images->start(p);
}

void PhotoBrowser3DView::nonThreadedFileLoad(const QUrl &url)
{
#if defined(QT_USE_TEST_IMAGES)
    Q_UNUSED(url);
    QDir testImages(":/pictures");
    QStringList pics = testImages.entryList();
    for (int i = 0; i < pics.size(); ++i)
    {
        QUrl url;
        url.setScheme("file");
        url.setPath(testImages.filePath(pics.at(i)));
        m_display->addThumbnailNode(url);
    }
    pickableDirty();
#else
    FileScanner *scanner = new FileScanner(this);
    scanner->setBaseUrl(url);
    QTimer::singleShot(0, scanner, SLOT(scan()));
    connect(scanner, SIGNAL(imageUrl(QUrl)), m_display, SLOT(addThumbnailNode(QUrl)));
#endif
}

void PhotoBrowser3DView::wheelEvent(QWheelEvent *e)
{
    e->accept();
    QVector3D viewVec = camera()->eye() - camera()->center();
    float zoomMag = viewVec.length();
    float inc = float(e->delta()) / 50.0f;
    if (!qFuzzyIsNull(inc))
    {
        zoomMag += inc;
        if (zoomMag < 2.0f)
            zoomMag = 2.0f;
        QRay3D viewLine(camera()->center(), viewVec.normalized());
        camera()->setEye(viewLine.point(zoomMag));
        update();
    }
}

void PhotoBrowser3DView::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Q)
    {
        m_done = true;
        emit done();
    }
    else if (e->key() == Qt::Key_Right)
    {
        m_pc->setDirection(Qt::RightArrow);
        emit pan();
    }
    else if (e->key() == Qt::Key_Left)
    {
        m_pc->setDirection(Qt::LeftArrow);
        emit pan();
    }
    else if (e->key() == Qt::Key_Up || e->key() == Qt::Key_Down)
    {
        QVector3D viewVec = camera()->eye() - camera()->center();
        float zoomMag = viewVec.length();
        zoomMag += (e->key() == Qt::Key_Up) ? -0.5f : 0.5f;
        if (zoomMag < 5.0f)
            zoomMag = 5.0f;
        QRay3D viewLine(camera()->center(), viewVec.normalized());
        camera()->setEye(viewLine.point(zoomMag));
        update();
    }
    else
    {
        QGLView::keyPressEvent(e);
    }
}

void PhotoBrowser3DView::waitForExit()
{
    QThread::yieldCurrentThread();
    m_images->wait();
    m_images->deleteLater();
    m_images = 0;
    if (m_closing)
    {
        if (!m_done)
        {
            emit done();
            m_done = true;
        }
    }
}

void PhotoBrowser3DView::hideEvent(QHideEvent *e)
{
    if (m_images)
    {
        e->ignore();
        m_images->stop();

        // this was a request to close the main window, so we are closing up shop
        // set this flag to indicate that when the image manager stops done event
        // should be signalled to the state machine, resulting in close
        m_closing = true;
    }
    else
    {
        e->accept();
    }
}

void PhotoBrowser3DView::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    registerPickableNodes();
    QGLView::mousePressEvent(e);
}

void PhotoBrowser3DView::initializeGL(QGLPainter *painter)
{
    Q_UNUSED(painter);
    QAtlas::instance()->initialize(painter);
    camera()->setEye(QVector3D(0.0f, 0.0f, 4.0f * m_displaySize));
    registerPickableNodes();
    float q = camera()->eye().z();
    float r = qBound(camera()->nearPlane(), q / 2.0f, camera()->nearPlane() * 3.0f);
    m_pc->setDefaultDistance(q);
    m_pc->setPanDistance(r);
}

void PhotoBrowser3DView::earlyPaintGL(QGLPainter *painter)
{
    Q_UNUSED(painter);

    if (!painter->isPicking())
    {
        QAtlas::instance()->paint(painter);
    }

    if (!m_done)
        m_pc->pan();

    painter->setClearColor(Qt::blue);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PhotoBrowser3DView::paintGL(QGLPainter *painter)
{
    if (!m_done)
    {
        glEnable(GL_BLEND);
        m_skybox->draw(painter);
        m_display->draw(painter);
        m_buttons->draw(painter);
    }
}

void PhotoBrowser3DView::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
    m_buttons->clearPositions();
    m_updateRequired = true;
}

void PhotoBrowser3DView::zoomImage()
{
    QGLPickNode *pn = qobject_cast<QGLPickNode*>(sender());
    Q_ASSERT(pn);
    QGLSceneNode *n = pn->target();
    m_fa->setTarget(n);
    emit zoom();
}

void PhotoBrowser3DView::goPan()
{
    QGLPickNode *pn = qobject_cast<QGLPickNode*>(sender());
    Q_ASSERT(pn);
    QGLSceneNode *n = pn->target();
    m_pc->setDirection(n->objectName() == "Left Button" ? Qt::LeftArrow : Qt::RightArrow);
    emit pan();
}

void PhotoBrowser3DView::pickableDirty()
{
    m_pickableDirty = true;
}

void PhotoBrowser3DView::registerPickableNodes()
{
    if (m_pickableDirty)
    {
        m_scene->generatePickNodes();
        QList<QGLPickNode*> pickList = m_scene->pickNodes();
        QList<QGLPickNode*>::const_iterator it = pickList.constBegin();
        for ( ; it != pickList.constEnd(); ++it)
        {
            QGLPickNode *pn = *it;
            pn->disconnect(this);
            ThumbnailNode *node = qobject_cast<ThumbnailNode*>(pn->target());
            if (node)
                QObject::connect(pn, SIGNAL(clicked()), this, SLOT(zoomImage()));
            else
                QObject::connect(pn, SIGNAL(clicked()), this, SLOT(goPan()));
            registerObject(pn->id(), pn);
        }
        m_pickableDirty = false;
    }
}
