Ext.define('Fileout.view.common.FtpSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.ftpsoft',

  mark: 10,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='ftpsoftcombo';
    this.columns = [
      {header:'������',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_ftpsoft",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_ftpsoft', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_ftpsoft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});