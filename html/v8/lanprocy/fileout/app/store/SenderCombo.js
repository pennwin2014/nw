Ext.define('Fileout.store.SenderCombo', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.Combo',
  autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamName&paramtype@5'
  }
});