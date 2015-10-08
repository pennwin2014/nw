Ext.define('Fileout.view.common.HttpsSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.httpssoft',
  
  mark: 16,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
     this.comboName='httpscombo';
    this.columns = [
      {header:'������',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_https",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode_https', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode_https', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});