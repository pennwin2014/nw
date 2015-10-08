 
Ext.define('ncViewer.lanSysSet',{
    extend: 'Ext.tab.Panel',
    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lan_set',
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
            html: '<iframe  id="lan_ipadress" name="lan_ipadress" src="/v8/lanset/lan_ipadress_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '数据存储设置',
            id:'dataclean',            
            html: '<iframe  id="lan_dataclean" name="lan_dataclean" src="/v8/lanset/lan_dataclean_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '备份设置',
            id:'databack',            
            html: '<iframe  id="lan_databak" name="lan_databak" src="/v8/lanset/lan_databk_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '电脑树显示字段',
            id:'dispfield',            
            html: '<iframe  id="lan_dispfiled" name="lan_dispfiled" src="/v8/lanset/lan_field_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '关机和重启',
            id:'shutdown',            
            html: '<iframe  id="lan_dispfiled" name="lan_shutdown" src="/v8/lanset/lan_shutdown_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '其他',
            id:'otherPennwin',            
            html: '<iframe  id="lanOtherPennwin" name="lan_otherPennwin" src="/v8/lanset/lan_other_pennwin.htm" scrolling="no" width=100% height=100%  FrameBorder=0 ></iframe>' 
        }]
          
        });
        this.callParent(arguments);
    }
})