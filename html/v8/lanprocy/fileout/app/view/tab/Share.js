Ext.define('Fileout.view.tab.Share',{
  extend: 'Ext.form.Panel',
  alias : 'widget.share',
  
  title: "共享目录外发",
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
    	value:'share'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"文件内容控制",comboLabel:"名称"},
	      {
	        xtype: 'fieldset',
	        title: '设置项',
	        width: 222,
          height: 250,
          items:[{
          	xtype: 'checkboxgroup',
          	margin: "0 0 0 10",
            name: 'snap',
          	columns: 1,
            items: [
              { boxLabel: '禁止共享目录外发文件', name: 'snap1', inputValue: 1},
              { boxLabel: '禁止共享目录读文件', name: 'snap2', inputValue: 1}
            ]
          }]
	      }
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
