/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanProcyXf',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lay_clxf',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '策略下发', 
            id:'xfcl',           
            html: '<iframe  id="lay_clxf_xfcl" name="lay_clxf_xfcl" src="/v8/lanprocy/lan_procy_clxf.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '按对象查看策略',
            id:'lookProcy',            
            html: '<iframe  id="lay_clxf_lookProcy" name="lay_clxf_lookProcy" src="/v8/lanprocy/lan_procy_lookProcy.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }
        ]
          
        });
        this.callParent(arguments);
    }
})