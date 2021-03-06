/**
 计算机信息tab
 */
 
Ext.define('ncViewer.lanComp',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
    		ncViewer.lanComp.compid='';
    		ncViewer.lanComp.compname='';
        Ext.apply(this,{
        layout : 'border',
        id:'lan_comp',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
//        activeTab: 0,        
        defaults :{
        	bodyPadding: 0
        },
        items: [{
            title: '基本信息', 
            id:'compbaseinfo',           
            html: '<iframe  id="lan_comp_compbaseinfo" name="lan_comp_compbaseinfo" src="/v8/lanComp/lanCompBase.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '硬件信息',
            id:'comphardinfo',            
            html: '<iframe  id="lan_comp_comphardinfo" name="lan_comp_comphardinfo" src="/v8/lanComp/lanCompHard.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '软件信息',
            id:'compsoftinfo',            
            html: '<iframe  id="lan_comp_compsoftinfo" name="lan_comp_compsoftinfo" src="/v8/lanComp/lanCompSoft.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '服务信息',
            id:'compservice',            
            html: '<iframe  id="lan_comp_compservice" name="lan_comp_compservice" src="/v8/lanComp/lanCompService.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }/*,{
            title: '实时进程',
            id:'compssjc',            
            html: '<iframe  id="lan_comp_compssjc" name="lan_comp_compssjc" src="/v8/ssjk/lan_online_proc.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }*/,{
            title: '磁盘信息',
            id:'compdiskinfo',            
            html: '<iframe  id="lan_comp_compdiskinfo" name="lan_comp_compdiskinfo" src="/v8/lanComp/lanCompDisk.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '网络连接信息',
            id:'compnetlinkinfo',            
            html: '<iframe  id="lan_comp_compnetlinkinfo" name="lan_comp_compnetlinkinfo" src="/v8/lanComp/lanCompNet.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }],
        listeners:{
  				'tabchange':function(tp, p){
  					try{
		  				var t_id=p.getId();
		  			  var vframe='lan_comp_'+t_id;
		  			  eval(vframe).reflash();
		  			}catch(e){
		  			
		  			}
  				}
  			}
          
        });
        this.callParent(arguments);
    }
})