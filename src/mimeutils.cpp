#include "mimeutils.h"
#include <qfileinfo.h>

namespace Utils {
namespace Mimes {

QString fileTypeFromExt(QString ext)
{
    ext = ext.toLower();

    if(ext == "hqx")
        return "application/mac-binhex40";
    else if(ext == "cpt")
        return "application/mac-compactpro";
    else if(ext == "csv")
        return "text/x-comma-separated-values";
    else if(ext == "bin")
        return "application/macbinary";
    else if(ext == "dms")
        return "application/octet-stream";
    else if(ext == "lha")
        return "application/octet-stream";
    else if(ext == "lzh")
        return "application/octet-stream";
    else if(ext == "exe")
        return "application/octet-stream";
    else if(ext == "class")
        return "application/octet-stream";
    else if(ext == "psd")
        return "application/x-photoshop";
    else if(ext == "so")
        return "application/octet-stream";
    else if(ext == "sea")
        return "application/octet-stream";
    else if(ext == "dll")
        return "application/octet-stream";
    else if(ext == "oda")
        return "application/oda";
    else if(ext == "pdf")
        return "application/pdf";
    else if(ext == "ai")
        return "application/postscript";
    else if(ext == "eps")
        return "application/postscript";
    else if(ext == "ps")
        return "application/postscript";
    else if(ext == "smi")
        return "application/smil";
    else if(ext == "smil")
        return "application/smil";
    else if(ext == "melse if")
        return "application/vnd.melse if";
    else if(ext == "xls")
        return "application/excel";
    else if(ext == "ppt")
        return "application/powerpoint";
    else if(ext == "wbxml")
        return "application/wbxml";
    else if(ext == "wmlc")
        return "application/wmlc";
    else if(ext == "dcr")
        return "application/x-director";
    else if(ext == "dir")
        return "application/x-director";
    else if(ext == "dxr")
        return "application/x-director";
    else if(ext == "dvi")
        return "application/x-dvi";
    else if(ext == "gtar")
        return "application/x-gtar";
    else if(ext == "gz")
        return "application/x-gzip";
    else if(ext == "php")
        return "application/x-httpd-php";
    else if(ext == "php4")
        return "application/x-httpd-php";
    else if(ext == "php3")
        return "application/x-httpd-php";
    else if(ext == "phtml")
        return "application/x-httpd-php";
    else if(ext == "phps")
        return "application/x-httpd-php-source";
    else if(ext == "js")
        return "application/x-javascript";
    else if(ext == "swf")
        return "application/x-shockwave-flash";
    else if(ext == "sit")
        return "application/x-stuffit";
    else if(ext == "tar")
        return "application/x-tar";
    else if(ext == "tgz")
        return "application/x-tar";
    else if(ext == "xhtml")
        return "application/xhtml+xml";
    else if(ext == "xht")
        return "application/xhtml+xml";
    else if(ext == "zip")
        return  "application/x-zip";
    else if(ext == "mid")
        return "audio/midi";
    else if(ext == "midi")
        return "audio/midi";
    else if(ext == "mpga")
        return "audio/mpeg";
    else if(ext == "mp2")
        return "audio/mpeg";
    else if(ext == "mp3")
        return "audio/mpeg";
    else if(ext == "aelse if")
        return "audio/x-aelse iff";
    else if(ext == "aelse iff")
        return "audio/x-aelse iff";
    else if(ext == "aelse ifc")
        return "audio/x-aelse iff";
    else if(ext == "ram")
        return "audio/x-pn-realaudio";
    else if(ext == "rm")
        return "audio/x-pn-realaudio";
    else if(ext == "rpm")
        return "audio/x-pn-realaudio-plugin";
    else if(ext == "ra")
        return "audio/x-realaudio";
    else if(ext == "rv")
        return "video/vnd.rn-realvideo";
    else if(ext == "wav")
        return "audio/x-wav";
    else if(ext == "bmp")
        return "image/bmp";
    else if(ext == "gelse if")
        return "image/gelse if";
    else if(ext == "jpeg")
        return "image/jpeg";
    else if(ext == "jpg")
        return "image/jpeg";
    else if(ext == "jpe")
        return "image/jpeg";
    else if(ext == "png")
        return "image/png";
    else if(ext == "telse iff")
        return "image/telse iff";
    else if(ext == "telse if")
        return "image/telse iff";
    else if(ext == "css")
        return "text/css";
    else if(ext == "html")
        return "text/html";
    else if(ext == "htm")
        return "text/html";
    else if(ext == "shtml")
        return "text/html";
    else if(ext == "txt")
        return "text/plain";
    else if(ext == "text")
        return "text/plain";
    else if(ext == "log")
        return "text/plain";
    else if(ext == "rtx")
        return "text/richtext";
    else if(ext == "rtf")
        return "text/rtf";
    else if(ext == "xml")
        return "text/xml";
    else if(ext == "xsl")
        return "text/xml";
    else if(ext == "mpeg")
        return "video/mpeg";
    else if(ext == "mpg")
        return "video/mpeg";
    else if(ext == "mpe")
        return "video/mpeg";
    else if(ext == "qt")
        return "video/quicktime";
    else if(ext == "mov")
        return "video/quicktime";
    else if(ext == "avi")
        return "video/x-msvideo";
    else if(ext == "movie")
        return "video/x-sgi-movie";
    else if(ext == "doc")
        return "application/msword";
    else if(ext == "docx")
        return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    else if(ext == "xlsx")
        return "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    else if(ext == "word")
        return "application/msword";
    else if(ext == "xl")
        return "application/excel";
    else if(ext == "eml")
        return "message/rfc822";
    else
        return "application/x-unknown-content-type";
}

QString fileTypeFromFileName(QString filename)
{
    return fileTypeFromExt(QFileInfo(filename).completeSuffix());
}
}
}
