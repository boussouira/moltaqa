#ifndef CTEXTCRUSOR_H
#define CTEXTCRUSOR_H

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QTextEdit>
#include <QMessageBox>
#include <QHash>

#include "constant.h"

class CTextCrusor : public QTextCursor
{
    public:
        CTextCrusor(QTextEdit *parent);
        void insertSoraName(QString SoraName);
        void insertBassemala();
        void insertAyaText(QString ayaText, int ayaNum, int soraNum);
        void setSoraAndAyaToHighLight(int pSoraNumber, int pAyaNumber);
        QTextCursor getCursorPosition(int pSoraNum, int pAyaNum);
        void clearAll();

   private:
        void setCurrentHighLightedAya(int pSora, int pAya);

   private:
        QTextEdit *m_parent;
        QHash<QString, int> m_ayatPostion;
        int m_soraPosition;
        int m_highLightSoraNumber;
        int m_hilightAyaNumber;
        int m_currentHighLightedAya;
        int m_currentHighLightedSora;

};

#endif // CTEXTCRUSOR_H
