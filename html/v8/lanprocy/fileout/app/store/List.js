Ext.define('Fileout.store.List', {
  extend: 'Ext.data.Store',

  model: 'Fileout.model.List',

  proxy: {
    type: 'ajax',
    url : '/pronline/Msg?FunName@lan_fileoutList',
    reader: {
      type: 'json',
      root: 'data',
      totalProperty: 'totalCount'
    }
  }
});

