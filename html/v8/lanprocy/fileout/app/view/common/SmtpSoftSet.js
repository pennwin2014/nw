Ext.define('Fileout.view.common.SmtpSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.smtpsoft',
  
  mark: 8,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='softcombo';
    this.columns = [
      {header:'�����',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_smtpsoft",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_smtpsoft', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_smtpsoft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});