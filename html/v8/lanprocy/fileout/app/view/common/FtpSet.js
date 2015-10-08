Ext.define('Fileout.view.common.FtpSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.ftpserver',
  
  mark: 11,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='ftpcombo';
    this.columns = [
      {header:'��������ַ',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.FtpGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_ftp",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_ftp', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_ftp', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});