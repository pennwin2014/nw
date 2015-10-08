Ext.define('LanProcy.view.tab.Log' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.procylog',
  
  title: "其他策略",
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
    	value:'log'
	  },{
	    xtype: 'fieldset',
	    title: '日志传送控制',
	    items:[{
      	xtype: 'timecombo',
      	name: 'timeid',
      	allowBlank: false
  	  },{
    		xtype: 'checkboxgroup',
      	fieldLabel: "控制方式",
      	name: 'controlmark',
      	columns: 2,
      	vertical: true,
      	items:[
      		{ boxLabel: '不上传聊天日志', name: 'controlmark', inputValue: '1'},
          { boxLabel: '不上传屏幕日志', name: 'controlmark', inputValue: '2'},
          { boxLabel: '不上传程序日志', name: 'controlmark', inputValue: '4'},
          { boxLabel: '不上传文件日志', name: 'controlmark', inputValue: '8'},
          { boxLabel: '不上传网址日志', name: 'controlmark', inputValue: '16'},
      	  { boxLabel: '不上传外设日志', name: 'controlmark', inputValue: '32'}
        ]
    	},{
      	xtype: 'numberfield',
        fieldLabel: '截屏间隔(分钟)',
        name: 'text',
        value: 2
      },{
		hidden:true,
        xtype: 'checkboxfield',
        boxLabel: '离开公司不控制', 
        name: 'controloffline', 
        inputValue: '128'
      },{
      	xtype: 'radiogroup',
        fieldLabel: '离线有否效',
        name: 'type',
      	columns: 2,
        items: [
          { boxLabel: '离线有效', name: 'type', inputValue: '0', checked: true},
          { boxLabel: '离线无效', name: 'type', inputValue: '1'}
        ]
      }]
	  },{
      xtype: 'typefieldset',
      title: '敏感进程屏幕控制',
      gid: 'senseGrid',
      store: 'SenseGrid',
      columns: [
        {header:'进程名',dataIndex:'name',flex:2},
        {header:'窗口标题关键词',dataIndex:'detail',flex:3,
          renderer: function(value,metaData,record){
            metaData.tdAttr = 'data-qtip="关键词以逗号分隔，为空则对整个进程抓屏"';
            return value;
          }
        }
      ],
      combo: Ext.create('LanProcy.view.common.SenseProcCombo',{
        id: "senseProcCombo"
      }),
      item1:{
        xtype: 'numberfield',
        minValue: 1,
        id: 'senseText2',
        margin: "5 0 0 0",
        fieldLabel: '抓屏频率(秒)',
        value: 2
      }
    }];
	
  	this.callParent(arguments);
  }
  
});
