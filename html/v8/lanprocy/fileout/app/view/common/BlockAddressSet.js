Ext.define('Fileout.view.common.BlockAddressSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.blockaddressadd',
  
  mark: 35,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='blockaddresscombo';
    this.columns = [
      {header:'禁止外发服务器IP',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
  
    
    this.callParent(arguments);
  }
});