Ext.define('Fileout.view.tab.Smtp',{
  extend: 'Ext.form.Panel',
  alias : 'widget.smtp',
  
  title: "SMTP外发",
  padding: "5 0 5 15",
  frame: true,
  
  defaults: {labelWidth: 120},
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"应用到当前",action:'change'},
   		{text:"应用到其他",action:'insert'}
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
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'smtpadd',title:"邮件主机控制",comboLabel:"主机类型"},
	      {xtype:'sender',title:"发件人控制",comboLabel:"发件人类型"},
	      {xtype:'receiver',title:"收件人控制",comboLabel:"收件人类型"},
	      {xtype:'smtpsoft',title:"邮件软件控制",comboLabel:"进程类型",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
