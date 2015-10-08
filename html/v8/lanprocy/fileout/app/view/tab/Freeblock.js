Ext.define('Fileout.view.tab.Freeblock',{
  extend: 'Ext.form.Panel',
  alias : 'widget.freeblock',
  
  title: "放行和禁止",
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
    	value:'freeblock'
	  },{
	    layout: 'hbox',
	    autoScroll:true,
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'freesoftadd',title:"放行不分析通讯软件",comboLabel:"软件类型"},
	      {xtype:'freeaddressadd',title:"放行不分析服务器列表",comboLabel:"地址类型"},
	      {xtype:'blocksoftadd',title:"禁止外发文件通讯软件",comboLabel:"软件类型"},
	      {xtype:'blockaddressadd',title:"禁止外发文件服务器列表",comboLabel:"地址类型"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
