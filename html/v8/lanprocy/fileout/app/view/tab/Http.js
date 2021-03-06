Ext.define('Fileout.view.tab.Http' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.http',
  
  title: "HTTP外发",
  padding: "15 5 5 15",
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
    	value:'http'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'smtpadd',title:"邮件主机控制",comboLabel:"主机类型"},
	      //{xtype:'sender',title:"发件人控制",comboLabel:"名称"},
	      //{xtype:'receiver',title:"收件人控制",comboLabel:"名称"},
	      {xtype:'httpsoft',title:"浏览器控制",comboLabel:"进程类型",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
