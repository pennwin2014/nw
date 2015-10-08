Ext.define('Fileout.view.common.AddWin' ,{
  extend: 'Ext.window.Window',
  alias : 'widget.addwin',

	title: null,
	button1: null,
	modal: true,
	field1: null,
	field2: null,
	value1: null,
	value2: null,
	value3: null,
	recno: null,
	deal: null,

  initComponent: function() {
    this.items = [{
      xtype: 'form',
      items:[{
        xtype: 'textfield',
        //labelWidth: 70,
        margin: "5 5 5 5",
        fieldLabel: this.field1,
        name: 'field1',
        hidden: this.field1==null
      },{
        xtype: 'textfield',
        margin: "5 5 5 5",
        //labelWidth: 70,
        fieldLabel: this.field2,
        name: 'field2',
        hidden: this.field2==null
      }]
    }];
    
    this.buttons=[{
      text: this.button1,
      action: 'addvalue'
    },{
      text: 'È¡Ïû',
      scope: this,
      handler: this.close
    }];
    
    this.callParent(arguments);
  }
});