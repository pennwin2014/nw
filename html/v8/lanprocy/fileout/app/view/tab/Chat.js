Ext.define('Fileout.view.tab.Chat',{
  extend: 'Ext.form.Panel',
  alias : 'widget.chat',
  
  title: "��������ⷢ",
  padding: "5 15 5 15",
  frame: true,
  
  defaults: {labelWidth: 120},
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"Ӧ�õ���ǰ",action:'change'},
   		{text:"Ӧ�õ�����",action:'insert'}
    ];
      	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'type',
    	value:'chat'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'chatsoft',title:"�����������",comboLabel:"�������",disable:true},
	      {xtype:'account',title:"�����˺ſ���",comboLabel:"�ʺ�����"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
