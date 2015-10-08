Ext.define('LanProcy.view.tab.Log' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procylog',
  
  title: "��������",
  height : 550,
  padding: "5 20 20 20",
  frame: true,
  
  defaults: {labelWidth: 120},
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"Ӧ�õ���ǰ",action:'change'},
   		{text:"Ӧ�õ�����",action:'insert'}
    ];
  	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'procytype1',
    	value:'log'
	  },{
	    xtype: 'fieldset',
	    title: '��־���Ϳ���',
	    items:[{
      	xtype: 'timecombo',
      	name: 'timeid',
      	allowBlank: false
  	  },{
    		xtype: 'checkboxgroup',
      	fieldLabel: "���Ʒ�ʽ",
      	name: 'controlmark',
      	columns: 2,
      	vertical: true,
      	items:[
      		{ boxLabel: '���ϴ�������־', name: 'controlmark', inputValue: '1'},
          { boxLabel: '���ϴ���Ļ��־', name: 'controlmark', inputValue: '2'},
          { boxLabel: '���ϴ�������־', name: 'controlmark', inputValue: '4'},
          { boxLabel: '���ϴ��ļ���־', name: 'controlmark', inputValue: '8'},
          { boxLabel: '���ϴ���ַ��־', name: 'controlmark', inputValue: '16'},
      	  { boxLabel: '���ϴ�������־', name: 'controlmark', inputValue: '32'}
        ]
    	},{
      	xtype: 'numberfield',
        fieldLabel: '�������(����)',
        name: 'text',
        value: 2
      },{
		hidden:true,
        xtype: 'checkboxfield',
        boxLabel: '�뿪��˾������', 
        name: 'controloffline', 
        inputValue: '128'
      },{
      	xtype: 'radiogroup',
        fieldLabel: '�����з�Ч',
        name: 'type',
      	columns: 2,
        items: [
          { boxLabel: '������Ч', name: 'type', inputValue: '0', checked: true},
          { boxLabel: '������Ч', name: 'type', inputValue: '1'}
        ]
      }]
	  },{
      xtype: 'typefieldset',
      title: '���н�����Ļ����',
      gid: 'senseGrid',
      store: 'SenseGrid',
      columns: [
        {header:'������',dataIndex:'name',flex:2},
        {header:'���ڱ���ؼ���',dataIndex:'detail',flex:3,
          renderer: function(value,metaData,record){
            metaData.tdAttr = 'data-qtip="�ؼ����Զ��ŷָ���Ϊ�������������ץ��"';
            return value;
          }
        }
      ],
      combo: Ext.create('LanProcy.view.common.SenseProcCombo',{
        id: "senseProcCombo"
      }),
      item1:{
        xtype: 'numberfield',
        minValue: 1,
        id: 'senseText2',
        margin: "5 0 0 0",
        fieldLabel: 'ץ��Ƶ��(��)',
        value: 2
      }
    }];
	
  	this.callParent(arguments);
  }
  
});
