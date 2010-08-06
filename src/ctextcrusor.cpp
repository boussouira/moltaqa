#include "ctextcrusor.h"

CTextCrusor::CTextCrusor(QTextEdit *parent):  QTextCursor(parent->textCursor()), m_parent(parent)
{

}


void CTextCrusor::insertSoraName(QString SoraName)
{
    QTextBlockFormat SoraNameFormat;
    QTextBlockFormat NormalTextFormat;
    ////////////////////
    QConicalGradient coneGradient(0, 0, 90.0);
    coneGradient.setColorAt(0.0, Qt::darkRed);
    coneGradient.setColorAt(1.0, Qt::white);
    coneGradient.setAngle(0);
    ////////////////////
    SoraNameFormat.setBackground(Qt::lightGray);
    SoraNameFormat.setBottomMargin(5);
    SoraNameFormat.setTopMargin(5);
    SoraNameFormat.setLeftMargin(5);
    SoraNameFormat.setRightMargin(5);
    SoraNameFormat.setAlignment(Qt::AlignCenter);
    SoraNameFormat.setBackground(QBrush(coneGradient));

   if(!this->atStart())
        this->insertBlock();

    this->setBlockFormat(SoraNameFormat);
    this->insertText(QString("%1: %2").arg(SORAT).arg(SoraName));
    this->insertBlock();
    this->setBlockFormat(NormalTextFormat);

}

void CTextCrusor::insertBassemala()
{
    QTextBlockFormat BassemalaBlockFormat;
    BassemalaBlockFormat.setAlignment(Qt::AlignCenter);
    this->setBlockFormat(BassemalaBlockFormat);
    this->insertText(BASMALA);
    this->insertBlock();
}

void CTextCrusor::insertAyaText(QString ayaText, int ayaNum, int soraNum)
{

    QTextCharFormat HighLightAyaFormat;
    m_ayatPostion.insert(QString("start-%1-%2").arg(soraNum).arg(ayaNum), this->position());
    if(m_highLightSoraNumber == soraNum and  m_hilightAyaNumber == ayaNum)
    {
        HighLightAyaFormat.setForeground(QBrush(QColor(190,0,0,255)));
        this->setCurrentHighLightedAya(soraNum, ayaNum);
    }
    HighLightAyaFormat.setProperty(QTextFormat::BlockLeftMargin, QVariant(10));
    this->setCharFormat(HighLightAyaFormat);
    this->insertText(QString("%1 (%2) ").arg(ayaText).arg(QString::number(ayaNum)));
    m_ayatPostion.insert(QString("end-%1-%2").arg(soraNum).arg(ayaNum), this->position());

}

QTextCursor CTextCrusor::getCursorPosition(int pSoraNum, int pAyaNum)
{
//    this->movePosition(QTextCursor::Start);
    QTextCharFormat HighLightAyaFormat;
    QTextCharFormat NormalTextFormat;

    int newStartPos = m_ayatPostion.value(QString("start-%1-%2").arg(QString::number(pSoraNum)).arg(QString::number(pAyaNum)));
    int newEndPos = m_ayatPostion.value(QString("end-%1-%2").arg(QString::number(pSoraNum)).arg(QString::number(pAyaNum)));
    int oldStartPos = m_ayatPostion.value(QString("start-%1-%2").arg(QString::number(m_currentHighLightedSora)).arg(QString::number(m_currentHighLightedAya)));
    int oldEndPos = m_ayatPostion.value(QString("end-%1-%2").arg(QString::number(m_currentHighLightedSora)).arg(QString::number(m_currentHighLightedAya)));
    HighLightAyaFormat.setForeground(QBrush(QColor(190, 0, 0, 255)));
    NormalTextFormat.setForeground(QBrush(QColor(0, 0, 0, 255)));

    // Unhighlight the current aya
    this->setPosition(oldStartPos);
    this->setPosition(oldEndPos, QTextCursor::KeepAnchor);
    this->mergeCharFormat(NormalTextFormat);
    this->setPosition(oldStartPos);

    // hightlight the new one
    this->setPosition(newStartPos);
    this->setPosition(newEndPos, QTextCursor::KeepAnchor);
    this->mergeCharFormat(HighLightAyaFormat);
    this->setPosition(newEndPos);

    this->setCurrentHighLightedAya(pSoraNum, pAyaNum);

    return (*this);

}

void CTextCrusor::setSoraAndAyaToHighLight(int pSoraNumber, int pAyaNumber)
{
    m_highLightSoraNumber = pSoraNumber;
    m_hilightAyaNumber = pAyaNumber;
}

void CTextCrusor::setCurrentHighLightedAya(int pSora, int pAya)
{
    m_currentHighLightedSora = pSora;
    m_currentHighLightedAya  = pAya;
}

void CTextCrusor::clearAll()
{
    m_parent->clear();
    m_ayatPostion.clear();

}
