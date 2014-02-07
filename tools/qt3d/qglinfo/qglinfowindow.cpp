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

#include "qglinfowindow.h"
#include "qglinfo.h"
#include "ui_qglinfowindow.h"
#include "aboutdialog.h"
#include "fpswidget.h"

#include <QtOpenGL/qgl.h>
#include <QtWidgets/qfiledialog.h>
#include <QtGui/qfont.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/QTextStream>

QGLInfoWindow::QGLInfoWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QGLInfoWindow)
{
    ui->setupUi(this);
    setStatusBar(0);
    if (QApplication::arguments().contains(QLatin1String("-maximize")) ||
        QApplication::arguments().contains(QLatin1String("-fullscreen")))
    {
        ui->verticalLayout->setMargin(1);
        ui->verticalLayout->setContentsMargins(1, 1, 1, 1);
        ui->verticalLayout_2->setMargin(1);
        ui->verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        ui->menubar->setVisible(false);
        connect(ui->quitPushButton, SIGNAL(clicked()),
                this, SLOT(on_actionQuit_triggered()));
        connect(ui->runFPSTestPushButton, SIGNAL(clicked()),
                this, SLOT(on_actionRun_FPS_Test_triggered()));
        connect(ui->saveAsPushButton, SIGNAL(clicked()),
                this, SLOT(on_action_Save_As_triggered()));
        QFont f = font();
        f.setPointSize(f.pointSize() * 1.4);
        setFont(f);
    }
    else
    {
        ui->buttonsWidget->setVisible(false);
    }
    info = new QGLInfo();
    connect(info, SIGNAL(reportHtml(QString)),
            ui->textBrowser, SLOT(setHtml(QString)));
    ui->fpsWidget->setVisible(false);
}

QGLInfoWindow::~QGLInfoWindow()
{
    delete ui;
    delete info;
}

void QGLInfoWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QGLInfoWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void QGLInfoWindow::on_action_Save_As_triggered()
{
    QString defName = QDir::home().absoluteFilePath(QLatin1String("qglinfo.txt"));

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                defName,
                                tr("Text (*.txt)"));
    if (!fileName.isEmpty())
    {
        QFile saveFile(fileName);
        saveFile.open(QFile::WriteOnly);
        {
            QTextStream s(&saveFile);
            s << info->report();
        }
        saveFile.close();
    }
}

void QGLInfoWindow::on_actionAbout_triggered()
{
    AboutDialog *about = new AboutDialog(this);
    about->exec();
}

void QGLInfoWindow::on_actionRun_FPS_Test_triggered()
{
    fps = new FPSWidget(0);
    fps->setGeometry(rect());
    fps->move(this->pos());
    connect(fps, SIGNAL(fps(int)),
            ui->fpsLcdNumber, SLOT(display(int)));
    ui->fpsWidget->setVisible(true);
    fps->show();
}

void QGLInfoWindow::on_actionCopy_triggered()
{
    if (ui->textBrowser->textCursor().hasSelection()) {
        ui->textBrowser->copy();
    }
    else {
        ui->textBrowser->selectAll();
        ui->textBrowser->copy();
        
        QTextCursor tc(ui->textBrowser->textCursor());
        tc.clearSelection();
        tc.setPosition(0);
        ui->textBrowser->setTextCursor(tc);
    }
}
