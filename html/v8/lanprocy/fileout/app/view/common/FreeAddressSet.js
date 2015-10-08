Ext.define('Fileout.view.common.FreeAddressSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.freeaddressadd',
  
  mark: 33,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='freeaddresscombo';
    this.columns = [
      {header:'放行服务器IP',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
  
    
    this.callParent(arguments);
  }
});