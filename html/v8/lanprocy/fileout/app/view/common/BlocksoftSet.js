Ext.define('Fileout.view.common.BlocksoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.blocksoftadd',
  
  mark: 34,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='blockcombo';
    this.columns = [
      {header:'软件进程名',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
  
    
    this.callParent(arguments);
  }
});