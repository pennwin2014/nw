Ext.define('Fileout.view.list.FileoutList' ,{
  extend: 'Ext.grid.Panel',
  alias : 'widget.fileoutlist',

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
      {header: '用户名', dataIndex: 'devname',  flex:1},
      {header: '策略类型', dataIndex: 'procytype', flex:1},
      {header: '控制', dataIndex: 'controlcode_desc', width:300},
      {header: '用户级别', dataIndex: 'levelname', widht:40},
      {header: '修改时间', dataIndex: 'modtime',   flex:1}
    ];

    this.callParent(arguments);
  }
    
});