Ext.define('ncViewer.lanBlackList',{
  extend: 'Ext.panel.Panel',

  initComponent : function(){
  	var w=window.innerWidth
			|| document.documentElement.clientWidth
			|| document.body.clientWidth;
			
			var h=window.innerHeight
			|| document.documentElement.clientHeight
			|| document.body.clientHeight;
    Ext.apply(this,{
      id:'lan_blacklist',
    	html: '<iframe  id="lan_blacklist" name="lan_blacklist" src="/v8/lanBlackList/lanBlackList.html" scrolling="no" width='+w+' height='+(h-138)+'  FrameBorder=0 ></iframe>'       
      });
    this.callParent(arguments);
  }
})