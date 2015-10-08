/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanProcy',{
  extend: 'Ext.tab.Panel',

  initComponent : function(){
    Ext.apply(this,{
      layout : 'border',
      id:'lan_procy',
      margins: '0 0 0 0',
      forceFit: true,
      activeTab: 0,        
      defaults :{
        bodyPadding: 0
      },
      items: [{
        title: '策略控制',
        id:'control',           
        html: '<iframe id="lan_procy_control" name="lan_procy_control" src="/v8/lanprocy/control/lan_procy_control.html" scrolling="no" width=100% height=650 FrameBorder=0 ></iframe>' 
      },{
        title: '黑白名单',
        id:'blacklist', 
        html: '<iframe id="lan_procy_blacklist" name="lan_procy_blacklist" src="/v8/lanprocy/lan_procy_blacklist.html" scrolling="no" width=100% height=650 FrameBorder=0></iframe>' 
      }
    ,{
       title: '文件外发',
       id:'fileout', 
       html: '<iframe id="lan_procy_fileout" name="lan_procy_fileout" src="/v8/lanprocy/fileout/lan_procy_fileout.html" scrolling="no" width=100% height=650 FrameBorder=0></iframe>' 
      }
      ]/*,
  		listeners:{
  			'beforetabchange':function(tp, p){
  				var t_id=p.getId();  				
  				if(t_id=='sjcomputer'){
  					 bStartim=0;
  					 bStartcomp=1;
  				}
  				else if(t_id=='immon'){
  					   bStartim=1;
  					   bStartcomp=0;
  				}
  				else if(t_id=='sspm'){
  					   bStartim=2;
  					   bStartcomp=0;
  				}else if(t_id=='ssjc'){
  					   bStartim=0;
  					   bStartcomp=1;
  //					 eval("lay_ssjk_sjcomputer").bStart=0;
  //					 eval("lay_ssjk_immon").bStart=1;
  				}else{
  					   bStartim=0;
  					   bStartcomp=0;
  	//			 eval("lay_ssjk_sjcomputer").bStart=0;
  //	  			 eval("lay_ssjk_immon").bStart=0;
  				}
  			}
  		}*/
  	});
    this.callParent(arguments);
  }
});