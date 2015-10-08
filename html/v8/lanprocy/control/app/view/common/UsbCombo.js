Ext.define('LanProcy.view.common.UsbCombo' ,{
  extend: 'Ext.form.field.ComboBox',
  alias : 'widget.usbcombo',

	fieldLabel: 'USB����',
  store: 'UsbCombo',
  queryMode: 'local',
  displayField: 'text',
  valueField: 'id',

  initComponent: function() {
    this.callParent(arguments);
  }
});