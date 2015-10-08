Ext.define('Fileout.view.tab.Freeblock',{
  extend: 'Ext.form.Panel',
  alias : 'widget.freeblock',
  
  title: "���кͽ�ֹ",
  padding: "5 0 5 15",
  frame: true,
  
  defaults: {labelWidth: 120},
  
  initComponent: function() {
    this.tbar = ['->',
      {text:"Ӧ�õ���ǰ",action:'change'},
   		{text:"Ӧ�õ�����",action:'insert'}
    ];
      	
  	this.items = [{
    	xtype: 'hidden',
    	name: 'type',
    	value:'freeblock'
	  },{
	    layout: 'hbox',
	    autoScroll:true,
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'freesoftadd',title:"���в�����ͨѶ���",comboLabel:"�������"},
	      {xtype:'freeaddressadd',title:"���в������������б�",comboLabel:"��ַ����"},
	      {xtype:'blocksoftadd',title:"��ֹ�ⷢ�ļ�ͨѶ���",comboLabel:"�������"},
	      {xtype:'blockaddressadd',title:"��ֹ�ⷢ�ļ��������б�",comboLabel:"��ַ����"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
