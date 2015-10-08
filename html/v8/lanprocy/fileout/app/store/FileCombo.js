Ext.define('Fileout.store.FileCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamName&paramtype@7'
  }
});