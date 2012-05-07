function pageTextChanged()
{
    $('.breadcrumbs').html(bookWidget.getBreadcrumbs());
    webView.pageTextChanged();

    toggleQuran();
    indexReading();
    moltaqaLink();
    setupToolTip();
}

function setPageText(text, page, part)
{
    $('#pageText').fadeOut('fast', function() {
                               $(this).html(text);
                               $('#pageHeader #currentPage .page .val').text(page);
                               $('#pageHeader #currentPage .part .val').text(part);
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
    $('.bookIndex > a, .breadcrumbs > span').click(function(){
        bookWidget.showIndex($(this).attr('tid'));
    });

    $('.bookIndex > img').click(function(){
        bookWidget.openPage($(this).attr('tid'));
    });
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
