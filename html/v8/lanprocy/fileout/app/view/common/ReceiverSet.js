Ext.define('Fileout.view.common.ReceiverSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.receiver',
  
  mark: 6,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');

    this.columns = [
      {header:'�ռ��˵�ַ',dataIndex:'name',flex:1}
    ];
     this.comboName='receivercombo';
    this.gridStore = Ext.create('Fileout.store.ReceiverGrid');
     this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_receiver",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_receiver', inputValue: '1'  },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_receiver', inputValue: '2', checked: true}
      ]
    });
    this.callParent(arguments);
  }
});