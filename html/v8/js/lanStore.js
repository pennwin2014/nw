/**
 定义策略tab
 */
 
Ext.define('ncViewer.lanRzck',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
    	  ncViewer.lanRzck.compid = "";
        ncViewer.lanRzck.compname = "";
        
        Ext.apply(this,{
        layout : 'border',
        id:'lay_rzck',
        forceFit: true,
        margins: '0 0 0 0',
 //       frames:true,
        forceFit: true,
 //       activeTab: 0,        
         defaults :{ 
            bodyPadding: 0
        },
        items: [{
            title: '聊天内容', 
            id:'imcontent',           
            html: '<iframe  id="lay_rzck_imcontent" name="lay_rzck_imcontent" src="/v8/rzck/chatLog.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '屏幕日志',
            id:'screenlog',            
            html: '<iframe  id="lay_rzck_screenlog" name="lay_rzck_screenlog" src="/v8/rzck/pingmurizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '网站访问日志',
            id:'urllog',            
            html: '<iframe  id="lay_rzck_urllog" name="lay_rzck_urllog" src="/v8/rzck/wangyerizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '进程使用日志',
            id:'proclog',            
            html: '<iframe  id="lay_rzck_proclog" name="lay_rzck_proclog" src="/v8/rzck/chengxurizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '外设使用日志',
            id:'waishelog',            
            html: '<iframe  id="lay_rzck_waishelog" name="lay_rzck_waishelog" src="/v8/rzck/waisherizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '文件操作日志',
            id:'filelog',            
            html: '<iframe  id="lay_rzck_filelog" name="lay_rzck_filelog" src="/v8/rzck/ncs_wenjian.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '告警日志',
            id:'warnlog',            
            html: '<iframe  id="lay_rzck_warnlog" name="lay_rzck_warnlog" src="/v8/rzck/gaojingrizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }],
  			listeners:{
  				'tabchange':function(tp, p){
  				var t_id=p.getId();
  			  var vframe='lay_rzck_'+t_id;
  			  eval(vframe).reflash();
  				}
  			} 
        });
        this.callParent(arguments);
    }
})