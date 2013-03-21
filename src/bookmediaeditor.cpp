#include "bookmediaeditor.h"
#include "ui_bookmediaeditor.h"
#include "webview.h"
#include "utils.h"
#include "modelutils.h"
#include "htmlhelper.h"

#include <bookeditor.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qinputdialog.h>

#define IMAGES_PREFIX "images/"

const QString IMAGE_HTML_BASE_PREVIEW =
    "<html>"
    "<head>"
    "<style type=\"text/css\">"
    "body { -webkit-user-select: none; }"
    "img { display: block; margin-left: auto; margin-right: auto; border-style: solid; border-width: 1px; max-width: 95%; max-height: 95%}"
    "</style>"
    "<body>"
    "<div><img src=\"data:image/png;base64,%1\" /></div>"
    "</body>"
    "</html>";

enum {
    FileNameRole = Qt::UserRole + 10,
    FilePathRole,
    FileDataRole
};

BookMediaEditor::BookMediaEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookMediaEditor),
    m_model(0),
    m_imagesItem(0),
    m_mediaEdited(false)
{
    ui->setupUi(this);

    setupModel();

    m_webView = new WebView(this);
    m_webView->autoObjectAdd("bookMediaEditor", this);

    ui->verticalLayout->insertWidget(0, m_webView);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(accept()));
    connect(ui->pushAddMedia, SIGNAL(clicked()), SLOT(addMedia()));
    connect(ui->pushRemoveMedia, SIGNAL(clicked()), SLOT(removeMedia()));
    connect(ui->treeView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(modelDoubleClick(QModelIndex)));
}

BookMediaEditor::~BookMediaEditor()
{
    delete ui;
}

void BookMediaEditor::setBook(LibraryBook::Ptr book)
{
    if(m_book == book)
        return;

    qDeleteAll(m_resources);
    m_resources.clear();

    m_mediaEdited = false;
    m_book = book;

    loadImages();
}

void BookMediaEditor::saveChanges(BookEditor *bookEditor)
{
    QList<BookMediaResource*> addedMedia, removedMedia;

    foreach(BookMediaResource *media, m_resources) {
        if(media->op == BookMediaResource::Add) {
            addedMedia.append(media);
        } else if(media->op == BookMediaResource::Remove) {
            removedMedia.append(media);
        }
    }

    bookEditor->zipHelper()->transaction();

        foreach(BookMediaResource *media, removedMedia) {
            bookEditor->zipHelper()->remove(media->path);
            media->data.clear();
            m_resources.removeAll(media);

#ifdef DEV_BUILD
            qDebug() << "BookMediaEditor::saveChanges Remove image:"
                     << qPrintable(media->path);
#endif
            delete media;
        }

        foreach(BookMediaResource *media, addedMedia) {
            bookEditor->zipHelper()->add(media->path,
                                         media->data,
                                         ZipHelper::AppendFile);

            media->op = BookMediaResource::None;

#ifdef DEV_BUILD
            qDebug() << "BookMediaEditor::saveChanges Add image:"
                     << qPrintable(media->path);
#endif
        }

        bookEditor->zipHelper()->commit();

    m_mediaEdited = false;
}

BookMediaResource* BookMediaEditor::getMediaByPath(QString path)
{
    for(int i=0; i<m_resources.count(); i++) {
        BookMediaResource *media = m_resources.at(i);
        if(media->path.compare(path, Qt::CaseInsensitive) == 0
                && media->op != BookMediaResource::Remove) {
            return media;
        }
    }

    return 0;
}

BookMediaResource *BookMediaEditor::getMediaByFileName(QString fileName)
{
    for(int i=0; i<m_resources.count(); i++) {
        BookMediaResource *media = m_resources.at(i);
        if(media->fileName.compare(fileName, Qt::CaseInsensitive) == 0
                && media->op != BookMediaResource::Remove) {
            return media;
        }
    }

    return 0;
}

bool BookMediaEditor::mediaEdited() const
{
    return m_mediaEdited;
}

void BookMediaEditor::setupModel()
{
    ml_delete_check(m_imagesItem);
    ml_delete_check(m_model);

    m_model = new QStandardItemModel();
    m_imagesItem = new QStandardItem(tr("Images"));

    m_model->appendRow(m_imagesItem);

    ui->treeView->setModel(m_model);

    connect(ui->treeView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(modelSelectionChanged(QModelIndex,QModelIndex)));
}

