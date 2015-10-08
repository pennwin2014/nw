Ext.define('Fileout.view.common.ServerIpSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.serverip',
  
  mark: 31,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='serveripcombo';
    this.columns = [
      {header:'IP��ַ',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_serverip",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_serverip', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_serverip', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});