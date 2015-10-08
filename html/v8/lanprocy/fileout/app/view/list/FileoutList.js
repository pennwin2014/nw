Ext.define('Fileout.view.list.FileoutList' ,{
  extend: 'Ext.grid.Panel',
  alias : 'widget.fileoutlist',

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
      {header: '�û���', dataIndex: 'devname',  flex:1},
      {header: '��������', dataIndex: 'procytype', flex:1},
      {header: '����', dataIndex: 'controlcode_desc', width:300},
      {header: '�û�����', dataIndex: 'levelname', widht:40},
      {header: '�޸�ʱ��', dataIndex: 'modtime',   flex:1}
    ];

    this.callParent(arguments);
  }
    
});