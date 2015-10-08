/**
 �������tab
 */
 
Ext.define('ncViewer.lanBase',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lan_base',
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
        //activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '������Ͷ���', 
            id:'soft',           
            html: '<iframe  id="lan_base_soft" name="lan_base_soft" src="/v8/lanBase/lanBaseSoft.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: 'ʱ�������',
            id:'timeobj',            
            html: '<iframe  id="lan_base_timeobj" name="lan_base_timeobj" src="/v8/lanBase/lanBaseTime.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '����ʱ�䶨��',
            id:'worktime',            
            html: '<iframe  id="lan_base_worktime" name="lan_base_worktime" src="/v8/lanBase/lanBaseWorkTime.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: 'USB���Ͷ���',
            id:'usb',            
            html: '<iframe  id="lan_base_usb" name="lan_base_usb" src="/v8/lanBase/lanBaseUSB.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '������������',
            id:'params',            
            html: '<iframe  id="lan_base_params" name="lan_base_params" src="/v8/lanBase/lanBaseParams.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '���Գ���',
            id:'ignore',            
            html: '<iframe  id="lan_base_ignore" name="lan_base_ignore" src="/v8/lanBase/lanIgnoreProc.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }]
          
      });
      this.callParent(arguments);
    }
})