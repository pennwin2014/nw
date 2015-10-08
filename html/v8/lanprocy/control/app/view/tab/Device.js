Ext.define('LanProcy.view.tab.Device' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procydevice',
  
  title: "外设策略",
  height : 550,
  padding: "5 20 20 20",
  frame: true,
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"应用到当前",action:'change'},
   		{text:"应用到其他",action:'insert'}
    ];  	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'procytype1',
    	value:'device'
	  },{
    	xtype: 'timecombo',
    	name: 'timeid',
    	margin: "0 0 0 10"
	  },{
    	xtype:"fieldset",
   		title: 'U盘',
    	items:[{
				xtype:'usbcombo',
      	name: 'usbid'
    	},{
    		xtype:"checkboxgroup",
    		fieldLabel: '控制方式',
    		name: "controlmark",
				columns: 3,
    		items:[
    		  {boxLabel:'禁用U盘',name:'controlmark',inputValue:'1'},
    		  {boxLabel:'禁止U盘读',name:'controlmark',inputValue:'2'},
    		  {boxLabel:'禁止U盘写',name:'controlmark',inputValue:'4'}
    		]
    	}]
    },{
    	xtype:"fieldset",
   		title: '其他外设',
    	items:[{
    		xtype:"checkboxgroup",
    		fieldLabel: '控制方式',
    		name: "controlmark2",
    		columns: 3,
    		items:[
    		  {boxLabel:'禁止光盘刻录',name:'controlmark2',inputValue:'8'},
    		  {boxLabel:'禁止打印',name:'controlmark2',inputValue:'32'}
    		]
    	}]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '启动截屏',
    	margin: "0 0 0 10",
      name: 'snap',
    	columns: 3,
      items: [
        { boxLabel: '不截屏', name: 'snap', inputValue: '0', checked: true},
        { boxLabel: '截屏', name: 'snap', inputValue: '2'}
      ]
    },{
    	xtype: 'radiogroup',
      fieldLabel: '离线有否效',
      name: 'type',
    	margin: "0 0 0 10",
    	columns: 3,
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
