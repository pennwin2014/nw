Ext.define('Fileout.store.ComCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoaded: false,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamName'
  }
});