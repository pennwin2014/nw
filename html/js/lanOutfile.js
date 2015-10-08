/**
 �������tab
 */
 
Ext.define('ncViewer.lanOutfile',{
    extend: 'Ext.tab.Panel',

    initComponent : function(){
    	//cViewer.lanOutfile.compid = "";
        //ncViewer.lanOutfile.compname = "";
        
        Ext.apply(this,{
        layout : 'border',
        id:'lan_outfile',
        forceFit: true,
        margins: '0 0 0 0',
 //       frames:true,
 //       activeTab: 0,        
         defaults :{ 
            bodyPadding: 0
        },
        items: [{
            title: '�ļ��ⷢ��־',
            id:'outfilelog',            
            html: '<iframe  id="lan_outfile_log" name="lan_outfile_log" src="/v8/outfile/lan_outfile_log.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�ⷢ�ļ����ݲ�ѯ',
            id:'filecontent',            
            html: '<iframe  id="lan_outfile_content" name="lan_rzck_warnlog" src="/v8/rzck/gaojingrizhi.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�ļ��ⷢ�����·�',
            id:'outfiletask',            
            html: '<iframe  id="lan_outfile_task" name="lan_outfile_task" src="/v8/outfile/lan_outfile_task.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�ļ��ⷢ���Բ鿴',
            id:'outfilelaw',            
            html: '<iframe  id="lan_outfile_law" name="lan_outfile_law" src="/v8/outfile/lan_outfile_law.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '�ļ��ⷢ�����ѯ',
            id:'outfileresult',            
            html: '<iframe  id="lan_outfile_resoult" name="lan_outfile_resoult" src="/v8/outfile/lan_outfile_resoult.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }
        ],
  			listeners:{
  				'tabchange':function(tp, p){
  					try{
		  				var t_id=p.getId();
		  			  var vframe='lan_outfile_'+t_id;
		  			  eval(vframe).reflash();
		  			}catch (e){
		  				
		  			}
  				}
  			} 
        });
        this.callParent(arguments);
    }
})