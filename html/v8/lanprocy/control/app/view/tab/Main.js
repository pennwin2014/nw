Ext.define('LanProcy.view.tab.Main' ,{
  extend: 'Ext.tab.Panel',
  alias : 'widget.procytabs',

  initComponent: function() {
    this.items = [
      {xtype:'procyfile'},
      {xtype:'procyoutlink'},
      {xtype:'procyapp'},
      {xtype:'procydevice'},
      {xtype:'procyurl'},
      {xtype:'procynet'},
      {xtype:'procylog'}
    ];

    this.callParent(arguments);
  }
});