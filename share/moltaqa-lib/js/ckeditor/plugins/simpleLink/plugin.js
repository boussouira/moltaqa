// Register a new CKEditor plugin.
CKEDITOR.plugins.add( 'simpleLink',
{
	// The plugin initialization logic goes inside this method.
	init: function( editor )
	{
        var iconPath = this.path + 'images/icon.png';
        
		// Create an editor command that stores the dialog initialization command.
		editor.addCommand( 'simpleLinkDialog', new CKEDITOR.dialogCommand( 'simpleLinkDialog' ) );
 
		// Create a toolbar button that executes the plugin command defined above.
		editor.ui.addButton( 'SimpleLink',
		{
			// Toolbar button tooltip.
			label: 'اضافة رابط الى كتاب',
			// Reference to the plugin command name.
			command: 'simpleLinkDialog',
			// Button's icon file path.
			icon: iconPath
		} );
 
		// Add a new dialog window definition containing all UI elements and listeners.
		CKEDITOR.dialog.add( 'simpleLinkDialog', function( editor )
		{
			return {
				// Basic properties of the dialog window: title, minimum size.
				title : 'رابط الى كتاب',
				minWidth : 400,
				minHeight : 200,
				// Dialog window contents.
				contents :
				[
					{
						// Definition of the Settings dialog window tab (page) with its id, label and contents.
						id : 'general',
						label : 'Settings',
						elements :
						[
							// Dialog window UI element: HTML code field.
							{
								type : 'html',
								// HTML code to be shown inside the field.
								html : 'هذه النافذة تمكنك من انشاء رابط يقوم بنقلك الى صفحة في كتاب اخر'
							},
							// Dialog window UI element: a textarea field for the link text.
							{
								type : 'text',
								id : 'contents',
								// Text that labels the field.
								label : 'النص',
								// Validation checking whether the field is not empty.
								validate : CKEDITOR.dialog.validate.notEmpty( 'حقل النص يجب ان لا يكون فارغا.' ),
								// This field is required.
								required : true,
                                setup : function( element )
                                {
                                    if(this.insertMode || !editor.getSelection().getSelectedText().length)
                                        this.setValue( element.getText() );
                                    else
                                        this.setValue(editor.getSelection().getSelectedText());
                                },
                                commit : function( element )
                                {
                                    element.setText( this.getValue() );
                                },
                                'onKeyPress': function(e) {
                                    //alert('test');
                                }
							},
							// Dialog window UI element: a text input field for the link URL.
							{
								type : 'text',
								id : 'url',
								label : 'معرف الصفحة',
								validate :CKEDITOR.dialog.validate.regex(/^\d+:\d+$/, 'معرف الصفحة غير صحيح' ),
								required : true,
								setup : function( element )
                                {
                                    if(element.getAttribute( 'book' ) && element.getAttribute( 'page' ))
                                        this.setValue( element.getAttribute( 'book' ) + ':' + element.getAttribute( 'page' ));
                                },
                                commit : function( element )
                                {
                                    var bookID = this.getValue().split(':')[0];
                                    var pageID = this.getValue().split(':')[1];

                                    element.setAttribute( 'href', '#' );
                                    element.setAttribute('book', bookID);
                                    element.setAttribute('page', pageID);
                                }
							},
						]
					}
				],
				onShow : function() 
                {
                    var sel = editor.getSelection(),
                        element = sel.getStartElement();
                    if ( element )
                        element = element.getAscendant( 'a', true );
 
                    if ( !element 
                        || element.getName() != 'a' 
                        || !element.getAttribute( 'book' ) 
                        || !element.getAttribute( 'page' )
                        || element.data( 'cke-realelement' ) )
                    {
                        element = editor.document.createElement( 'a' );
                        this.insertMode = true;
                    }
                    else
                        this.insertMode = false;
 
                    this.element = element;
 
                    this.setupContent( this.element );
                },
                onOk : function()
                {
                    var dialog = this;
                    var link = this.element;
 
                    if ( this.insertMode )
                        editor.insertElement( link );
                    this.commitContent( link );
                },
			};
		} );
	}
} );