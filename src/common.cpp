#include "common.h"

int rand_int(int smin, int smax)
{
    return (smin + (qrand() % (smax-smin+1)));
}

QString genBookName(QString path)
{
    QString fileName("book_");
    char c[] = "abcdefABCDEF1234567";
    int len = strlen(c);

    for(int i=0; i<6; i++) {
        fileName.append(c[rand_int(0, len-1)]);
    }

    while(true) {
        if(QFile::exists(path+"/"+fileName+".alb")){
            fileName.append(c[rand_int(0, len-1)]);
        } else {
            break;
        }
    }

    return fileName+".alb";
}
