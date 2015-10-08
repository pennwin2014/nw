Ext.define('Fileout.view.common.CloudSoftSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.cloudsoft',
  
  mark: 14,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='cloudcombo';
    this.columns = [
      {header:'进程名',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.SoftGrid');
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode_cloudsoft",
      items: [
        { boxLabel: '启用禁止模式', name: 'mode_cloudsoft', inputValue: '1', checked: true },
        { boxLabel: '启用只允许模式', name: 'mode_cloudsoft', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});