Ext.define('Fileout.view.tab.Https' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.https',
  
  title: "HTTPS外发",
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
    	value:'https'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'httpssoft',title:"HTTPS通信软件",comboLabel:"软件类型",disable:true},
	      {xtype:'serverip',title:"服务器地址控制",comboLabel:"地址类型"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
