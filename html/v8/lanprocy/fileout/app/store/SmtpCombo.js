Ext.define('Fileout.store.SmtpCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamName&paramtype@4'
  }
});