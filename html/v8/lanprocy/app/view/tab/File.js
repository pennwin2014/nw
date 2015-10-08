Ext.define('LanProcy.view.tab.File' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyfile',
  
  title: "�ļ�����",
  //width: 300,
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
    	value:'file'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
    	xtype: 'textfield',
    	fieldLabel: "�ļ�������",
    	name: "text"
    },{
    	xtype: 'checkboxgroup',
    	fieldLabel: "���Ʒ�ʽ",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 1,
    	vertical: true,
    	items:[
    		{ boxLabel: '��ֹ�ļ����Ƶ�U��', name: 'controlmark', inputValue: '1' },
        { boxLabel: '��ֹ�ļ����Ƶ�����Ŀ¼', name: 'controlmark', inputValue: '2'},
        { boxLabel: '��ֹ�ļ����Ƶ�����', name: 'controlmark', inputValue: '4' }
    	]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '��������',
      name: 'snap',
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