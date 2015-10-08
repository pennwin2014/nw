/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanRztj',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
    	  ncViewer.lanRztj.compid = "";
        ncViewer.lanRztj.compname = "";
        
        Ext.apply(this,{
        layout : 'border',
        id:'lan_rztj',
        forceFit: true,
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
 //       activeTab: 0,        
         defaults :{ 
            bodyPadding: 0
        },
        items: [{
            title: '开关机统计',
            id:'compStatics',
            html: '<iframe  id="lan_rztj_compStatics" name="lan_rztj_compStatics" src="/v8/rztj/compStatics.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '网站统计',
            id:'webStatics',
            html: '<iframe  id="lan_rztj_webStatics" name="lan_rztj_webStatics" src="/v8/rztj/webStatics.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '程序统计',
            id:'processStatics',
            html: '<iframe  id="lan_rztj_processStatics" name="lan_rztj_processStatics" src="/v8/rztj/processStatics.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }],
  			listeners:{
  				'tabchange':function(tp, p){
  					try{
		  				var t_id=p.getId();
		  			  var vframe='lan_rztj_'+t_id;
		  			  eval(vframe).reflash();
		  			}catch (e){
		  				
		  			}
  				}
  			} 
        });
        this.callParent(arguments);
    }
})