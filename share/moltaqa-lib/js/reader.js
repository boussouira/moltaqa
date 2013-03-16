function pageTextChanged()
{
    $('.breadcrumbs').html(bookWidget.getBreadcrumbs());
    webView.pageTextChanged();

    toggleQuran();
    indexReading();
    setupToolTip();
}

function setPageText(text, page, part)
{
    $('#pageText').fadeOut('fast', function() {
                               $(this).html(text);

                               if(page)
                                   $('#pageHeader #currentPage .page .val').text(page);

                               if(part)
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
        bookWidget.openPageID($(this).attr('tid'));
    });
}

function toggleQuran()
{
    $('.toggale_quran').click(function() {
        if($('.quran_text').css('display')!="none") {
            // Hide the quran text
            $('.toggale_quran > img').attr('src', 'qrc:/images/add2.png');
            $('.quran_text').slideUp('slow');
        } else {
            $('.toggale_quran > img').attr('src', 'qrc:/images/delete2.png');
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
            var book = shorooh[i];
            var pageLink = 'moltaqa://?c=open&t=book&id=' + book.uuid + '&page=' + book.page;
            if(i > 0)
                $('#shorooh .shoroohBooks').append($('<br/>'));

            $('#shorooh .shoroohBooks').append($('<a>',
                                                 {'text': book.title,
                                                  'href': pageLink}));
            ++shoroohCount;
        }

        infoSpin.removeClass('disabled');
        $('#shorooh').show();
    } else {
        if(!infoSpin.hasClass('disabled'))
            infoSpin.addClass('disabled');
    }

    infoSpin.text(infoSpin.text().replace(new RegExp('([0-9]+)'), shoroohCount));
}
