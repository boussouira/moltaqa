var clearBody = true;

function clear(selector)
{
    $(selector).html('');
}

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
    resultEvents();
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

function pageTextChanged()
{
    $('.breadcrumbs').html(bookWidget.getBreadcrumbs());
    webView.pageTextChanged();

    toggleQuran();
    indexReading();
    bookLink();
    setupToolTip();
}

function setPageText(text, page, part)
{
    $('#pageText').fadeOut('fast', function() {
                               $(this).html(text);
                               $('#pageHeader > #partInfo > .partNum').text(part);
                               $('#pageFooter > .page').text(page);
                               $(this).fadeIn('fast', function() {
                                                  pageTextChanged();
                                              });
                           });
}

$('#simpleBook #pageHeader .bookName').click(function(){
    bookWidget.showIndex();
});

function indexReading()
{
    $('ul.bookIndex > li, .breadcrumbs > span').click(function(){
        tid = $(this).attr('tid');
        //alert("ID: " + tid);
        bookWidget.showIndex(tid);
    });
}

function setEditorText(text)
{
    editor.setData(text);
}

function getEditorText()
{
    return editor.getData();
}

function toggleQuran()
{
    $('.toggale_quran > img').click(function() {
        if($('.quran_text').css('display')!="none") {
            // Hide the quran text
            $(this).attr('src', 'qrc:/images/add.png');
            $('.quran_text').slideUp('slow');
        } else {
            $(this).attr('src', 'qrc:/images/remove.png');
            $('.quran_text').slideDown('slow');
        }
    });
}

function toggleShorooh()
{
    $('#shorooh span.info').click(function() {
        element = $('#shorooh .shoroohBooks');
        if(element.css('display')!="none") {
            element.slideUp('slow');// Hide it
        } else {
            element.slideDown('slow');
        }
    });
}

function setShorooh(shorooh)
{
    infoSpin = $('#shorooh span.info');
    shoroohCount = 0;

    if(shorooh) {
        $('#shorooh .shoroohBooks').html('');

        for(i in shorooh) {
            $('#shorooh .shoroohBooks').append($('<p>',
                                                 {'text': shorooh[i].bookName,
                                                  'book': shorooh[i].bookID,
                                                  'page': shorooh[i].pageID}));
            ++shoroohCount;
        }

        $('#shorooh .shoroohBooks p').click(function() {
                                                book = $(this).attr('book');;
                                                page = $(this).attr('page');;

                                                booksViewer.openBook(book, page);
                                            });

        infoSpin.removeClass('disabled');
        $('#shorooh').show();
    } else {
        if(!infoSpin.hasClass('disabled'))
            infoSpin.addClass('disabled');
    }

    infoSpin.text(infoSpin.text().replace(new RegExp('([0-9]+)'), shoroohCount));
}

function bookLink()
{
    $('a[book][page]').click(function() {
        booksViewer.openBook($(this).attr('book'),
                             $(this).attr('page'));
        return false;
    });
}

function setupToolTip() {
    $('abbr').tooltip({
        track: true,
        showURL: false,
        fixPNG: true,
        opacity: 0.95,
        left: 0,
        positionLeft: true
    });
}
