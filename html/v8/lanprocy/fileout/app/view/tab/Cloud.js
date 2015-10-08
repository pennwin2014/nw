Ext.define('Fileout.view.tab.Cloud',{
  extend: 'Ext.form.Panel',
  alias : 'widget.cloud',
  
  title: "云盘外发",
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
    	value:'cloud'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"文件类型"},
	      {xtype:'cloudsoft',title:"云盘软件控制",comboLabel:"软件类型",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
