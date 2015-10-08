Ext.define('Fileout.store.FtpCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamName&paramtype@11'
  }
});