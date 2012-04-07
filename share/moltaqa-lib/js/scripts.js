function bookLink()
{
    $('a[book][page]').click(function() {
        booksViewer.openBook($(this).attr('book'),
                             $(this).attr('page'));
        return false;
    });
}

function setupToolTip() {
    $('*[title]').tooltip({
        track: true,
        showURL: false,
        fixPNG: true,
        opacity: 0.95,
        left: 0,
        positionLeft: true
    });
}

$('a').each(function(index) {
    link = $(this).attr('href');

    scheme = link.substring(0, link.indexOf(':'));

    if(scheme == 'moltaqa') {
        $(this).addClass('ext');
        $(this).click(function() {
            webView.openMoltaqaLink($(this).attr('href'));
        });
    }
});

bookLink();
setupToolTip();
