Ext.define('Fileout.view.common.FileSet' ,{
  extend: 'Fileout.view.common.TypeFieldset',
  alias : 'widget.file',
  
  mark: 7,

  initComponent: function() {
    this.comboStore = Ext.create('Fileout.store.ComCombo');
    this.comboName='filecombo';
    this.columns = [
      {header:'��չ��',dataIndex:'name',flex:1}
    ];
    this.gridStore = Ext.create('Fileout.store.FileGrid');
    
    this.item1 = Ext.create('Ext.form.RadioGroup',{
      columns: 1,
      vertical: true,
      name: "mode",
      items: [
        { boxLabel: '���ý�ֹģʽ', name: 'mode', inputValue: '1', checked: true },
        { boxLabel: '����ֻ����ģʽ', name: 'mode', inputValue: '2'}
      ]
    });
    this.item2 = Ext.create('Ext.form.field.Text',{
      margin: "0 0 0 5",
      name:'filelimit',
      fieldLabel: '�ļ���С(kb)'
    });
    this.item3 = Ext.create('Ext.form.CheckboxGroup',{
      columns: 1,
      vertical: true,
      name: "mode2",
      items: [
        { boxLabel: '��������ͨ���ⷢ', name: 'mode2', inputValue: '1'},
        { boxLabel: '���ݵ��ļ�������', name: 'mode2', inputValue: '2'}
      ]
    });
    
    this.callParent(arguments);
  }
});