var clearBody = true;

function resultEvents()
{
    $('.result .resultText').click(function() {
        var d = $(this).parent('.result');
        var p = d.find('.resultText');
        var bookID = p.attr('bookid');
        var resultID = p.attr('rid');

        resultWidget.openResult(resultID);

        scroll(d.position().left, d.position().top);
    });
}

function addResult(str)
{
    $('#searchResult').append($(str));
}

function searchStarted()
{
    $('#searchResult').html($('<p>', {text: "جاري البحث...", 'class': 'statusDiv'}));
}

function searchFinnished()
{
    $('#searchResult').html('');
    $('#pagination').html('');
    clearBody = false;
}

function fetechStarted()
{
    if(clearBody) {
        $('#searchResult').html('');
        $('#pagination').html('');
    }
}

function fetechFinnished()
{
    resultEvents();
    clearBody = true;
}

function searchInfo(searchTime, searchCount)
{
    var sec = '' + searchTime / 1000;
    if(sec.indexOf('.') != -1) {
        sec = sec.substr(0, sec.indexOf('.')+5);
    }

    $.Growl.show({
        'title' : "انتهى البحث",
        'message': "تم البحث خلال " + sec + " ثانية" +
                    "<br>" + "عدد النتائج: " + searchCount,
        'timeout':3000
    });
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
