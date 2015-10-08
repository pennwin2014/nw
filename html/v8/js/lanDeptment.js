/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanDeptment',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lan_deptment',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '部门管理', 
            id:'dept',           
            html: '<iframe  id="lan_deptment_dept" name="lan_deptment_dept" src="/v8/lanDeptment/lanDeptment.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }
        ]
          
        });
        this.callParent(arguments);
    }
})