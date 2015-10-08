Ext.define('Fileout.view.tab.Other',{
  extend: 'Ext.form.Panel',
  alias : 'widget.other',
  
  title: "���������ⷢ",
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
    	value:'other'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {xtype:'othersoft',title:"���������ⷢ����",comboLabel:"��������",disable:true}
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});