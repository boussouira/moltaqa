function setEditorText(text)
{
    editor.setData(text);
}

function getEditorText()
{
    return editor.getData();
}

function makeSelectionTitle(text, level, tagID)
{
    var sel = editor.getSelection();
    var element = sel.getStartElement();

    if(element) {
        if(Math.abs(element.getText().length - text.length) < 10) {
            element.renameNode('h' + level);
            element.setAttribute('id', tagID);
        } else {
            var n = editor.document.createElement('h' + level);
            n.setAttribute('id', tagID);
            n.setText(text);
            n.insertBefore(element);
        }
    }
}

function inserImage(imageSrc)
{
    var element = editor.document.createElement('img');
    element.setAttribute('src', imageSrc);

    editor.insertElement(element);
}
