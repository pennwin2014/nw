Ext.define('LanProcy.view.tab.Outlink' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyoutlink',
  
  title: "外联策略",
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
    	value:'outlink'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
  		xtype: 'checkboxgroup',
    	fieldLabel: "控制方式",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 2,
    	vertical: true,
    	items:[
    		{ boxLabel: '禁止WIFI', name: 'controlmark', inputValue: '2'},
        { boxLabel: '禁止ADSL', name: 'controlmark', inputValue: '4'},
        { boxLabel: '禁止VPN', name: 'controlmark', inputValue: '8'},
        { boxLabel: '禁止离线连接网络', name: 'controlmark', inputValue: '16'},
        { boxLabel: '禁止未装客户端连网', name: 'controlmark', inputValue: '64', hidden:false}
    	]
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
