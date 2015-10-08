Ext.define('Fileout.view.common.SmtpSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.smtpadd',
  
  mark: 4,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='mailcombo';
    this.columns = [
      {header:'�ʼ�������ַ',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SmtpGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_smtp",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_smtp', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_smtp', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});