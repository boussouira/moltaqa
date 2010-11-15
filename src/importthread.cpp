#include "importthread.h"
#include "importmodel.h"
#include "booksindexdb.h"

#include <qdatetime.h>
#include <qgridlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qsignalmapper.h>
#include <qdebug.h>

ImportThread::ImportThread(QObject *parent) : QThread(parent)
{
    m_importedBooks = 0;
}

void ImportThread::run()
{
    QTime time;
    time.start();

    QList<ImportModelNode *> nodesList = m_model->nodeFromIndex(QModelIndex())->childs();

    for(int i=0;i<nodesList.count();i++) {
        ImportModelNode *node = nodesList.at(i);
        int lastInsert = m_indexDB->addBook(node);

        if(lastInsert != -1) {
            m_importedBooks++;
            m_booksList.insert(lastInsert, node->bookName());
        } else {
            qWarning() << "Error:" << node->bookName();
        }

        emit setProgress(i+1);
    }

    m_importTime = time.elapsed();
}
