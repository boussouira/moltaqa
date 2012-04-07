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
