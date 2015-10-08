Ext.define('Fileout.view.common.ChatSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.chatsoft',
  
  mark: 12,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='chatsoftcombo';
    this.columns = [
      {header:'�����',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_chatsoft",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_chatsoft', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_chatsoft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});