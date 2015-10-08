Ext.define('Fileout.view.tab.FileoutMain' ,{
  extend: 'Ext.tab.Panel',
  alias : 'widget.fileouttabs',

  initComponent: function() {
    this.items = [
      {xtype:'advance'},
      {xtype:'http'},
      {xtype:'smtp'},
      {xtype:'ftp'},
      {xtype:'chat'},
      {xtype:'cloud'},
      {xtype:'upan'},
      {xtype:'share'},
      {xtype:'https'},
      {xtype:'other'},
      {xtype:'freeblock'}
    ];

    this.callParent(arguments);
  }
});