void BookMediaEditor::openBook()
{
    ml_return_on_fail2(m_book, "BookMediaEditor::openBook book is null");

    ml_return_on_fail2(QFile::exists(m_book->path),
                       "BookMediaEditor::openBook file doesn't exists" << m_book->path);

    m_zip.setZipName(m_book->path);

    ml_return_on_fail2(m_zip.open(QuaZip::mdUnzip),
                       "BookMediaEditor::openBook Can't zip file" << m_book->path
                       << "Error" << m_zip.getZipError());
}

void BookMediaEditor::closeBook()
{
    if(m_zip.isOpen())
        m_zip.close();
}

void BookMediaEditor::loadImages()
{
    openBook();
    setupModel();

    QuaZipFileInfo info;
    QuaZipFile file(&m_zip);
    for(bool more=m_zip.goToFirstFile(); more; more=m_zip.goToNextFile()) {
        ml_return_on_fail2(m_zip.getCurrentFileInfo(&info),
                           "BookIndexerSimple::start getCurrentFileInfo Error" << m_zip.getZipError());

        QString fileName = info.name;
        if(fileName.startsWith(IMAGES_PREFIX)
                && fileName.compare(IMAGES_PREFIX, Qt::CaseInsensitive)!=0) {
            fileName = fileName.remove(0, QString(IMAGES_PREFIX).size());

        } else {
            continue;
        }

        if(!file.open(QIODevice::ReadOnly)) {
            qWarning("BookIndexerSimple::start zip error %d", m_zip.getZipError());
            continue;
        }

        QByteArray out;
        Utils::Files::copyData(file, out);

        BookMediaResource *resource = new BookMediaResource();
        resource->path = info.name;
        resource->fileName = fileName;
        resource->data = out;

        m_resources.append(resource);
        addResoourceToModel(resource);

        file.close();
    }

    ui->treeView->expandAll();

#ifdef DEV_BUILD
    qDebug("BookMediaEditor::loadImages %d files", m_resources.size());
#endif

    closeBook();
}

void BookMediaEditor::addImage(QString path)
{
    QString fileName = QFileInfo(path).fileName();
    ml_return_on_fail2(fileName.size(), "BookMediaEditor::addImage fileName is empty");

    while(getMediaByFileName(fileName)) {
        bool ok;
        QString text = QInputDialog::getText(this, tr("ملف موجود بنفس الاسم"),
                                             tr("اختر اسما اخر لهذا الملف:"), QLineEdit::Normal,
                                             fileName, &ok);
        if (ok && text.size())
            fileName = text;
        else
            return;
    }

    QFile file(path);
    ml_return_on_fail2(file.open(QIODevice::ReadOnly),
                       "BookMediaEditor::addImage Can't open" << path << "Error:" << file.errorString());

    QByteArray data;
    Utils::Files::copyData(file, data);

    BookMediaResource *resource = new BookMediaResource();
    resource->fileName = fileName;
    resource->path = IMAGES_PREFIX + fileName;
    resource->data = data;
    resource->op = BookMediaResource::Add;

    m_resources.append(resource);
    addResoourceToModel(resource);

    m_mediaEdited = true;
}

void BookMediaEditor::addResoourceToModel(BookMediaResource *resources)
{
    QList<QStandardItem*> rowItems;

    QStandardItem *nameItem = new QStandardItem(resources->fileName);
    nameItem->setToolTip(resources->path);
    nameItem->setData(resources->fileName, FileNameRole);
    nameItem->setData(resources->path, FilePathRole);
    nameItem->setData(resources->data, FileDataRole);
    rowItems << nameItem;
/*
    QPixmap pixmap;
    pixmap.loadFromData(resources->data, qPrintable(QFileInfo(resources->fileName).suffix()));

    if (pixmap.height() > 100 || pixmap.width() > 100) {
        pixmap = pixmap.scaled(QSize(100, 100), Qt::KeepAspectRatio);
    }

    QStandardItem *iconItem = new QStandardItem();
    iconItem->setIcon(QIcon(pixmap));
    iconItem->setEditable(false);
    rowItems << iconItem;
    //m_model->setColumnCount(2);
*/

    m_imagesItem->appendRow(rowItems);
}

