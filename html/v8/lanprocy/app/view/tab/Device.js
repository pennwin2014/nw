Ext.define('LanProcy.view.tab.Device' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procydevice',
  
  title: "�������",
  height : 550,
  padding: "5 20 20 20",
  frame: true,
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"Ӧ�õ���ǰ",action:'change'},
   		{text:"Ӧ�õ�����",action:'insert'}
    ];  	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'procytype1',
    	value:'device'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid',
    	margin: "0 0 0 10"
	  },{
    	xtype:"fieldset",
   		title: 'U��',
    	items:[{
				xtype:'usbcombo',
      	name: 'usbid'
    	},{
    		xtype:"checkboxgroup",
    		fieldLabel: '���Ʒ�ʽ',
    		name: "controlmark",
				columns: 3,
    		items:[
    		  {boxLabel:'����U��',name:'controlmark',inputValue:'1'},
    		  {boxLabel:'��ֹU�̶�',name:'controlmark',inputValue:'2'},
    		  {boxLabel:'��ֹU��д',name:'controlmark',inputValue:'4'}
    		]
    	}]
    },{
    	xtype:"fieldset",
   		title: '��������',
    	items:[{
    		xtype:"checkboxgroup",
    		fieldLabel: '���Ʒ�ʽ',
    		name: "controlmark2",
    		columns: 3,
    		items:[
    		  {boxLabel:'��ֹ���̿�¼',name:'controlmark2',inputValue:'8'},
    		  {boxLabel:'��ֹ��ӡ',name:'controlmark2',inputValue:'16'}
    		]
    	}]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '��������',
    	margin: "0 0 0 10",
      name: 'snap',
    	columns: 3,
      items: [
        { boxLabel: '������', name: 'snap', inputValue: '0', checked: true},
        { boxLabel: '����', name: 'snap', inputValue: '2'}
      ]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '�����з�Ч',
      name: 'type',
    	margin: "0 0 0 10",
    	columns: 3,
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
