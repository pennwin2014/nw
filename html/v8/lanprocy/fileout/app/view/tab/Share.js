Ext.define('Fileout.view.tab.Share',{
  extend: 'Ext.form.Panel',
  alias : 'widget.share',
  
  title: "����Ŀ¼�ⷢ",
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
    	value:'share'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"����"},
	      {
	        xtype: 'fieldset',
	        title: '������',
	        width: 222,
          height: 250,
          items:[{
          	xtype: 'checkboxgroup',
          	margin: "0 0 0 10",
            name: 'snap',
          	columns: 1,
            items: [
              { boxLabel: '��ֹ����Ŀ¼�ⷢ�ļ�', name: 'snap1', inputValue: 1},
              { boxLabel: '��ֹ����Ŀ¼���ļ�', name: 'snap2', inputValue: 1}
            ]
          }]
	      }
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});
