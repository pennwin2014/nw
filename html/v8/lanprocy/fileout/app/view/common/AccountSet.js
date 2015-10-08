Ext.define('Fileout.view.common.AccountSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.account',
  
  mark: 13,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='accountcombo';
    this.columns = [
      {header:'�����˺�',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_account",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_account', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_account', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});