#include "cssformatter.h"
#include "clutils.h"

#include <qregexp.h>

CssFormatter::CssFormatter()
{
    numHighlights = 0;
    m_useColor = true;

    m_numColor = 8;
}

CssFormatter::~CssFormatter()
{
}

TCHAR *CssFormatter::highlightTerm(const TCHAR *originalText, const lucene::search::highlight::TokenGroup *group)
{
        if (group->getTotalScore() <= 0) {
            return STRDUP_TtoT(originalText);
        }
        numHighlights++; //update stats used in assertions

        QString word = Utils::CLucene::WCharToString(originalText);
        QString color;

        if(m_useColor && m_words.size() <= m_numColor) {
            QString cleanWord = Utils::CLucene::WCharToString(originalText)
                    .remove(QRegExp("[\\x064B-\\x0653\\x0640]"))
                    .replace("[\\x0622\\x0623\\x0625]", "\\x0627");

            int wordIndex = m_words.indexOf(cleanWord);
            if(wordIndex != -1)  {
                color = QString("resultHL_%1").arg(wordIndex);
            } else {
                m_words.append(cleanWord);
                color = QString("resultHL_%1").arg(m_words.size()-1);
            }
        } else {
            color = "resultHL_default";
        }

        QString formattedText = QString("<span class=\"%1 resultHL\">%2</span>").arg(color).arg(word);
        return Utils::CLucene::QStringToWChar(formattedText);
    }
