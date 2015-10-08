/**
 定义实时监控tab
 */
 
Ext.define('ncViewer.lanSsjk',{
  extend: 'Ext.tab.Panel',

  initComponent : function(){
      Ext.apply(this,{
      layout : 'border',
      id:'lan_ssjk',
      margins: '0 0 0 0',
      forceFit: true,
      activeTab: 0,        
      defaults :{
        bodyPadding: 0
      },
      items: [{
          title: '在线计算机', 
          id:'sjcomputer',           
          html: '<iframe  id="lan_ssjk_sjcomputer" name="lan_ssjk_sjcomputer" src="/v8/ssjk/lan_sjcomputer_list.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
      },{
          title: '实时屏幕',
          id:'sspm', 
         // tname:'sspmtree',
        //  handler:onCheckItem,
          html: '<iframe  id="lan_ssjk_sspm" name="lan_ssjk_sspm" src="/v8/ssjk/lan_jiugongge.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
      },
      {
          title: '实时聊天',
          id:'immon',            
          html: '<iframe  id="lan_ssjk_immon" name="lan_ssjk_immon" src="/v8/ssjk/lan_online_im.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
      },{
          title: '实时进程',
          id:'ssjc',            
          html: '<iframe  id="procy_ssjk_ssjc" name="lan_ssjk_ssjc" src="/v8/ssjk/lan_online_proc.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
      }]
        ,
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
	//					 eval("lan_ssjk_sjcomputer").bStart=0;
	//					 eval("lan_ssjk_immon").bStart=1;
					}else{
						   bStartim=0;
						   bStartcomp=0;
		//			 eval("lan_ssjk_sjcomputer").bStart=0;
	//	  			 eval("lan_ssjk_immon").bStart=0;
					}
				},
				'tabchange':function(tp, p){
					try{
	  				var t_id=p.getId();
	  			  var vframe='lan_ssjk_'+t_id;
	  			  eval(vframe).reflash();
	  			}catch (e){
	  				
	  			}
				}
			}
		});
    this.callParent(arguments);
  }
})