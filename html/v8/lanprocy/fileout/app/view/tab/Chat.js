Ext.define('Fileout.view.tab.Chat',{
  extend: 'Ext.form.Panel',
  alias : 'widget.chat',
  
  title: "聊天软件外发",
  padding: "5 15 5 15",
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
    	value:'chat'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'chatsoft',title:"聊天软件控制",comboLabel:"软件类型",disable:true},
	      {xtype:'account',title:"聊天账号控制",comboLabel:"帐号类型"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
