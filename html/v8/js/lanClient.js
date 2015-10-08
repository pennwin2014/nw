
Ext.define('ncViewer.lanClient',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
	    	id:'lan_clinetinfo',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '客户端管理', 
            id:'client',           
            html: '<iframe  id="lan_clinetinfo_client" name="lan_clinetinfo_client" src="/v8/lanClient/lanClient.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }
        ]
          
        });
        this.callParent(arguments);
    }
})