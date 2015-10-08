Ext.define('Fileout.view.tab.Https' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.https',
  
  title: "HTTPS�ⷢ",
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
    	value:'https'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'httpssoft',title:"HTTPSͨ������",comboLabel:"��������",disable:true},
	      {xtype:'serverip',title:"��������ַ����",comboLabel:"��ַ����"}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});