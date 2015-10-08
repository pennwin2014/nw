Ext.define('LanProcy.view.tab.Outlink' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyoutlink',
  
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
    	value:'outlink'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
  		xtype: 'checkboxgroup',
    	fieldLabel: "���Ʒ�ʽ",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 2,
    	vertical: true,
    	items:[
    		{ boxLabel: '��ֹWIFI', name: 'controlmark', inputValue: '2'},
        { boxLabel: '��ֹADSL', name: 'controlmark', inputValue: '4'},
        { boxLabel: '��ֹVPN', name: 'controlmark', inputValue: '8'},
        { boxLabel: '��ֹ������������', name: 'controlmark', inputValue: '16'},
        { boxLabel: '��ֹδװ�ͻ�������', name: 'controlmark', inputValue: '64', hidden:false}
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
