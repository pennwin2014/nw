Ext.define('Fileout.store.UsbCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_USB_class_search'
  }
});