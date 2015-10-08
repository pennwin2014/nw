Ext.define('LanProcy.view.list.List' ,{
  extend: 'Ext.grid.Panel',
  alias : 'widget.procylist',

  title : '�����б�',
  store : 'List',
  
  height: 570,
  
  initComponent: function() {
    
    this.tbar = ['-','-',{
      text: "ɾ��",
      action: "remove"
    },{
      text: "ȫ��ɾ��",
      action: "removeall"
    }];
  	
    this.columns = [
      {header: '����',     dataIndex: 'groupname', flex:1},
      {header: 'IP',       dataIndex: 'ip',        flex:1},
      {header: 'ʱ������', dataIndex: 'timename',  flex:1,
        renderer:function(value,metaData,record){
          if(record.get('timeid')=='0'){
            return 'ȫ��'
          }
          return value;
        }
      },
      {header: '��������', dataIndex: 'procytype', flex:1},
      {header: '�û�����', dataIndex: 'levelname', flex:1},
      {header: '����ʱ��', dataIndex: 'modtime',   flex:1}
    ];

    this.callParent(arguments);
  }
    
});