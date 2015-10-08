Ext.define('Fileout.store.SoftCombo', {
  extend: 'Ext.data.Store',

  fields:['recno','name'],

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamValue',
    reader: {
      type:'json',
      root: 'data'
    }
  }
});