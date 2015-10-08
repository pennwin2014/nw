Ext.application({
  name: 'Fileout',

  appFolder: 'app',
  
  controllers: [
  	'Fileout'
  ],

  launch: function() {
    Ext.tip.QuickTipManager.init();
    Ext.create('Ext.container.Viewport', {
      layout: 'vbox',
      
      height: 690,
      
      items: [
        {xtype: 'fileouttabs',width:'100%',height:370}
        ,
        {xtype: 'fileoutlist',width:'100%',height:320}
      ]
    });
  }
});