Ext.define('LanProcy.view.tab.Url' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyurl',
  
  title: "网址策略",
  height : 550,
  padding: "5 20 20 20",
  frame: true,
  
  defaults: {labelWidth: 120},
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"应用到当前",action:'change'},
   		{text:"应用到其他",action:'insert'}
    ];
      	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'procytype1',
    	value:'url'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
	    xtype: 'textfield',
	    fieldLabel: "禁用网址关键词",
	    name: "text",
    	allowBlank: false
	  },{
    	xtype: 'radiogroup',
      fieldLabel: '启动截屏',
      name: 'snap',
    	columns: 2,
    	allowBlank: false,
      items: [
        { boxLabel: '不截屏', name: 'snap', inputValue: '0', checked: true},
        { boxLabel: '截屏', name: 'snap', inputValue: '2'}
      ]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '离线有否效',
      name: 'type',
    	columns: 2,
      items: [
        { boxLabel: '离线有效', name: 'type', inputValue: '0', checked: true},
        { boxLabel: '离线无效', name: 'type', inputValue: '1'}
      ]
    }];
/*    
    this.buttons = [
   		{text:"修改",action:'change'},
   		{text:"增加",action:'insert'}
   	];
*/  	
  	this.callParent(arguments);
  }
  
});
