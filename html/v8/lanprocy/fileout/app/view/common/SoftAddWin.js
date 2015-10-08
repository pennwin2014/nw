Ext.define('Fileout.view.common.SoftAddWin' ,{
  extend: 'Ext.window.Window',
  alias : 'widget.softaddwin',

	title:  "����������",
	modal:  true,
	value1: null,
	value2: null,
	value3: null,
	recno: 0,

  initComponent: function() {
    var store = Ext.create('Fileout.store.SoftCombo');
//    alert(this.value1);
    store.load({params:{paramtype:30,keyname:this.value1}});
    this.items = [{
      xtype: 'form',
      items:[{
        xtype: 'combo',
  	    action: 'combo',
  	    fieldLabel: "������",
  	    labelWidth: 40,
  	    width: 180,
        store: store,
        queryMode: 'local',
        displayField: 'name',
        valueField: 'recno',
  	    triggerAction: 'all',
        margin: "5 5 5 5",
        name: 'field1',
        editable: false
      }]
    }];
    
    this.buttons=[{
      text: "����",
      action: 'addvalue'
    },{
      text: 'ȡ��',
      scope: this,
      handler: this.close
    }];
    
    this.callParent(arguments);
  }
});