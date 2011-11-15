#ifndef QUICKLOAD_H
#define QUICKLOAD_H

#include <QObject>
#include <QFile>
#include <QUrl>
#include <QFileDialog>
#include <QDebug>

class QuickLoad : public QObject {
    Q_OBJECT

    QString _filename;

    Q_PROPERTY(QString openModelFile READ openModelFile)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
public:
    QuickLoad(QObject *parent=0) : QObject(parent) {}

    QString openModelFile() {
        QString qmlFilename = QFileDialog::getOpenFileName(0, tr("Open Model File"), "",tr("Files (*.*)"));

        setFilename(qmlFilename);

        return qmlFilename;
    }

    QString filename() const { return _filename; }
    void setFilename(const QString filename) { if (_filename != filename) _filename = filename; emit filenameChanged(filename); }

signals:
    void filenameChanged(const QString newFilename);

};

#endif // QUICKLOAD_H
