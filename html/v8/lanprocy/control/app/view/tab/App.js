Ext.define('LanProcy.view.tab.App' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyapp',
  
	title: "Ӧ�ó������",
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
    	value:'app'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
    	xtype: 'softcombo',
    	name: 'softid',
    	allowBlank: false
	  },{
  		xtype: 'checkboxgroup',
    	fieldLabel: "���Ʒ�ʽ",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 2,
    	vertical: true,
    	items:[
        //{ boxLabel: '��������', name: 'controlmark', inputValue: '0'},
    		{ boxLabel: '��ֹ����', name: 'controlmark', inputValue: '1'}
    	]
  	},{
  	  xtype: 'textfield',
  	  fieldLabel: "��������ʱ��(����)",
  	  name: "text"
  	},{
    	xtype: 'radiogroup',
      fieldLabel: '��������',
      name: 'snap',
    	columns: 2,
      items: [
        { boxLabel: '������', name: 'snap', inputValue: '0', checked: true},
        { boxLabel: '����', name: 'snap', inputValue: '2'}
      ]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '�����з�Ч',
      name: 'type',
    	columns: 2,
      items: [
        { boxLabel: '������Ч', name: 'type', inputValue: '0', checked: true},
        { boxLabel: '������Ч', name: 'type', inputValue: '1'}
      ]
    }];
/*  	
  	this.buttons = [
   		{text:"�޸�",action:'change'},
   		{text:"����",action:'insert'}
   	];
*/  	
  	this.callParent(arguments);
  }

});   