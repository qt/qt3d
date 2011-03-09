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

#ifndef PHOTOBROWSER3DVIEW_H
#define PHOTOBROWSER3DVIEW_H

#include "qglview.h"

class ImageDisplay;
class SkyBox;
class QGLMaterialCollection;
class QGLSceneNode;
class ImageManager;
class QState;
class QStateMachine;
class QFocusAdaptor;
class QAtlas;
class Buttons;
class QPhotoBrowser3DScene;
class PanController;

class PhotoBrowser3DView : public QGLView
{
    Q_OBJECT
public:
    PhotoBrowser3DView();
    ~PhotoBrowser3DView();
    void initializeGL(QGLPainter *);
    QPhotoBrowser3DScene *scene() { return m_scene; }
signals:
    void done();
    void zoom();
    void pan();
protected:
    void earlyPaintGL(QGLPainter *);
    void paintGL(QGLPainter *);
    void resizeGL(int w, int h);
    void wheelEvent(QWheelEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
    void mousePressEvent(QMouseEvent *e);
private slots:
    void initialise();
    void zoomImage();
    void pickableDirty();
    void waitForExit();
    void goPan();
private:
    void registerPickableNodes();
    void setupStates();
    void nonThreadedFileLoad(const QUrl &url);
    void initialiseImageManager(const QUrl &url);

    QPhotoBrowser3DScene *m_scene;
    ImageDisplay *m_display;
    ImageManager *m_images;
    Buttons *m_buttons;
    SkyBox *m_skybox;
    QGLMaterialCollection *m_palette;
    QStateMachine *m_state;
    QState *m_app;
    QState *m_zoomed;
    QState *m_browse;
    QState *m_pan;
    QFocusAdaptor *m_fa;
    PanController *m_pc;
    bool m_pickableDirty;
    qreal m_displaySize;
    bool m_done;
    bool m_closing;
    bool m_updateRequired;
};

#endif // PHOTOBROWSER3DVIEW_H
