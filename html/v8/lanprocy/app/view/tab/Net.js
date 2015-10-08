Ext.define('LanProcy.view.tab.Net' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procynet',
  
  title: "�������",
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
    	value:'net'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
	    xtype: 'textfield',
	    fieldLabel: "���ö˿�<br/>(�Զ��ŷָ�)",
	    name: "text",
    	allowBlank: false
	  },{
	    xtype: 'radiogroup',
      fieldLabel: '������������',
      name: 'controlmark1',
    	columns: 2,
    	allowBlank: false,
      items: [
        { boxLabel: 'TCP', name: 'controlmark', inputValue: '6', checked: true},
        { boxLabel: 'UDP', name: 'controlmark', inputValue: '17'}
      ]
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