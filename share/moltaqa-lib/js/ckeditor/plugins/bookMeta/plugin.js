// Register a new CKEditor plugin.
CKEDITOR.plugins.add( 'bookMeta',
                     {
                         init: function( editor )
                         {
                             var iconspath = this.path + '/images/';

                             var addButtonCommand = function( buttonName, buttonLabel, commandName, styleDefiniton )
                             {
                                 var style = new CKEDITOR.style( styleDefiniton );

                                 editor.attachStyleStateChange( style, function( state )
                                     {
                                         if(!editor.readOnly)
                                             editor.getCommand( commandName ).setState( state );
                                     });

                                 editor.addCommand( commandName, new CKEDITOR.styleCommand( style ) );

                                 editor.ui.addButton( buttonName,
                                     {
                                         label : buttonLabel,
                                         command : commandName,
                                         icon: iconspath + commandName + '.png'
                                     });
                             };

                             var config = editor.config

                             addButtonCommand( 'Sanad', 'سند حديث', 'sanad', config.coreStyles_sanad );
                             addButtonCommand( 'Mateen', 'متن حديث', 'mateen', config.coreStyles_mateen );
                             addButtonCommand( 'Sheer', 'بيت شعر', 'sheer', config.coreStyles_sheer );

                             config.removeFormatTags = config.removeFormatTags + ',sanad,mateen,sheer';

                         },

                         requires : ['link']
                     } );

CKEDITOR.config.coreStyles_sanad = { element : 'sanad' };
CKEDITOR.config.coreStyles_mateen = { element : 'mateen' };
CKEDITOR.config.coreStyles_sheer = { element : 'sheer' };
