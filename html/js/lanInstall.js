/**
 定义安装包下载tab
 */
 
Ext.define('ncViewer.lanInstall',{
    extend: 'Ext.panel.Panel',
    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lan_instalload',
        margins: '0 0 0 0',
        forceFit: true,
        title: '安装包下载',          
        html: '<iframe  id="lan_instalload" name="lan_instalload" src="/v8/softupdate/lan_setup.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>',        
        defaults :{
        	bodyPadding: 0
        }          
    });
        this.callParent(arguments);
    }
})