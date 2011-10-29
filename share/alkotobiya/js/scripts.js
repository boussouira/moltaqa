var clearBody = true;

function resultEvents()
{
    $('.result .resultText').click(function() {
        var d = $(this).parent('.result');
        var p = d.find('.resultText');
        var bookID = p.attr('bookid');
        var resultID = p.attr('rid');

        resultWidget.openResult(resultID);
    });
}

function addResult(str)
{
    $('body').append($(str));
}

function searchStarted()
{
    $('body').html($('<p>', {text: "جاري البحث...", 'class': 'statusDiv'}));
}

function searchFinnished()
{
    $('body').html('');
    clearBody = false;
}

function fetechStarted()
{
    if(clearBody) {
        $('body').html('');
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