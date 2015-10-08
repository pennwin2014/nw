Ext.define('Fileout.view.common.FileSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.file',
  
  mark: 7,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='filecombo';
    this.columns = [
      {header:'扩展名',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.FileGrid');
    
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode",
      items: [
        { boxLabel: '启用禁止模式', name: 'mode', inputValue: '1', checked: true },
        { boxLabel: '启用只允许模式', name: 'mode', inputValue: '2'}
      ]
    });
    this.item2 = Ext.create('Ext.form.field.Text',{
      margin: "0 0 0 5",
      name:'filelimit',
      fieldLabel: '文件大小(kb)'
    });
    this.item3 = Ext.create('Ext.form.CheckboxGroup',{
      columns: 1,
      vertical: true,
      name: "mode2",
      items: [
        { boxLabel: '必须审批通过外发', name: 'mode2', inputValue: '1'},
        { boxLabel: '备份到文件服务器', name: 'mode2', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});