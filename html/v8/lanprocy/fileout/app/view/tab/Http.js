Ext.define('Fileout.view.tab.Http' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.http',
  
  title: "HTTP�ⷢ",
  padding: "15 5 5 15",
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
    	value:'http'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'smtpadd',title:"�ʼ���������",comboLabel:"��������"},
	      //{xtype:'sender',title:"�����˿���",comboLabel:"����"},
	      //{xtype:'receiver',title:"�ռ��˿���",comboLabel:"����"},
	      {xtype:'httpsoft',title:"���������",comboLabel:"��������",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});