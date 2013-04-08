#include "qurantextformat.h"
#include "utils.h"
#include "stringutils.h"
#include <qsettings.h>
#include <qfontmetrics.h>
#include <qbuffer.h>
#include <qimage.h>
#include <qpainter.h>

QString ayaImagePath;

QuranTextFormat::QuranTextFormat()
{
    m_cssID = "quran";

    m_styleDir = QDir(App::stylesDir());
    if(m_styleDir.cd(App::currentStyleName())) {
        m_drawAyaNumber = m_styleDir.exists("images/aya_border.png")
                && Utils::Settings::get("Style/drawAyatNumber", true).toBool();
    } else {
        m_drawAyaNumber = false;
    }

    if(m_drawAyaNumber)
        ayaImagePath = m_styleDir.absoluteFilePath("images/aya_border.png");

    m_useDataProtocol = false;
}

void QuranTextFormat::insertSoraName(const QString &pSoraName)
{
    QString soraName = tr("سورة %1").arg(pSoraName);
    m_htmlHelper.insertDiv(soraName, ".soraname");
}

void QuranTextFormat::insertBassemala()
{
    m_htmlHelper.insertDiv(tr("بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ"), ".bassemala");
}

void QuranTextFormat::insertAyaText(const QString &pAyaText, int pAyaNumber, int pSoraNumber)
{
    m_htmlHelper.beginSpan(".aya");
    m_htmlHelper.insertSpan(pAyaText, QString(".ayatxt|#s%1a%2").arg(pSoraNumber).arg(pAyaNumber));

    if(m_drawAyaNumber) {
        QString imageSrc = (m_useDataProtocol
                            ? getAyaNumberImageData(pAyaNumber)
                            : QString("book://quran/aya/%1.png").arg(pAyaNumber));

        m_htmlHelper.insertImage(imageSrc, ".ayanumber",
                                 QString("alt='(%1)' ayaimage='%2'")
                                 .arg(Utils::String::Arabic::arabicNumbers(pAyaNumber))
                                 .arg(pAyaNumber));

    } else {
        m_htmlHelper.beginSpan(".ayanumber", QString("ayaimage='%1'").arg(pAyaNumber));
        m_htmlHelper.appendText(_u(" ﴿%1﴾ ").arg(Utils::String::Arabic::arabicNumbers(pAyaNumber)));
        m_htmlHelper.endSpan();
    }

    m_htmlHelper.endSpan();
}

QByteArray QuranTextFormat::getAyaNumberImage(int ayaNumber, QString bgImage)
{
    if(bgImage.isEmpty()) {
        if(ayaImagePath.size())
            bgImage = ayaImagePath;
        else
            bgImage = ":/images/aya_background.png";
    }

    QString  ayaNumberStr = Utils::String::Arabic::arabicNumbers(ayaNumber);

    QSettings settings;
    settings.beginGroup("Style");
    QString fontString = settings.value("fontFamily", "Lotus Linotype").toString();
    int fontSize = settings.value("fontSize", 24).toInt();

    QFont font;
    font.fromString(fontString);
    font.setPointSize(fontSize);

    QFontMetrics info(font);
    QImage image(bgImage);

    int scaleWidth = info.width(Utils::String::Arabic::arabicNumbers(286))+4;
    int scaleHeight = fontSize+10;

    image = image.scaled(scaleWidth, scaleHeight,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation);

    double factor = (ayaNumberStr.size()==1 ? 2.2 : (ayaNumberStr.size()==2 ? 2.5 : 2.9));
    font.setPointSizeF(fontSize/factor);

    QFontMetrics fontMetrics(font);
    QPainter painter(&image);
    painter.setFont(font);
    painter.drawText(image.width()/2 - fontMetrics.width(ayaNumberStr)/2,
                     image.height()/2 + fontMetrics.descent()/2,
                     ayaNumberStr);

    QBuffer buffer;
    buffer.open(QIODevice::ReadWrite);
    image.save(&buffer, "png");

    buffer.seek(0);

    return buffer.readAll();
}

void QuranTextFormat::setDrawAyaNumber(bool drawAyaNumber)
{
    m_drawAyaNumber = drawAyaNumber;
}

void QuranTextFormat::setUseDataProtocol(bool useDataProtocol)
{
    m_useDataProtocol = useDataProtocol;
}

QString QuranTextFormat::getAyaNumberImageData(int ayaNumber)
{
    QByteArray imageData = getAyaNumberImage(ayaNumber, m_styleDir.absoluteFilePath("images/aya_border.png"));
    QString data("data:image/png;base64,");
    data.append(imageData.toBase64());

    return data;
}
