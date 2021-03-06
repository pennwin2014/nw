/**
 定义实时监控tab
 */
 
Ext.define('ncViewer.lanSsjk',{
    extend: 'Ext.tab.Panel',
	
    initComponent : function(){
        Ext.apply(this,{
        layout : 'border',
        id:'lay_ssjk',
        margins: '0 0 0 0',
        forceFit: true,
        activeTab: 0,        
         defaults :{
 
            bodyPadding: 0
 
        },
        items: [{
            title: '在线计算机', 
            id:'sjcomputer',           
            html: '<iframe  id="lay_ssjk_sjcomputer" name="lay_ssjk_sjcomputer" src="/v8/ssjk/lan_sjcomputer_list.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },
        {
            title: '实时聊天',
            id:'immon',            
            html: '<iframe  id="lay_ssjk_immon" name="lay_ssjk_immon" src="/v8/ssjk/lan_online_im.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '实时屏幕',
            id:'sspm', 
           // tname:'sspmtree',
          //  handler:onCheckItem,
            html: '<iframe  id="lay_ssjk_sspm" name="lay_ssjk_sspm" src="/v8/ssjk/lan_jiugongge.html" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        },{
            title: '实时进程',
            id:'ssjc',            
            html: '<iframe  id="procy_ssjk_ssjc" name="lay_ssjk_ssjc" src="/v8/ssjk/lan_online_proc.htm" scrolling="no" width=100% height=950  FrameBorder=0 ></iframe>' 
        }]
          ,
  			listeners:{
  				'beforetabchange':function(tp, p){

  					var t_id=p.getId();  				
  					if(t_id=='sjcomputer'){
  						 bStartim=0;
  						 bStartcomp=1;
  					}
  					else if(t_id=='immon'){
  						   bStartim=1;
  						   bStartcomp=0;
  					}
  					else if(t_id=='sspm'){
  						   bStartim=2;
  						   bStartcomp=0;
  					}else if(t_id=='ssjc'){
  						   bStartim=0;
  						   bStartcomp=1;
  	//					 eval("lay_ssjk_sjcomputer").bStart=0;
  	//					 eval("lay_ssjk_immon").bStart=1;
  					}else{
  						   bStartim=0;
  						   bStartcomp=0;
  		//			 eval("lay_ssjk_sjcomputer").bStart=0;
  	//	  			 eval("lay_ssjk_immon").bStart=0;
  					}

						if(t_id=='sspm'){ //改变左边的树
							var tabs_tree=Ext.getCmp('treetab');
							var tab_len=tabs_tree.items.length;
							var tree_panel;
							var tree_flag=0;
							tabs_tree.items.each(function(item) { 
							
							});	
			  			tabs_tree.remove(0);
			 				
							tabs_tree.add({
								closable: false,
								id:'tree_comp_c',
								border:false,
								items:[Ext.create(ncViewer.lanGroupCheckTree)],
								title: '部门电脑'
							}).show();
						}else{ //改变左边的树
							var tabs_tree=Ext.getCmp('treetab');
							var tab_len=tabs_tree.items.length;
							var tree_panel;
							tabs_tree.items.each(function(item) { 
								if(item.id=='tree_comp'){
									tree_flag=1;
								}
							});	
							if(tree_flag==1) 
								return;
			      
			  			tabs_tree.remove(0);
			 				
							tabs_tree.add({
								closable: false,
								id:'tree_comp',
								border:false,
								items:[Ext.create(ncViewer.lanGroupTree)],
								title: '部门电脑'
							}).show();
						}
  				}
  			}
  		});
        this.callParent(arguments);

    }
})