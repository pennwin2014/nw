Ext.define('Fileout.view.tab.Upan',{
  extend: 'Ext.form.Panel',
  alias : 'widget.upan',
  
  title: "U���ⷢ",
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
    	value:'upan'
	  },{
	    layout: 'hbox',
	    xtype: 'fieldcontainer',
	    combineErrors: true,
	    items:[
	      {xtype:'file',title:"�ļ����ݿ���",comboLabel:"�ļ�����"},
	      {
	        xtype: 'fieldset',
	        title: '������',
	        width: 222,
          height: 250,
          items:[{
            xtype: 'usbcombo',
            name: 'usbid'
          },{
          	xtype: 'radiogroup',
          	margin: "0 0 0 10",
            name: 'mode_usb',
          	columns: 1,
            items: [
              { boxLabel: '��ֹģʽ', name: 'mode_usb', inputValue: 1, checked: true},
              { boxLabel: 'ֻ����ģʽ', name: 'mode_usb', inputValue: 2}
            ]
          }]
	      }
	    ]
    }];
	  
  	this.callParent(arguments);
  }
  
});