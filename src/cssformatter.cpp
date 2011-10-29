#include "cssformatter.h"
#include "clutils.h"
#include <qregexp.h>

CssFormatter::CssFormatter() : numHighlights(0)
{
    numHighlights = 0;
    m_colorIndex = 0;

    m_colors << "#ffff63"
             << "#a5ffff"
             << "#ff9a9c"
             << "#9cff9c"
             << "#ef86fb"
             << "#ab9bff"
             << "#e7cda4"
             << "#c8dc79"
             << "#d4ff62";
}

CssFormatter::~CssFormatter()
{
}

TCHAR * CssFormatter::highlightTerm(const TCHAR *originalText, const lucene::search::highlight::TokenGroup *group)
{
        if (group->getTotalScore() <= 0) {
            return STRDUP_TtoT(originalText);
        }
        numHighlights++; //update stats used in assertions

        QString word = Utils::WCharToString(originalText);
        QString color;

        if(m_useColor) {
            QString cleanWord = Utils::WCharToString(originalText)
                    .remove(QRegExp("[\\x064B-\\x0653\\x0640]"))
                    .replace("[\\x0622\\x0623\\x0625]", "\\x0627");

            if(m_wordColor.contains(cleanWord)) {
                color = m_wordColor.value(cleanWord);
            } else {
                color = m_colors.value(m_colorIndex, m_colors.at(0));
                m_wordColor.insert(cleanWord, color);

                m_colorIndex++;
            }
        } else {
            color = m_colors.value(0);
        }

        QString formattedText = QString("<b style=\"background-color:%1\">%2</b>").arg(color).arg(word);
        return Utils::QStringToWChar(formattedText);
    }
