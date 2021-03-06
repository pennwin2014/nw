Ext.define('LanProcy.view.tab.App' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyapp',
  
	title: "应用程序策略",
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
    	value:'app'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
    	xtype: 'softcombo',
    	name: 'softid',
    	allowBlank: false
	  },{
  		xtype: 'checkboxgroup',
    	fieldLabel: "控制方式",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 2,
    	vertical: true,
    	items:[
        //{ boxLabel: '允许运行', name: 'controlmark', inputValue: '0'},
    		{ boxLabel: '禁止运行', name: 'controlmark', inputValue: '1'}
    	]
  	},{
  	  xtype: 'textfield',
  	  fieldLabel: "限制运行时间(分钟)",
  	  name: "text"
  	},{
    	xtype: 'radiogroup',
      fieldLabel: '启动截屏',
      name: 'snap',
    	columns: 2,
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
