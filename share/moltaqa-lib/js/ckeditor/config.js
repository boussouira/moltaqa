/*
Copyright (c) 2003-2011, CKSource - Frederico Knabben. All rights reserved.
For licensing, see LICENSE.html or http://ckeditor.com/license
*/

CKEDITOR.editorConfig = function( config )
{
    config.toolbar = 'Custom';

    config.toolbar_Custom =
            [
                { name: 'document', items : [ 'Source' ] },
                { name: 'clipboard', items : [ 'Cut','Copy','Paste','PasteText','PasteFromWord','-','Undo','Redo' ] },
                { name: 'editing', items : [ 'Find','Replace','-','SelectAll' ] },
                { name: 'tools', items : [ 'ShowBlocks' ] },
                '/',
                { name: 'basicstyles', items : [ 'Bold','Italic','Underline','Strike','Subscript','Superscript','-','RemoveFormat' ] },
                { name: 'paragraph', items : [ 'NumberedList','BulletedList','-','Outdent','Indent','-','Blockquote','CreateDiv','-',
                        'JustifyRight','JustifyCenter','JustifyLeft','JustifyBlock','-','BidiRtl','BidiLtr' ] },
                { name: 'links', items : [ 'Link','Unlink','Anchor' ] },
                { name: 'insert', items : [ 'Table','HorizontalRule', 'SpecialChar' ] },
                '/',
                { name: 'styles', items : [ 'Styles','Format','Font','FontSize' ] },
                { name: 'colors', items : [ 'TextColor','BGColor' ] },
                '/',
                {name : 'Special', items: ['Abbr', 'RemoveAbbr', '-', 'BookLink', 'RemoveBookLink', '-', 'Sanad', 'Mateen', 'Sheer']},
            ];

    config.language = 'ar';
    config.font_names = 'Lotus Linotype/Lotus Linotype;Traditional arabic/Traditional arabic;' + config.font_names;

    config.extraPlugins = 'abbr,bookLink,bookMeta';
    config.removePlugins = 'elementspath';
    config.entities = false;
};
