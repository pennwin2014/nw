Ext.define('Fileout.view.common.SoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.soft',

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.SoftCombo');
    this.columns = [
      {header:'软件名',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode",
      items: [
        { boxLabel: '启用禁止模式', name: 'mode', inputValue: '1', checked: true },
        { boxLabel: '启用只允许模式', name: 'mode', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});