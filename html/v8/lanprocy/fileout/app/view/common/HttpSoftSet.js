Ext.define('Fileout.view.common.HttpSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.httpsoft',
  
  mark: 3,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.columns = [
      {header:'������',dataIndex:'name',flex:1}
    ];
    this.comboName='softcombo';
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_soft",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_soft', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_soft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});