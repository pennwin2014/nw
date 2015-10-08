Ext.define('Fileout.view.tab.Ftp',{
  extend: 'Ext.form.Panel',
  alias : 'widget.ftp',
  
  title: "FTP外发",
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
    	value:'ftp'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'ftpserver',title:"FTP服务器控制",comboLabel:"服务器类型"},
	      {xtype:'ftpsoft',title:"FTP软件控制",comboLabel:"FTP进程",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
