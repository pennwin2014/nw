 
Ext.define('ncViewer.lanSysSet',{
    extend: 'Ext.tab.Panel',
    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lay_set',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: 'IP地址配置', 
            id:'ipset',           
            html: '<iframe  id="lay_ipadress" name="lay_ipadress" src="/v8/lanset/lan_ipadress_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '数据存储设置',
            id:'dataclean',            
            html: '<iframe  id="lay_dataclean" name="lay_dataclean" src="/v8/lanset/lan_dataclean_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '备份设置',
            id:'databack',            
            html: '<iframe  id="lay_databak" name="lay_databak" src="/v8/lanset/lan_databk_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '电脑树显示字段',
            id:'dispfield',            
            html: '<iframe  id="lay_dispfiled" name="lay_dispfiled" src="/v8/lanset/lan_field_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '关机和重启',
            id:'shutdown',            
            html: '<iframe  id="lay_dispfiled" name="lay_shutdown" src="/v8/lanset/lan_shutdown_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }]
          
        });
        this.callParent(arguments);
    }
})