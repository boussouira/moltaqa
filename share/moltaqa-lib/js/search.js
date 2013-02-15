var clearBody = true;

function clear(selector)
{
    $(selector).html('');
}

function bookNameClick()
{
    $('.result .resultInfo .bookInfo').click(function() {
        var p = $(this).parent('.resultInfo');
        var bookID = p.attr('bookid');

        resultWidget.showBookMenu(bookID);
    });
}

function addResult(rid, str)
{
    var result = $(str);
    result.find('.resultText').click(function() {
        var bookID = $(this).attr('bookid');
        var resultID = $(this).attr('rid');

        resultWidget.openResult(resultID);

        var p = $(this).parent('.result');
        scroll(p.position().left, p.position().top);
    });

    $('.r_'+rid).removeClass('result_loading').replaceWith(result);
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

function fetechStarted(start, end)
{
    if(clearBody) {
        clear('#searchResult');
        clear('#pagination');
    }

    for(i=start; i<end; i++) {
        $('#searchResult').append($('<div>', {
                                        'text': 'جاري تحميل النتيجة...',
                                        'class': 'result_loading r_'+i
                                    }));
    }
}

function fetechFinnished()
{
    bookNameClick();
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