void BookMediaEditor::imageGallery()
{
    HtmlHelper html;
    html.beginHead();
    html.setCharset();
    html.setTitle(m_book->title);

    html.beginHtmlTag("style");
    html.append("body {background-color: #aaa}");
    html.endHtmlTag();

    html.endHead();

    html.beginBody();

    foreach(BookMediaResource *media, m_resources) {
        if(media->op != BookMediaResource::Remove) {
            QString imageSrc = QString::fromLatin1(media->data.toBase64());

            html.beginSpan("", QString("style='cursor:pointer; display:block; float: left; margin:5px; padding: 5px; "
                                       "border:1px solid #ccc; text-align:center; background-color: #fff; "
                                       "width:200px; height:200px;' onclick='bookMediaEditor.insertImage(''%1'')'")
                           .arg(media->path));

            html.insertImage(QString("data:image/png;base64,%1").arg(imageSrc), "",
                             "style='display:block; border:1px solid #000; "
                             "max-width:180px;max-height:180px;"
                             "margin-left: auto; margin-right: auto; margin-bottom: 6px;'");

            html.beginSpan("", "style='margin-top:5px; font-size:0.6em;'");
            html.append(media->fileName);
            html.endSpan();

            html.endSpan();
        }
    }

    html.endAll();

    m_webView->setHtml(html.html());
}

void BookMediaEditor::addMedia()
{
    QString lastPath = Utils::Settings::get("SavedPath/BookMediaEditor").toString();

    QStringList files = QFileDialog::getOpenFileNames(this,
                                                      tr("اختر الكتب التي تريد استيرادها:"),
                                                      lastPath,
                                                      "Supported formats(*.png *.jpg *.jpeg *.gif *.bmp *.svg);;"
                                                      "All files(*)");
    if(files.size()) {
        foreach(QString file, files) {
            addImage(file);
        }

        Utils::Settings::set("SavedPath/BookMediaEditor",
                             QFileInfo(files.first()).absolutePath());
    }
}

void BookMediaEditor::removeMedia()
{
    QModelIndex index = Utils::Model::selectedIndex(ui->treeView);
    if(index.isValid()
            && index.data(FilePathRole).toString().size()) {
        if(QMessageBox::question(this,
                                 tr("حذف صورة"),
                                 tr("هل انت متأكد من انك تريد الصورة '%1'؟")
                                 .arg(index.data().toString()),
                                 QMessageBox::Yes|QMessageBox::No,
                                 QMessageBox::No)==QMessageBox::Yes) {
            QString path = index.data(FilePathRole).toString();

            for(int i=0; i<m_resources.count(); i++) {
                BookMediaResource *media = m_resources.at(i);
                if(media->path.compare(path, Qt::CaseInsensitive) == 0) {
                    media->op = BookMediaResource::Remove;

                    //QModelIndex sourceIndex = m_filter->filterModel()->mapToSource(index);
                    m_model->removeRow(index.row(), index.parent());

                    m_mediaEdited = true;
                }
            }
        }
    }
}

void BookMediaEditor::modelSelectionChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    if(current.data(FileDataRole).isNull()) {
        ui->labelDetails->clear();

        if(m_imagesItem && m_imagesItem->index() == current) {
            imageGallery();
        } else {
            m_webView->setHtml("");
        }
    } else {
        QByteArray imageData = current.data(FileDataRole).toByteArray();
        QString imageSrc = QString::fromLatin1(imageData.toBase64());

        const double ffsize = imageData.size() / 1024.0;
        const QString fsize = QLocale().toString(ffsize, 'f', 2);
        //const double ffmbsize = ffsize / 1024.0;
        //const QString fmbsize = QLocale().toString(ffmbsize, 'f', 2);

        QImage img = QImage::fromData(imageData, qPrintable(QFileInfo(current.data(FileNameRole).toString()).suffix()));
        QString details = QString("%1x%2px | %3 KB | %4").arg(img.width())
                .arg(img.height()).arg(fsize)
                .arg(current.data(FilePathRole).toString());

        QWebSettings::clearMemoryCaches();

        ui->labelDetails->setText(details);
        m_webView->setHtml(IMAGE_HTML_BASE_PREVIEW.arg(imageSrc));
    }
}

void BookMediaEditor::modelDoubleClick(const QModelIndex &index)
{
    QString path = index.data(FilePathRole).toString();
    if(path.size()) {
        emit insertImage(path);
    }
}
