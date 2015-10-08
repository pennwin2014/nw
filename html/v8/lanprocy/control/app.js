Ext.application({
  name: 'LanProcy',

  appFolder: 'app',
  
  controllers: [
  	'Procy'
  ],

  launch: function() {
    Ext.tip.QuickTipManager.init();
    Ext.create('Ext.container.Viewport', {
      layout: 'hbox',
      
      height: 530,
      
      items: [
        {xtype: 'procylist',width:'50%'},
        {xtype: 'procytabs',width:'50%'}
      ]
    });
  }
});