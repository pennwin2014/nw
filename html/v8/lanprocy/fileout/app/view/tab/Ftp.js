Ext.define('Fileout.view.tab.Ftp',{
  extend: 'Ext.form.Panel',
  alias : 'widget.ftp',
  
  title: "FTP�ⷢ",
  padding: "5 15 5 15",
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
    	value:'ftp'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'ftpserver',title:"FTP����������",comboLabel:"����������"},
	      {xtype:'ftpsoft',title:"FTP�������",comboLabel:"FTP����",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
