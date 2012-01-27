#ifndef QUICKFILE_H
#define QUICKFILE_H

#include <QObject>

class QuickFile : public QObject
{
    Q_OBJECT

    QString _filename;
    QString _title;
    QString _filter;
    QString _data;

    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit QuickFile(QObject *parent = 0);

    QString filename() const;
    void setFilename(const QString filename);

    QString title() const;
    void setTitle(const QString title);

    QString filter() const;
    void setFilter(const QString filter);

    QString data() const;
    void setData(const QString data);

    Q_INVOKABLE QString save() const;
    Q_INVOKABLE void load();
    Q_INVOKABLE QString promptSave() const;

signals:
    void filenameChanged(const QString newFilename);
    void titleChanged(const QString newTitle);
    void filterChanged(const QString newFilter);
    void dataChanged(const QString newData);
};

#endif // QUICKFILE_H
