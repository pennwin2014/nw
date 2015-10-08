Ext.define('Fileout.view.common.OtherSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.othersoft',

  mark: 15,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='othersoftcombo';
    this.columns = [
      {header:'������',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_othersoft",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_othersoft', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_othersoft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});