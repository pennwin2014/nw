Ext.define('LanProcy.view.list.List' ,{
  extend: 'Ext.grid.Panel',
  alias : 'widget.procylist',

  title : '策略列表',
  store : 'List',
  
  height: 570,
  
  initComponent: function() {
    
    this.tbar = ['-','-',{
      text: "删除",
      action: "remove"
    },{
      text: "全部删除",
      action: "removeall"
    }];
  	
    this.columns = [
      {header: '部门',     dataIndex: 'groupname', flex:1},
      {header: 'IP',       dataIndex: 'ip',        flex:1},
      {header: '时间名称', dataIndex: 'timename',  flex:1,
        renderer:function(value,metaData,record){
          if(record.get('timeid')=='0'){
            return '全部'
          }
          return value;
        }
      },
      {header: '策略类型', dataIndex: 'procytype', flex:1},
      {header: '用户级别', dataIndex: 'levelname', flex:1},
      {header: '设置时间', dataIndex: 'modtime',   flex:1}
    ];

    this.callParent(arguments);
  }
    
});