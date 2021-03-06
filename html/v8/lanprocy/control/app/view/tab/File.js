Ext.define('LanProcy.view.tab.File' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procyfile',
  
  title: "文件策略",
  //width: 300,
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
    	value:'file'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid'
	  },{
    	xtype: 'textfield',
    	fieldLabel: "文件名包含",
    	name: "text"
    },{
    	xtype: 'checkboxgroup',
    	fieldLabel: "控制方式",
    	name: 'controlmark',
    	allowBlank: false,
    	columns: 1,
    	vertical: true,
    	items:[
    		{ boxLabel: '禁止文件复制到U盘', name: 'controlmark', inputValue: '1' },
        { boxLabel: '禁止文件复制到共享目录', name: 'controlmark', inputValue: '2'},
        { boxLabel: '禁止文件复制到云盘', name: 'controlmark', inputValue: '4' }
    	]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '启动截屏',
      name: 'snap',
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