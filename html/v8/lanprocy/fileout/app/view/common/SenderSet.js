Ext.define('Fileout.view.common.SenderSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.sender',
  
  mark: 5,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='sendercombo';
    this.columns = [
      {header:'发件人地址',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SenderGrid');
      this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_sender",
      items: [
        { boxLabel: '启用禁止模式', name: 'mode_sender', inputValue: '1'  },
        { boxLabel: '启用只允许模式', name: 'mode_sender', inputValue: '2', checked: true}
      ]
    });
    this.callParent(arguments);
  }
});