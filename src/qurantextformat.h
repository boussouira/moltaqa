#ifndef KTEXT_H
#define KTEXT_H

#include <QtCore>

/**
  @brief This class handle the formatting of the Quran pages.
  */
class QuranTextFormat
{
protected:
    QuranTextFormat();
    /**
      @brief Insert and format a SORA name.
      @param pSoraName      The SORA name.
      */
    void insertSoraName(QString pSoraName);

    /// @brief Insert a BASSEMALA.
    void insertBassemala();

    /**
      @brief Insert a formatted AYA text.
      @param pAyaText       The AYA text.
      @param pAyaNumber     AYA number.
      @param pSoraNumber    The SORA number
      */
    void insertAyaText(QString pAyaText, int pAyaNumber, int pSoraNumber);

    /**
      @brief Get the formatted text.
      @return The formatted text.
      */
    QString text();

    /**
      @brief Clear the current formatted text.
      */
    void clearQuranText();

private:
    QString m_text;

};

#endif // KTEXT_H
