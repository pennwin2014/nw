Ext.define('LanProcy.store.List', {
  extend: 'Ext.data.Store',

  model: 'LanProcy.model.List',

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_procyList',
    reader: {
      type: 'json',
      root: 'data'
    }
  }
});