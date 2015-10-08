 
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
            title: 'IP��ַ����', 
            id:'ipset',           
            html: '<iframe  id="lan_ipadress" name="lan_ipadress" src="/v8/lanset/lan_ipadress_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '���ݴ洢����',
            id:'dataclean',            
            html: '<iframe  id="lan_dataclean" name="lan_dataclean" src="/v8/lanset/lan_dataclean_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '��������',
            id:'databack',            
            html: '<iframe  id="lan_databak" name="lan_databak" src="/v8/lanset/lan_databk_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '��������ʾ�ֶ�',
            id:'dispfield',            
            html: '<iframe  id="lan_dispfiled" name="lan_dispfiled" src="/v8/lanset/lan_field_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�ػ�������',
            id:'shutdown',            
            html: '<iframe  id="lan_dispfiled" name="lan_shutdown" src="/v8/lanset/lan_shutdown_set.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '����',
            id:'otherPennwin',            
            html: '<iframe  id="lanOtherPennwin" name="lan_otherPennwin" src="/v8/lanset/lan_other_pennwin.htm" scrolling="no" width=100% height=100%  FrameBorder=0 ></iframe>' 
        }]
          
        });
        this.callParent(arguments);
    }
})