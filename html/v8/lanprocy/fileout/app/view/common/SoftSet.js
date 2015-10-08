Ext.define('Fileout.view.common.SoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.soft',

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.SoftCombo');
    this.columns = [
      {header:'������',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});