Ext.define('LanProcy.view.tab.Url' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyurl',
  
  title: "��ַ����",
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
    	value:'url'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
	    xtype: 'textfield',
	    fieldLabel: "������ַ�ؼ���",
	    name: "text",
    	allowBlank: false
	  },{
    	xtype: 'radiogroup',
      fieldLabel: '��������',
      name: 'snap',
    	columns: 2,
    	allowBlank: false,
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