Ext.define('Fileout.view.tab.Smtp',{
  extend: 'Ext.form.Panel',
  alias : 'widget.smtp',
  
  title: "SMTP�ⷢ",
  padding: "5 0 5 15",
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
    	value:'smtp'
	  },{
	    layout: 'hbox',
	    autoScroll:true,
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'smtpadd',title:"�ʼ���������",comboLabel:"��������"},
	      {xtype:'sender',title:"�����˿���",comboLabel:"����������"},
	      {xtype:'receiver',title:"�ռ��˿���",comboLabel:"�ռ�������"},
	      {xtype:'smtpsoft',title:"�ʼ��������",comboLabel:"��������",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
