Ext.define('Fileout.store.FtpGrid', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.FieldSet',
  //autoLoad: true,

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_searchParamValue',
    reader: {
      type:'json',
      root: 'data'
    }
  }
});