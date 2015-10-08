/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanSoftUpdate',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lay_softupdate',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '软件上传', 
            id:'updatesoft',           
            html: '<iframe  id="lay_softupdate_updatesoft" name="lay_softupdate_updatesoft" src="/v8/softupdate/lan_softupload_list.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '软件下发',
            id:'softdown',            
            html: '<iframe  id="lay_softupdate_softdown" name="lay_softupdate_softdown" src="/v8/softupdate/lan_softupdate_xf.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '升级日志',
            id:'downlog',            
            html: '<iframe  id="lay_softupdate_downlog" name="lay_softupdate_updatesoft" src="/v8/softupdate/lan_softupdate_log.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }/*,{
            title: '安装包生成',
            id:'setup',            
            html: '<iframe  id="lay_softupdate_setup" name="lay_softupdate_setup" src="/v8/softupdate/lan_setup.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }*/]
          
        });
        this.callParent(arguments);
    }
})