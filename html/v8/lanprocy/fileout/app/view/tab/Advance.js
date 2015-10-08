Ext.define('Fileout.view.tab.Advance' ,{
  extend: 'Ext.form.Panel',
  alias : 'widget.advance',
  
  title: "���Ȳ���",
  height : 550,
  padding: "5 20 20 20",
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
    	value:'advance'
	  },{
      xtype: 'checkboxgroup',
      name: 'forbidden',
      margin: '30 0 0 0',
      columns: 2,
      vertical: true,
      items: [
        { boxLabel: '��ֹHTTP�ⷢ', name: 'forbidden', inputValue: '1' },
        { boxLabel: '��ֹSMTP�ⷢ', name: 'forbidden', inputValue: '2' },
        { boxLabel: '��ֹFTP�ⷢ', name: 'forbidden', inputValue: '4' },
        { boxLabel: '��ֹ���������ⷢ', name: 'forbidden', inputValue: '16' },
        { boxLabel: '��ֹ�����ⷢ', name: 'forbidden', inputValue: '32' },
        { boxLabel: '��ֹU���ⷢ', name: 'forbidden', inputValue: '64' },
        { boxLabel: '��ֹ����Ŀ¼�ⷢ', name: 'forbidden', inputValue: '128' },
        { boxLabel: '��ֹ���������ⷢ', name: 'forbidden', inputValue: '256' }
      ]
    }];
	  
  	this.callParent(arguments);
  }
  
});