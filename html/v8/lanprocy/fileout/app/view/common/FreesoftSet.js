Ext.define('Fileout.view.common.FreesoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.freesoftadd',
  
  mark: 32,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='freecombo';
    this.columns = [
      {header:'软件进程名',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
  
    
    this.callParent(arguments);
  }
});