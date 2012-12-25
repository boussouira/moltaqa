#include "bookutils.h"

namespace Utils {
namespace Book {

int getPageTitleID(QList<int> &titles, int pageID)
{
    if(!titles.contains(pageID)) {
        int title = 0;
        for(int i=0; i<titles.size(); i++) {
            title = titles.at(i);
            if(i == titles.size()-1)
                return titles.last();
            else if(title <= pageID && pageID < titles.at(i+1))
                return title;
            if(title > pageID)
                break;
        }

        if(titles.size())
            return titles.first();
    }

    return pageID;
}

}
}
