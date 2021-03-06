Ext.define('Fileout.view.common.FtpSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.ftpserver',
  
  mark: 11,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='ftpcombo';
    this.columns = [
      {header:'服务器地址',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.FtpGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_ftp",
      items: [
        { boxLabel: '启用禁止模式', name: 'mode_ftp', inputValue: '1', checked: true },
        { boxLabel: '启用只允许模式', name: 'mode_ftp', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});