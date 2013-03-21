#ifndef BOOKMEDIAEDITOR_H
#define BOOKMEDIAEDITOR_H

#include <qdialog.h>
#include "librarybook.h"

#include <qstandarditemmodel.h>
#include <quazip.h>
#include <quazipfile.h>

namespace Ui {
class BookMediaEditor;
}

class WebView;
class BookEditor;

class BookMediaResource
{
public:
    BookMediaResource() : op(None) {}

    enum Operation {
        None,
        Add,
        Remove
    };

    QString fileName;
    QString path;
    QByteArray data;
    Operation op;
};

class BookMediaEditor : public QDialog
{
    Q_OBJECT

public:
    BookMediaEditor(QWidget *parent = 0);
    ~BookMediaEditor();

    void setBook(LibraryBook::Ptr book);

    void saveChanges(BookEditor *bookEditor);

    BookMediaResource* getMediaByPath(QString path);
    BookMediaResource* getMediaByFileName(QString fileName);

    bool mediaEdited() const;

protected:
    void setupModel();
    void openBook();
    void closeBook();
    void loadImages();
    void addImage(QString path);

    void addResoourceToModel(BookMediaResource *resources);

    void imageGallery();

protected slots:
    void addMedia();
    void removeMedia();
    void modelSelectionChanged(const QModelIndex &current,
                               const QModelIndex &previous);

    void modelDoubleClick(const QModelIndex &index);

signals:
    void insertImage(QString src);

protected:
    Ui::BookMediaEditor *ui;
    QStandardItemModel *m_model;
    QStandardItem *m_imagesItem;
    WebView *m_webView;
    LibraryBook::Ptr m_book;
    QuaZip m_zip;
    QList<BookMediaResource*> m_resources;
    bool m_mediaEdited;
};

#endif // BOOKMEDIAEDITOR_H
