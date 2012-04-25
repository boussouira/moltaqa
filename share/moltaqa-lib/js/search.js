var clearBody = true;

function clear(selector)
{
    $(selector).html('');
}

function addResult(str)
{
    var result = $(str);
    result.find('.resultText').click(function() {
        var bookID = $(this).attr('bookid');
        var resultID = $(this).attr('rid');

        resultWidget.openResult(resultID);

        var p = $(this).parent('.result');
        scroll(p.position().left, p.position().top);
    });

    $('#searchResult').append(result);
}

function searchStarted()
{
    $('#searchResult').html($('<div>', {text: "جاري البحث...", 'class': 'info'}));
    clear('#pagination');
}

function searchFinnished()
{
    clear('#searchResult');
    clear('#pagination');

    clearBody = false;
}

function fetechStarted()
{
    if(clearBody) {
        clear('#searchResult');
        clear('#pagination');
    }
}

function fetechFinnished()
{
    clearBody = true;
}

function showError(title, desc)
{
    errorDiv = $('<div>', {'class': 'error'});
    errorDiv.append($('<p>', {'text': title}));
    errorDiv.append($('<p>', {'text': desc}));

    $('#searchResult').html(errorDiv);
    clear('#pagination');
}

function searchInfo(searchTime, searchCount)
{
    if(searchCount == 0) {
        showError('لم يتم العثور على ما يطابق بحثك');
    } else {
        var sec = '' + searchTime / 1000;
        if(sec.indexOf('.') != -1) {
            sec = sec.substr(0, sec.indexOf('.')+5);
        }

        $.Growl.show({
                         'title' : "انتهى البحث",
                         'message': "تم البحث خلال " + sec + " ثانية" +
                                    "<br>" + "عدد النتائج: " + searchCount,
                         'timeout': 5000
                     });
    }
}

function handlePaginationClick(new_page_index, pagination_container)
{
    resultWidget.goToPage(new_page_index);
    return true;
}

function setPagination(currentPage, resultCount, resultPeerPage)
{
    $("#pagination").pagination(resultCount, {
                                    items_per_page:resultPeerPage,
                                    current_page: currentPage,
                                    num_edge_entries: 2,
                                    load_first_page: false,
                                    next_text: '&gt;',
                                    prev_text: '&lt;',
                                    callback:handlePaginationClick
                                });
}
