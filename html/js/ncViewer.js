


Ext.Loader.setConfig({enabled: true,paths:{'ncViewer':'/v8/js'}});
Ext.define('ncViewer.App', {
    extend: 'Ext.container.Viewport',    
    uses:['ncViewer.ncGridUser','ncViewer.lanProcy','ncViewer.lanSsjk','ncViewer.lanRzck','ncViewer.lanComp','ncViewer.lanBase','ncViewer.lanProcyXf','ncViewer.lanSystem','ncViewer.lanSoftUpdate','ncViewer.lanDeptment','ncViewer.lanGroupTree','ncViewer.lanGroupCheckTree','ncViewer.lanSysSet','ncViewer.lanInstall','ncViewer.lanOutfile'],
     initComponent: function(){
        Ext.define('State', {
            extend: 'Ext.data.Model',
            fields: ['groupid', 'groupname']
        });       
        Ext.apply(this, {
            layout: {
                type: 'border',
                padding: 2
            },
            items: [this.createNcTb(),this.createNcLeft(),this.createNcCenter(),this.createNcStatus()]
        });
        this.callParent(arguments);
    },
  
    /**
    ����������
     */
    createNcTb: function(){
			
        var right_fun_s=right_fun;
 //�������Բ˵�
/*   var menu_procy = Ext.create('Ext.menu.Menu', {
        id: 'sfkglMenu',
        style: {
            overflow: 'visible'     
        },
        items: [{
          text: '���Կ���',
          itemid:'kzcl',
          srcurl:'ncViewer.lanProcy',  
          handler:this.onItemClick
       	},{
					text: '�����·�',
					itemid:'clxf',
					srcurl:'ncViewer.lanProcyXf',
					handler:this.onItemClick
        },{
					text: '�ڰ�����',
					itemid:'blacklist',
					srcurl:'ncViewer.lanBlackList',
					handler:this.onItemClick
        }]
    });
  
    menu_procy.items.each(function(item){
			if(right_fun_s.indexOf(item.text)== -1){
				item.destroy();
			}
		});
*/

 //�����������ݲ˵�
   var menu_base = Ext.create('Ext.menu.Menu', {
        id: 'baseMenu',
        style: {
            overflow: 'visible'  
        },
        items: [
          {
              text: '�ͻ�����Ϣ',
              itemid:'clinetinfo',
              srcurl:'ncViewer.lanClient',  
              handler:this.onItemClick
       
           },
           {
           	    text: '���Ź���',
                itemid:'deptment',
                srcurl:'ncViewer.lanDeptment',
               handler:this.onItemClick
           },
           {
           	    text: 'Ԥ�������',
                itemid:'baseinfo',
               srcurl:'ncViewer.lanBase',
               handler:this.onItemClick
           }
        ]
    });

		menu_base.items.each(function(item){
			if(right_fun_s.indexOf(item.text)== -1){
				item.destroy();
			}
		});
    	 var tb = Ext.create('Ext.toolbar.Toolbar',{ style: 'background-image:url(/images/large/toolbarbackground.png);',id: "maintab"});
    	    tb.add({
	    	    itemid:'logo',
       		  xtype:'label',
        		html:'&nbsp;<img src="/images/v8/logo_cent_2.png" width="196" height="60">'
         	},{
            text: '��ҳ',
            itemid:'yxfx',
            scale:'large',
            iconAlign: 'top',
            srcurl:'widget.ncCharPoral',
            iconCls: 'dvd',
            handler:this.onItemClick
        
       },'-',{
            itemid:'ssjk',
            hidden:false,
            text:'ʵʱ���',
            scale:'large',
            iconCls: 'ssjk',
            iconAlign: 'top',
            srcurl:'ncViewer.lanSsjk',
            handler:this.onItemClick
        },'-',{
            itemid:'procy',
            hidden:false,
            text:'���Ʋ���',
            scale:'large',
            iconCls: 'kzcl',
            iconAlign: 'top',
//            menu:menu_procy
            srcurl:'ncViewer.lanProcy',
            handler:this.onItemClick
        },'-',{
            itemid:'rzck',
            hidden:false,
            text:'��־�鿴',
            scale:'large',
            iconCls: 'rzck',
            iconAlign: 'top',
            srcurl:'ncViewer.lanRzck',
            handler:this.onItemClick
        }
        ,'-',{
            itemid:'outfile',
            hidden:false,
            text:'�ⷢ�ļ�',
            scale:'large',
            iconCls: 'rzck',
            iconAlign: 'top',
            srcurl:'ncViewer.lanOutfile',
            handler:this.onItemClick
        }
        ,'-',{
            itemid:'rztj',
            hidden:false,
            text:'��־ͳ��',
            scale:'large',
            iconCls: 'rzck',
            iconAlign: 'top',
            srcurl:'ncViewer.lanRztj',
            handler:this.onItemClick
        },'-',{
            itemid:'comp',
            hidden:false,
            text:'�������Ϣ',
            scale:'large',
            iconCls: 'computer',
            iconAlign: 'top',
            srcurl:'ncViewer.lanComp',
            handler:this.onItemClick
        }, '-',{
            itemid:'jcxxgl',
            hidden:false,
            text:'������Ϣ',
            scale:'large',
            iconCls: 'macs',
            menu:menu_base,
            iconAlign: 'top'
 //           srcurl:'ncViewer.lanBase',
 //           handler:this.onItemClick
        }, '-',{
            itemid:'system',
            hidden:false,
            text:'ϵͳ����',
            scale:'large',
            iconCls: 'address',
            iconAlign: 'top',
            srcurl:'ncViewer.lanSystem',
            handler:this.onItemClick
        }, '-',{
            itemid:'softupdate',
            hidden:false,
            text:'�ͻ�������',
            scale:'large',
            iconCls: 'add_sf',
            iconAlign: 'top',
            srcurl:'ncViewer.lanSoftUpdate',
            handler:this.onItemClick
        } , '-',{
            itemid:'sysset',
            hidden:false,
            text:'ϵͳ����',
            scale:'large',
            iconCls: 'dvd',
            iconAlign: 'top',
            srcurl:'ncViewer.lanSysSet',
            handler:this.onItemClick
        },'-',{
            itemid:'instalload',
            hidden:false,
            text:'��װ������',
            scale:'large',
            iconCls: 'download',
            iconAlign: 'top',
            srcurl:'ncViewer.lanInstall',
            handler:this.onItemClick
        } , '-','-','->','->',{
            text: 'ϵͳˢ��',
            itemid:'xtsx',
            scale:'large',
            iconAlign: 'top',
            iconCls: 'refresh',
            handler: this.onItemReflash
        },'-',{
            text: '���µ�¼',
            scale:'large',
            itemid:'cxdl',
            iconAlign: 'top',
            iconCls: 'unlock',
          handler: this.onItemClose
        }); 
        
        //console.info(right_fun_s);
         var isep=0;

         tb.items.each(function(item){
//      console.info(right_fun_s);
      	if((right_fun_s.indexOf(item.text)!=-1)||(item.text=='ϵͳˢ��')||(item.text=='���µ�¼')||item.itemid=='logo'){
      	
      	    item.hidden=false;
      	    isep=1;
      	 
      	}
      	else{
      		if(item.text){
//   		console.info(item.text);
      			item.hidden=true;
      			isep=0;
      		
      		}
      		else{
      			
      			if(isep==0){
      			 item.hidden=true;
      			 isep=1;
      			}
      			else{
      				item.hidden=false;
      				isep=0;
      			}
      		}
      	
      	}

      	 
      	});  
  	
      this.toolPanel = Ext.create(Ext.panel.Panel, {
             id: 'header',
             margins: '0 2 0 2',
             border: true,
             region: 'north',
             items:[tb]
          
        });
        return this.toolPanel;
       
    },
    onItemClose:function(){
    	self.location='/login2.htm';
    },
    onItemReflash:function(){
          var fp=Ext.create('Ext.FormPanel');
               fp.form.doAction('submit',{
               url:'/pronline/Msg',
                method:'POST',
               params:{FunName:'ncsSysReflesh_v4'},
                 
                success:function(form,action){ 
           					Ext.getCmp('winstatus').close();
                 },
             		 failure:function(form,action){        		 
                    Ext.Msg.alert('����',"ϵͳ��æ,�Ժ�����!");
                    Ext.getCmp('winstatus').close();
            		 }
                }
             )
					    var wint = Ext.create('Ext.Window', {
					 //       title: 'ϵͳˢ��',
					        id:'winstatus',
					        header:false,
					        width: 150,
					        x:600,
					        y:210,
					        height: 40,
					        closable: false,
					        modal: true,
					  //      closeAction: 'hide',
					        bodyPadding: 10,
					        bbar: Ext.create('Ext.ux.StatusBar', {
					            id: 'win-statusbar',
					            defaultText: 'ϵͳ����'
					        })
					    });					   
					    wint.show();
	//				    loadFn('win-button', 'win-statusbar');
    
    },
    createNcLeft:function(){   	
    	 var left_tabs=Ext.createWidget('tabpanel', {
        width: 290,
        border:false,
         margins: '0 0 0 0',
//        height:970,
        id:'treetab',    
        activeTab: 0,
         defaults :{
            bodyPadding: 0
        }
     });
     	left_tabs.add({
						          closable: false,
					             border:false,
                        items:[Ext.create(ncViewer.lanGroupTree)],
						            title: '���ŵ���'
		 });
     
     var formTmp = Ext.create('Ext.form.Panel');
      
     this.leftPanel=Ext.create(Ext.panel.Panel, {
             id: 'left',
             margins: '0 2 0 2',
             
             layout: 'border',
              title:'��ǰλ�ã�'+"ʵʱ���",
             region: 'west',
             collapsible: true,
             autoScroll: true,
             
             bodyStyle: 'background:#fff;',
             split:true,
             width: 210,
	            minSize: 100,
	            maxSize: 500,
             items:[left_tabs],
             bbar:[{
             	xtype: "button",
             	text: "�������",
             	width: "30%",
             	id: 'treeComp',
             	handler: function(Btn){
             		formTmp.submit({
             			url:'/pronline/Msg',
             			method:'post',
             			params:{FunName:"lanParmSet_Save",filedname:'compname'},
             			success:function(form,action){
             				Ext.getCmp('treeBtn').fireEvent('click');
         						Btn.addClass("treeBtnCls");
         						Ext.getCmp("treeIP").removeCls("treeBtnCls");
         						Ext.getCmp("treeUser").removeCls("treeBtnCls");
         						eval("lay_ssjk_sjcomputer").reflash();
         						eval("lay_ssjk_sspm").reflash();
             			}
             		});
             	}
             },{
             	xtype: "button",
             	text: "IP",
             	width: "30%",
             	id: "treeIP",
             	handler: function(Btn){
             		formTmp.submit({
             			url:'/pronline/Msg',
             			method:'post',
             			params:{FunName:"lanParmSet_Save",filedname:'ip'},
             			success:function(form,action){
             				Ext.getCmp('treeBtn').fireEvent('click');
         						Btn.addClass("treeBtnCls");
         						Ext.getCmp("treeComp").removeCls("treeBtnCls");
         						Ext.getCmp("treeUser").removeCls("treeBtnCls");
         						eval("lay_ssjk_sjcomputer").reflash();
         						eval("lay_ssjk_sspm").reflash();
             			}
             		});
             	}
             },{
             	xtype: "button",
             	text: "�û���",
             	width: "30%",
             	id: "treeUser",
             	handler: function(Btn){
             		formTmp.submit({
             			url:'/pronline/Msg',
             			method:'post',
             			params:{FunName:"lanParmSet_Save",filedname:'devname'},
             			success:function(form,action){
             				Ext.getCmp('treeBtn').fireEvent('click');
         						Btn.addClass("treeBtnCls");
         						Ext.getCmp("treeIP").removeCls("treeBtnCls");
         						Ext.getCmp("treeComp").removeCls("treeBtnCls");
         						eval("lay_ssjk_sjcomputer").reflash();
         						eval("lay_ssjk_sspm").reflash();
             			}
             		});
             	}
             }]
          
        });
        Ext.define('globparm', {
	        extend: 'Ext.data.Model',
	        fields: [
	            {name: 'filedname',type: 'string'}
	        ]
	    });
        var store = Ext.create('Ext.data.Store', {
	        model: 'globparm',
	        autoLoad: true,
	        remoteSort: true,
	        proxy: {
	            type: 'ajax',
	            url: '/pronline/Msg?FunName@lanParmSet',
	            reader: {
	                type:'json',
	                root: 'eimdata',
	                totalProperty: 'totalCount'
	            },         
	            simpleSortMode: true
	        },
	        listeners:{
	        	'load': function(thisStore, records, successful, operation, eOpts ){
	        	//alert(this.getAt(0).get('filedname'));
	        	if(thisStore){
  	        	var record = thisStore.getAt(0);
  	        	if(record){
    	        	if(record.get('filedname')=="devname"){
    	        		Ext.getCmp("treeUser").addCls("treeBtnCls");
    	        	}else if(record.get('filedname')=="ip"){
    	        		Ext.getCmp("treeIP").addCls("treeBtnCls");
    	        	}else if(record.get('filedname')=="compname"){
    	        		Ext.getCmp("treeComp").addCls("treeBtnCls");
    	        	}
    	        }
    	      }
	        }}
				});
				store.load();
        
       return this.leftPanel;
    },
    createNcCenter:function(){

//   var charPort=Ext.create('widget.ncCharPoral');
      groupid='';
      compid='';
      dxtitle='';
    	this.centerPanel=Ext.createWidget('tabpanel', {
        resizeTabs: true,
        enableTabScroll: true,
          region:'center',
        	id:'layout_center',
        	margins: '2 2 1 0',
        defaults: {
            autoScroll: false,
            bodyPadding: 0
        }/*,
        items: [
        {
        	  closable: true,
            autoScroll:true,
            id:'ssjk',
            iconCls: 'add16',          
            title: 'ʵʱ���',
            items:[Ext.create(ncViewer.lanSsjk)]
        }]*/,
        plugins: Ext.create('Ext.ux.TabCloseMenu', {
            extraItemsTail: [
                '-',
                {
                    text: 'Closable',
                    checked: true,
                    hideOnClick: true,
                    handler: function (item) {
                        currentItem.tab.setClosable(item.checked);
                    }
                },
                '-',
                {
                    text: 'Enabled',
                    checked: true,
                    hideOnClick: true,
                    handler: function(item) {
                        currentItem.tab.setDisabled(!item.checked);
                    }
                }
            ],
            listeners: {
                aftermenu: function () {
                    currentItem = null;
                },
                beforemenu: function (menu, item) {
                    menu.child('[text="Closable"]').setChecked(item.closable);
                    menu.child('[text="Enabled"]').setChecked(!item.tab.isDisabled());

                    currentItem = item;
                }
            }
         }),
         listeners: {              
            tabchange:function(tp,p){
              var tabs_center=Ext.getCmp("layout_center");
        	    var active = tabs_center.getActiveTab();
              //console.log(active.getId());
		        	var active_id='lan_'+active.id;
		        	//alert(active_id);
		        	var tabs_if=Ext.getCmp(active_id).getActiveTab();
		        	if(tabs_if){
			          if(tabs_if.id=='immon'){
			           	bStartcomp=0;
			           	bStartim=1;
			          }
			          else if(tabs_if.id=='sjcomputer'){
			          	bStartcomp=1;
			          	bStartim=0;
			          }
          		  else if(tabs_if.id=='sspm'){
          		 	  bStartcomp=0;
			          	bStartim=0;
          		  }
          		  var vframe= active_id+"_"+tabs_if.getId()
          		  //alert(vframe);
          		  try{
            		  eval(vframe).reflash();
            		}catch(e){}
          		}
          	}
         }
                
       });
       var right_fun_s=right_fun;
       var item2 = Ext.getCmp("maintab").items.items;
//console.info(right_fun_s);      
      for(var i=0;i<item2.length;i++){
    //  	console.info(item2[i].text);  
      	
        //console.log(item2[i]);
        try{
          if((right_fun_s.indexOf(item2[i].text)!=-1)){
        	  if(item2[i].itemid=="jcxxgl"){
        	    continue;
        	  }
            this.onItemClick(item2[i]);
            break;
        	}
        }catch(e){}  
      	
      }
       
      
/*
			this.onItemClick({
				itemid:'ssjk',
        text:'ʵʱ���',
        srcurl:'ncViewer.lanSsjk'
			});
*/      
       
       return this.centerPanel;
    },
    createNcStatus:function(){   	
    	this.ncstatus=Ext.create('Ext.ux.StatusBar', {
        id: 'statusbar',
        region: 'south',
        margins: '1 2 0 2',
        defaultText: 'Default status text',
        text: 'Ready',
        iconCls: 'x-status-valid',
        items: [{
          xtype: 'button',
          text: ''
        },{
          xtype: 'button',
          text: '�Ϻ���������Ϣ�����ɷ����޹�˾&nbsp;&nbsp;',
          handler: function (){
            var sb = Ext.getCmp('statusbar');
            sb.setStatus({
							text: 'Oops!',
							iconCls: 'x-status-error',
							clear: true // auto-clear after a set interval
            });
          }
        }]
      });
      return this.ncstatus;
    },
    onItemClick:function(item){
    	 //bbbb=document.all('ltitle');
       Ext.getCmp('left').setTitle('��ǰλ�ã�'+item.text);
       //bbbb.innerHTML='��ǰλ�ã�'+item.text;
       
       
       
        //�ı���ߵ���
     var tabs_tree=Ext.getCmp('treetab');
     var tab_len=tabs_tree.items.length;
     var tree_panel;
     var tree_flag=0;
     tabs_tree.items.each(function(item) { 
			if(item.id=='tree_comp'){
						   		tree_flag=1;
						   	
						   	}
      });	
      if(tree_flag!=1){
      
      
	  tabs_tree.remove(0);
	 				
		tabs_tree.add({
			closable: false,
			id:'tree_comp',
			border:false,
			items:[Ext.create(ncViewer.lanGroupTree)],
			title: '���ŵ���'
		}).show();
       
     }
       
 //�ı��м����ͼ      
       
       var i=0;
       var m=0;
       var tab_title=item.text;
       var tab_src=item.srcurl;
       var tab_id=item.itemid;
       var tabs_center=Ext.getCmp("layout_center");
       var tab_len=tabs_center.items.length;
       tabs_center.items.each(function(item) { 
	     	 if(item.id==tab_id)
	     	 { i++;}
	       if(i==0){m++;}
       });   
      if(i>0){
   	  	tabs_center.setActiveTab(m);
   	  	return;
     		
     	} 
      if(tab_len>5){
      	tabs_center.remove(1);  
      }  

      var grid=Ext.create(tab_src);
      
      //Ȩ�޴���
      var str = right_fun;
      
      grid.items.each(function(item){
 //   console.info(str);
 //   console.info(item.getId());
 //   console.info(str.indexOf(item.getId()));
      	
      	if(str.indexOf(item.getId())== -1){
      		item.destroy();
      	}
      });
      
      tabs_center.add({
        closable: true,
        autoScroll:true,
        id:tab_id,
        iconCls: 'add16',          
        title: tab_title,
        items:[grid]
      }).show();
     
    },
    showTabs:function(parentTab,childTab){
    	var i=0;
			var m=0;
			var tab_title=parentTab.text;
			var tab_src=parentTab.srcurl;
			var tab_id=parentTab.itemid;
			var tabs_center=Ext.getCmp("layout_center");
			var tab_len=tabs_center.items.length;
			tabs_center.items.each(function(item) { 
				if(item.id==tab_id){
					i++;
				}
				if(i==0){
					m++;
				}
			});
      if(i>0){
      	tabs_center.remove(m);
     	} 
      if(tab_len>5){
      	tabs_center.remove(1);  
      }

      var grid=Ext.create(tab_src,{activeTab:childTab});
      
      //Ȩ�޴���
      var str = right_fun;
    
      grid.items.each(function(item){
//console.info(str,",",item.getId(),",",str.indexOf(item.getId()));
      	if(str.indexOf(item.getId())== -1){
      		item.destroy();
      	}
      });
      
      tabs_center.add({
        closable: true,
        autoScroll:true,
        id:tab_id,
        iconCls: 'add16',          
        title: tab_title,
        items:[grid]
      }).show();
      //Ext.getCmp('lay_'+tab_id).setActiveTab(childTab);
    },
    refreshComTree: function(){
      Ext.getCmp('treeBtn').fireEvent('click');
    }

 
});
